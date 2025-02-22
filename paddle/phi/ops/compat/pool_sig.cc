// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/phi/core/compat/op_utils.h"

namespace phi {

KernelSignature Pool2dOpArgumentMapping(
    const ArgumentMappingContext& ctx UNUSED) {
  return KernelSignature("pool2d",
                         {"X"},
                         {"ksize",
                          "strides",
                          "paddings",
                          "ceil_mode",
                          "exclusive",
                          "data_format",
                          "pooling_type",
                          "global_pooling",
                          "adaptive",
                          "padding_algorithm"},
                         {"Out"});
}

KernelSignature Pool2dGradOpArgumentMapping(
    const ArgumentMappingContext& ctx UNUSED) {
  return KernelSignature("pool2d_grad",
                         {"X", "Out", "Out@GRAD"},
                         {"ksize",
                          "strides",
                          "paddings",
                          "ceil_mode",
                          "exclusive",
                          "data_format",
                          "pooling_type",
                          "global_pooling",
                          "adaptive",
                          "padding_algorithm"},
                         {"X@GRAD"});
}

KernelSignature Pool2dDoubleGradOpArgumentMapping(
    const ArgumentMappingContext& ctx UNUSED) {
  return KernelSignature("pool2d_double_grad",
                         {"X"},
                         {"ksize",
                          "strides",
                          "paddings",
                          "ceil_mode",
                          "exclusive",
                          "data_format",
                          "pooling_type",
                          "global_pooling",
                          "adaptive",
                          "padding_algorithm"},
                         {"Out"});
}

KernelSignature Pool3dOpArgumentMapping(
    const ArgumentMappingContext& ctx UNUSED) {
  return KernelSignature("pool3d",
                         {"X"},
                         {"ksize",
                          "strides",
                          "paddings",
                          "ceil_mode",
                          "exclusive",
                          "data_format",
                          "pooling_type",
                          "global_pooling",
                          "adaptive",
                          "padding_algorithm"},
                         {"Out"});
}

KernelSignature Pool3dGradOpArgumentMapping(
    const ArgumentMappingContext& ctx UNUSED) {
  return KernelSignature("pool3d_grad",
                         {"X", "Out", "Out@GRAD"},
                         {"ksize",
                          "strides",
                          "paddings",
                          "ceil_mode",
                          "exclusive",
                          "data_format",
                          "pooling_type",
                          "global_pooling",
                          "adaptive",
                          "padding_algorithm"},
                         {"X@GRAD"});
}

}  // namespace phi

PD_REGISTER_ARG_MAPPING_FN(pool2d, phi::Pool2dOpArgumentMapping);
PD_REGISTER_ARG_MAPPING_FN(pool2d_grad, phi::Pool2dGradOpArgumentMapping);
PD_REGISTER_ARG_MAPPING_FN(pool2d_double_grad,
                           phi::Pool2dDoubleGradOpArgumentMapping);

PD_REGISTER_ARG_MAPPING_FN(pool3d, phi::Pool3dOpArgumentMapping);
PD_REGISTER_ARG_MAPPING_FN(pool3d_grad, phi::Pool3dGradOpArgumentMapping);
