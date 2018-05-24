/*
 * Copyright (C) 2017-2018 Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef VISUALMESH_ENGINE_OPENCL_SCALARS_HPP
#define VISUALMESH_ENGINE_OPENCL_SCALARS_HPP

namespace visualmesh {
namespace engine {
  namespace opencl {

    std::string get_scalar_defines(float) {
      return "#define Scalar float\n"
             "#define Scalar2 float2\n"
             "#define Scalar3 float3\n"
             "#define Scalar4 float4\n"
             "#define Scalar8 float8\n"
             "#define Scalar16 float16\n";
    }

    std::string get_scalar_defines(double) {
      return "#define Scalar double\n"
             "#define Scalar2 double2\n"
             "#define Scalar3 double3\n"
             "#define Scalar4 double4\n"
             "#define Scalar8 double8\n"
             "#define Scalar16 double16\n";
    }

  }  // namespace opencl
}  // namespace engine
}  // namespace visualmesh

#endif  // VISUALMESH_ENGINE_OPENCL_SCALARS_HPP
