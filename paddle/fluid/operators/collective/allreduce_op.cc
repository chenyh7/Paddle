/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/fluid/operators/collective/allreduce_op.h"

#include <future>  // NOLINT
#include <ostream>

namespace paddle {
namespace operators {

class AllReduceDelOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

  void InferShape(framework::InferShapeContext* ctx) const override {}

 protected:
  phi::KernelKey GetExpectedKernelType(
      const framework::ExecutionContext& ctx) const override {
    return phi::KernelKey(OperatorWithKernel::IndicateVarDataType(ctx, "X"),
                          ctx.GetPlace());
  }
};

class AllReduceDelOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  void Make() {
    AddInput("X", "(Tensor), tensor to be allreduced.");
    AddOutput("Out", "(Tensor) the result of allreduced.");
    AddAttr<int>("reduce_type", "(int) determine the reduce type.")
        .SetDefault(0);
    AddAttr<bool>(
        "sync_mode",
        "(bool) whether to synchronize the CUDA stream after nccl call.")
        .SetDefault(false);
    AddComment(R"DOC(
***AllReduce Operator***

Call NCCL AllReduce internally. Note that this op must be used when one
thread is managing one GPU device.

For speed reasons, reduce_type should be an integer:

0: sum
1: prod
2: max
3: min

If input and output are the same variable, in-place allreduce will be used.
)DOC");
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;
namespace plat = paddle::platform;

REGISTER_OP_WITHOUT_GRADIENT(allreduce,
                             ops::AllReduceDelOp,
                             ops::AllReduceDelOpMaker);

PD_REGISTER_STRUCT_KERNEL(allreduce,
                          CPU,
                          ALL_LAYOUT,
                          ops::AllReduceOpKernel,
                          float,
                          double,
                          int,
                          int64_t,
                          plat::float16) {}
