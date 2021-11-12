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
  projection_m[0][1] = 0.f;
  projection_m[0][2] = 0.f;
  projection_m[0][3] = 0.f;
  projection_m[1][0] = 0.f;
  projection_m[1][1] = near / t;
  projection_m[1][2] = 0.f;
  projection_m[1][3] = 0.f;
  projection_m[2][0] = 0.f;
  projection_m[2][1] = 0.f;
  projection_m[2][2] = (far + near) / (near - far);
  projection_m[2][3] = 2 * near * far / (near - far);
  projection_m[3][0] = 0.f;
  projection_m[3][1] = 0.f;
  projection_m[3][2] = -1;
  projection_m[3][3] = 0.f;

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

float Length(const Matrix<float> &mat) { 
  float x{mat[0][0]};
  float y{mat[1][0]};
  float z{mat[2][0]};
  return std::sqrt(x * x + y * y + z * z);
}

} // namespace math

#endif // MATRIX_MATH_HPP
