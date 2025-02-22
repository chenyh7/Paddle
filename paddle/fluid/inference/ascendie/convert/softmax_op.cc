/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <algorithm>

#include "paddle/fluid/inference/ascendie/convert/op_converter.h"

namespace paddle {
namespace inference {
namespace ascendie {

/*
 * SoftMaxOp, ISoftMaxLayer in TRT. This Layer doesn't has weights.
 */
class SoftMaxOpConverter : public OpConverter {
 public:
  void operator()(const framework::proto::OpDesc& op,
                  const framework::Scope& scope,
                  bool test_mode) override {
    VLOG(3) << "convert a softmax op to ascendie softmax layer without bias";
    framework::OpDesc op_desc(op, nullptr);
    // Declare inputs
    AscendIE::Tensor* input1 = engine_->GetITensor(op_desc.Input("X")[0]);
    AscendIE::Dims input_shape = input1->GetDimensions();
    int input_dims = input_shape.Size();
    int axis = op_desc.HasAttr("axis") ? PADDLE_GET_CONST(int, op_desc.GetAttr("axis")) : -1;

    // support 0 or 1 dims input
    bool is_0_dims = input_dims == 0;
    bool is_1_dims = input_dims == 1;
    if (is_0_dims || is_1_dims) {
      AscendIE::ShuffleLayer *reshaped_layer = engine_->network()->AddShuffle(input1);

      int64_t shape[2] = {1, 0};
      shape[1] = is_0_dims ? 1 : input_shape[0];
      AscendIE::Dims reshaped_dims(2, shape);
    
      reshaped_layer->SetReshapeDimensions(reshaped_dims);
      input1 = reshaped_layer->GetOutput(0);
      input_shape = input1->GetDimensions();
      input_dims = input_shape.Size();
      axis = -1;
    }

    AscendIE::SoftMaxLayer* layer = engine_->network()->AddSoftMax(input1);

    uint32_t axes = std::max(0, input_dims - 3);
    // TODO(cryoco): Poor workaround. Fix padded dims problem when AIE layers
    // support Nd.
    // Tips: Dynammic shape alreay fixes.
    int padded_dims = 0;
    int explicit_batch = 0;
    if (engine_->with_dynamic_shape()) explicit_batch = 1;
    for (int i = input_dims - 1; i > explicit_batch; i--) {
      if (input_shape[i] == 1) {
        padded_dims += 1;
      } else {
        break;
      }
    }
    if (!engine_->with_dynamic_shape()) {
      if (axis < 0) {
        axes = input_dims + axis - padded_dims;
      } else {
        axes = axis - 1;
      }
    } else {
      if (axis < 0) {
        axes = input_dims + axis;
      } else {
        axes = axis;
      }
    }
    layer->SetAxes(1 << axes);

    // The trt will not run int for softmax.
    engine_->SetTensorDynamicRange(input1, 1.0);
    auto output_name = op_desc.Output("Out")[0];

    // support 0 or 1 dims input
    if (is_0_dims || is_1_dims) {
      AscendIE::ShuffleLayer *reshaped_layer = engine_->network()->AddShuffle(layer->GetOutput(0));

      reshaped_layer->SetReshapeDimensions(engine_->GetITensor(op_desc.Input("X")[0])->GetDimensions());
      RreplenishLayerAndOutput( reshaped_layer, "reshape_softmax_reshape", {output_name}, test_mode);
    } else {
      RreplenishLayerAndOutput(layer, "softmax", {output_name}, test_mode);
    }
  }
};

}  // namespace ascendie
}  // namespace inference
}  // namespace paddle

USE_OP_ITSELF(softmax);
REGISTER_ASCEND_OP_CONVERTER(softmax, SoftMaxOpConverter);
