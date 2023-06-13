// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
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

#include "paddle/fluid/inference/ascendie/op_teller.h"

#include <bitset>

#include "paddle/fluid/framework/block_desc.h"
#include "paddle/fluid/framework/data_layout.h"
#include "paddle/fluid/framework/phi_utils.h"
#include "paddle/phi/core/compat/op_utils.h"
#include "paddle/phi/core/kernel_factory.h"

namespace paddle {
namespace framework {
class OpDesc;
}  // namespace framework
}  // namespace paddle

namespace paddle {
namespace inference {
namespace ascendie {

// Just tell by the op_types.
struct SimpleOpTypeSetTeller : public Teller {
  SimpleOpTypeSetTeller() {}

  bool operator()(const framework::OpDesc& desc,
                  bool use_no_calib_int8 = false,
                  bool with_dynamic_shape = false) override {
    const std::string op_type = desc.Type();
    /*
    eg:
    if(op_type == "XXX") {
        return true;
    }
    */
    return false;
  }

 private:
};

bool OpTeller::Tell(const framework::ir::Node* node,
                    bool use_no_calib_int8,
                    bool with_dynamic_shape) {
  const std::string op_type = node->Op()->Type();
  const framework::OpDesc desc = *node->Op();
  auto& default_teller = GetDefaultTeller();
  if ((*default_teller)(desc, use_no_calib_int8, with_dynamic_shape)) {
    return true;
  }
  return false;
}

OpTeller::OpTeller() {
  tellers_.emplace_back(new ascendie::SimpleOpTypeSetTeller);
}

}  // namespace ascendie
}  // namespace inference
}  // namespace paddle
