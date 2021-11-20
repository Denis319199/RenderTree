#ifndef MATRIX_MATH_HPP
#define MATRIX_MATH_HPP

#include <cmath>

#include "Matrix.hpp"

namespace math {

constexpr float kPi{3.141592f};

Matrix<float> GetProjectionMatrix(float near, float far, float fov,
                                  float aspect) {
  Matrix<float> projection_m(4, 4);

  float t = near * std::tanf(fov / 2.f);

  projection_m[0][0] = near / (t * aspect);
  projection_m[1][1] = near / t;
  projection_m[2][2] = (far + near) / (near - far);
  projection_m[2][3] = 2 * near * far / (near - far);
  projection_m[3][2] = -1;

  return projection_m;
}

Matrix<float> GetIdentityMatrix() {
  return Matrix<float>{{1.f, 0.f, 0.f, 0.f},
                       {0.f, 1.f, 0.f, 0.f},
                       {0.f, 0.f, 1.f, 0.f},
                       {0.f, 0.f, 0.f, 1.f}};
}

Matrix<float> Translate(const Matrix<float> &mat, float x, float y, float z) {
  Matrix<float> result{{1.f, 0.f, 0.f, x},
                       {0.f, 1.f, 0.f, y},
                       {0.f, 0.f, 1.f, z},
                       {0.f, 0.f, 0.f, 1.f}};

  return result *= mat;
}

Matrix<float> Scale(const Matrix<float> &mat, float factor) {
  Matrix<float> result{{factor, 0.f, 0.f, 0.f},
                       {0.f, factor, 0.f, 0.f},
                       {0.f, 0.f, factor, 0.f},
                       {0.f, 0.f, 0.f, 1.f}};

  return result *= mat;
}

Matrix<float> RotateMatrix(float x, float y, float z, float angle) {
  auto vec_length{std::sqrt(x * x + y * y + z * z)};

  auto norm_x{x / vec_length};
  auto norm_y{y / vec_length};
  auto norm_z{z / vec_length};

  auto cos_res{std::cosf(angle)};
  auto one_minus_cos_res{1 - cos_res};
  auto sin_res{std::sinf(angle)};

  Matrix<float> result(4, 4);
  result[0][0] = cos_res + one_minus_cos_res * norm_x * norm_x;
  result[0][1] = one_minus_cos_res * norm_x * norm_y - sin_res * norm_z;
  result[0][2] = one_minus_cos_res * norm_x * norm_z + sin_res * norm_y;
  result[1][0] = one_minus_cos_res * norm_y * norm_x + sin_res * norm_z;
  result[1][1] = cos_res + one_minus_cos_res * norm_y * norm_y;
  result[1][2] = one_minus_cos_res * norm_y * norm_z - sin_res * norm_x;
  result[2][0] = one_minus_cos_res * norm_z * norm_x - sin_res * norm_y;
  result[2][1] = one_minus_cos_res * norm_z * norm_y + sin_res * norm_x;
  result[2][2] = cos_res + one_minus_cos_res * norm_z * norm_z;

  return result;
}

}  // namespace math

#endif  // MATRIX_MATH_HPP
