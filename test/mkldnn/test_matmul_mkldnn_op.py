# Copyright (c) 2020 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import unittest

import numpy as np
from eager_op_test import OpTest


class TestDnnlMatMulOp(OpTest):
    def generate_data(self):
        self.x = np.random.random((25, 2, 2)).astype("float32")
        self.y = np.random.random((25, 2, 2)).astype("float32")
        self.alpha = 1.0
        self.out = self.alpha * np.matmul(self.x, self.y)

    def set_attributes(self):
        self.alpha = self.alpha if hasattr(self, 'alpha') else 1.0
        self.attrs = {'alpha': self.alpha}

    def setUp(self):
        # Set max isa, otherwise fails on SKX and earlier
        os.environ["DNNL_MAX_CPU_ISA"] = "AVX"
        self.op_type = "matmul"
        self._cpu_only = True
        self.use_mkldnn = True
        self.generate_data()
        self.set_attributes()
        self.attrs['use_mkldnn'] = True

        self.inputs = {'X': self.x, 'Y': self.y}
        self.outputs = {'Out': self.out}

    def test_check_output(self):
        self.check_output()


class TestDnnlMatMulWithGradOp(TestDnnlMatMulOp):
    def test_check_grad(self):
        self.check_grad(['X', 'Y'], 'Out', max_relative_error=1e-2)


class TestDnnlMatMulOpMixedDims1(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((17, 2, 3)).astype("float32")
        self.y = np.random.random((3, 4)).astype("float32")
        self.out = np.matmul(self.x, self.y)


class TestDnnlMatMulOpMixedDimsYWiderTransposeY(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((8, 2, 3)).astype("float32")
        self.y = np.random.random((4, 3)).astype("float32")
        self.out = np.matmul(self.x, np.transpose(self.y))

    def set_attributes(self):
        self.attrs = {'transpose_Y': True}


class TestDnnlMatMulOpMixedDimsYWiderTransposeX(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((8, 3, 2)).astype("float32")
        self.y = np.random.random((3, 4)).astype("float32")
        self.out = np.matmul(np.transpose(self.x, (0, 2, 1)), self.y)

    def set_attributes(self):
        self.attrs = {'transpose_X': True}


class TestDnnlMatMulOpMixedDimsXWiderTransposeXY(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((8, 3, 2)).astype("float32")
        self.y = np.random.random((4, 3)).astype("float32")
        self.out = np.matmul(
            np.transpose(self.x, (0, 2, 1)), np.transpose(self.y)
        )

    def set_attributes(self):
        self.attrs = {'transpose_X': True, 'transpose_Y': True}


class TestDnnlMatMulOpMixedDimsYWiderTransposeXY(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((3, 2)).astype("float32")
        self.y = np.random.random((8, 4, 3)).astype("float32")
        self.out = np.matmul(
            np.transpose(self.x), np.transpose(self.y, (0, 2, 1))
        )

    def set_attributes(self):
        self.attrs = {'transpose_X': True, 'transpose_Y': True}


class TestDnnlMatMulOpMixedDimsXWiderTransposeX(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((5, 4)).astype("float32")
        self.y = np.random.random((8, 5, 4)).astype("float32")
        self.out = np.matmul(np.transpose(self.x), self.y)

    def set_attributes(self):
        self.attrs = {'transpose_X': True}


class TestDnnlMatMulOpVectorMultiply(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random(5).astype("float32")
        self.y = np.random.random(5).astype("float32")
        self.out = np.matmul(self.x, self.y)


class TestDnnlMatMulOpVectorMultiplyTranspose(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random(5).astype("float32")
        x_resized = np.copy(self.x)
        x_resized = np.expand_dims(x_resized, 1)
        self.y = np.random.random(6).astype("float32")
        y_resized = np.copy(self.y)
        y_resized = np.expand_dims(y_resized, 0)
        self.out = np.matmul(x_resized, y_resized)

    def set_attributes(self):
        self.attrs = {'transpose_Y': True, 'transpose_X': True}


class TestDnnlMatMulOpMixedDims2(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((2, 3)).astype("float32")
        self.y = np.random.random((17, 3, 4)).astype("float32")
        self.out = np.matmul(self.x, self.y)


class TestDnnlMatMulOpAlpha(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((17, 2, 3)).astype("float32")
        self.y = np.random.random((17, 3, 2)).astype("float32")
        self.alpha = 2.0
        self.out = self.alpha * np.matmul(self.x, self.y)


class TestDnnlMatMulOp2D(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((12, 9)).astype("float32")
        self.y = np.random.random((9, 12)).astype("float32")
        self.out = np.matmul(self.x, self.y)


class TestDnnlMatMulOpTransposeX(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((12, 9)).astype("float32")
        self.y = np.random.random((12, 9)).astype("float32")
        self.out = np.matmul(np.transpose(self.x), self.y)

    def set_attributes(self):
        self.attrs = {'transpose_X': True}


class TestDnnlMatMulOpTransposeY(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((12, 9)).astype("float32")
        self.y = np.random.random((12, 9)).astype("float32")
        self.out = np.matmul(self.x, np.transpose(self.y))

    def set_attributes(self):
        self.attrs = {'transpose_Y': True}


class TestDnnlMatMulOpTransposeY3D(TestDnnlMatMulWithGradOp):
    def generate_data(self):
        self.x = np.random.random((17, 3, 2)).astype("float32")
        self.y = np.random.random((17, 3, 2)).astype("float32")
        self.out = np.matmul(self.x, np.transpose(self.y, (0, 2, 1)))

    def set_attributes(self):
        self.attrs = {'transpose_Y': True}


class TestDnnlMatMulOpInt8NoScales(TestDnnlMatMulOp):
    def generate_data(self):
        self.x = np.random.random((12, 9)).astype("int8")
        self.y = np.random.random((9, 12)).astype("int8")
        self.out = np.matmul(self.x, self.y)


class TestDnnlMatMulOpInt8(TestDnnlMatMulOp):
    # Due to limitation in int8 matmul implementation
    # on older platforms (BDW, SKX) we needed to reduce
    # range from [-127, 127] to [-63, 63]
    def quantize(self, tensor):
        scale = 63.0 / np.abs(np.amax(tensor))
        quantized = np.round(scale * tensor).astype("int8")
        return scale, quantized

    def generate_data(self):
        x_float = np.random.random((12, 9)).astype("float32")
        self.x_scale, self.x = self.quantize(x_float)

        y_float = np.random.random((9, 12)).astype("float32")
        self.y_scale, self.y = self.quantize(y_float)

        out_float = np.matmul(x_float, y_float)
        self.out_scale, self.out = self.quantize(out_float)

    def set_attributes(self):
        self.attrs = {
            'Scale_x': self.x_scale,
            'Scale_y': self.y_scale,
            'Scale_out': self.out_scale,
        }

    def test_check_output(self):
        int_atol = 1
        self.check_output(atol=int_atol)


class TestDnnlMatMulOpInt8ForceFP32(TestDnnlMatMulOpInt8):
    def generate_data(self):
        x_float = np.random.random((12, 9)).astype("float32")
        self.x_scale, self.x = self.quantize(x_float)

        y_float = np.random.random((9, 12)).astype("float32")
        self.y_scale, self.y = self.quantize(y_float)

        out_float = np.matmul(x_float, y_float)
        self.out = out_float

    def set_attributes(self):
        self.attrs = {
            'Scale_x': self.x_scale,
            'Scale_y': self.y_scale,
            'force_fp32_output': True,
        }


class TestDnnlMatMulOpInt8ForceFP32BasicScales(TestDnnlMatMulOp):
    def generate_data(self):
        self.x = np.random.randint(0, 3, (12, 9)).astype("int8")
        self.y = np.random.randint(0, 3, (9, 12)).astype("int8")
        self.out = np.matmul(self.x, self.y).astype("float32")

    def set_attributes(self):
        self.attrs = {'force_fp32_output': True}


if __name__ == "__main__":
    from paddle import enable_static

    enable_static()
    unittest.main()
