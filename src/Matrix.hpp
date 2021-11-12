#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <utility>

namespace math {

template <class T> class Matrix {
private:
  using ValueType = T;
  using SizeType = std::size_t;

  class MatrixRow {
  public:
    MatrixRow(T *row_data) : row_data_{row_data} {}

    T &operator[](SizeType index) noexcept { 
      return *(row_data_ + index); 
    }

    const T &operator[](SizeType index) const noexcept {
      return *(row_data_ + index);
    }

  private:
    T *row_data_;
  };

public:
  Matrix() : data_{}, n_row_{}, n_col_{} {}

  Matrix(SizeType n_col, SizeType n_row, T val = T())
      : data_{}, n_row_{n_row}, n_col_{n_col} {
    auto matrix_size{n_col_ * n_row_};
    if (matrix_size != 0) {
      data_ = static_cast<T *>(::operator new(sizeof(T) * matrix_size));

      for (SizeType i{}; i < matrix_size; ++i) {
        ::new (data_ + i) T(val);
      }
    }  
  }

  Matrix(std::initializer_list<std::initializer_list<T>> init)
      : data_{}, n_row_{init.size()}, n_col_{init.begin()->size()} {
    auto matrix_size{n_col_ * n_row_};
    if (matrix_size != 0) {
      data_ = static_cast<T *>(::operator new(sizeof(T) * matrix_size));

      SizeType i{};
      for (auto row_vals : init) {
        assert(row_vals.size() == n_col_ && "Rows have different length");

        for (const auto &val : row_vals) {
          ::new (data_ + i) T(val);
          ++i;
        }
      }
    }   
  }

  Matrix(const Matrix &other)
      : data_{}, n_row_{other.n_row_}, n_col_{other.n_col_} {
    auto matrix_size{n_row_ * n_col_};
    if (matrix_size != 0) {
      data_ = static_cast<T *>(::operator new(sizeof(T) * matrix_size));

      for (SizeType i{}; i < matrix_size; ++i) {
        ::new (data_ + i) T(*(other.data_ + i));
      }
    } 
  }

  Matrix(Matrix &&other) noexcept
      : data_{other.data_}, n_row_{other.n_row_}, n_col_{other.n_col_} {
    other.data_ = nullptr;
    other.n_row_ = 0;
    other.n_col_ = 0;
  }

  Matrix &operator=(const Matrix &other) {
    auto other_matrix_size{other.n_row_ * other.n_col_};

    if (other_matrix_size != 0) {
      if (n_row_ * n_col_ == other_matrix_size) {
        auto matrix_size{n_col_ * n_row_};
        for (SizeType i{}; i < matrix_size; ++i) {
          (data_ + i)->~T();
        }
      } else {
        DeleteData();
        data_ = static_cast<T *>(::operator new(sizeof(T) * other_matrix_size));
      }

      for (SizeType i{}; i < other_matrix_size; ++i) {
        ::new (data_ + i) T((other.data_ + i));
      }

      n_row_ = other.n_row_;
      n_col_ = other.n_col_;
    } else {
      Clear();
    }

    
    return *this;
  }

  Matrix &operator=(Matrix &&other) noexcept {
    Clear();

    std::swap(data_, other.data_);
    std::swap(n_row_, other.n_row_);
    std::swap(n_col_, other.n_col_);

    return *this;
  }

  Matrix operator*(const Matrix &other) const {
    Matrix result{*this};
    return result *= other;
  }

  Matrix &operator*=(const Matrix &other) {
    assert(n_col_ * n_row_ != 0 && "Empty matrix");
    assert(n_col_ == other.n_row_ &&
           "The number of columns of the first matrix is not equal to the "
           "number of rows of the second");

    auto new_matrix =
        static_cast<T *>(::operator new(sizeof(T) * n_row_ * other.n_col_));

    for (SizeType i{}; i < n_row_; ++i) {
      for (SizeType j{}; j < other.n_col_; ++j) {
        T sum{};

        for (SizeType k{}; k < n_col_; ++k) {
          sum += (*(data_ + i * n_col_ + k)) *
                 (*(other.data_ + k * other.n_col_ + j));
        }

        ::new (new_matrix + i * other.n_col_ + j) T(std::move(sum));
      }
    }

    DeleteData();

    data_ = new_matrix;
    n_col_ = other.n_col_;
    return *this;
  }

  Matrix &operator+=(const Matrix &rhs) {
    assert(n_col_ * n_row_ != 0 && "Empty matrix");
    assert(n_row_ == rhs.n_row_ && n_col_ == rhs.n_col_ &&
           "Matrices have different sizes");

    auto matrix_size{n_row_ * n_col_};
    for (SizeType i{}; i < matrix_size; ++i) {
      *(data_ + i) += *(rhs.data_ + i);
    }

    return *this;
  }

  Matrix operator+(const Matrix &rhs) const {
    auto tmp{*this};
    return tmp += rhs;
  }

  Matrix &operator-=(const Matrix &rhs) {
    assert(n_col_ * n_row_ != 0 && "Empty matrix");
    assert(n_row_ == rhs.n_row_ && n_col_ == rhs.n_col_ &&
           "Matrices have different sizes");

    auto matrix_size{n_row_ * n_col_};
    for (SizeType i{}; i < matrix_size; ++i) {
      *(data_ + i) -= *(rhs.data_ + i);
    }

    return *this;
  }

  Matrix operator-(const Matrix &rhs) const {
    auto tmp{*this};
    return tmp -= rhs;
  }

  MatrixRow operator[](SizeType index) { 
    assert(n_row_ > index && "Invalid index");
    return data_ + index * n_col_; 
  }

  const MatrixRow operator[](SizeType index) const {
    return data_ + index * n_col_;
  }

  ~Matrix() { Clear(); }

  void Clear() {
    if (data_) {
      DeleteData();
      n_row_ = 0;
      n_col_ = 0;
    }
  }

  const T *GetPtr() const noexcept { return data_; }

  Matrix Transpose() const {
    Matrix result{};
    result.n_row_ = n_col_;
    result.n_col_ = n_row_;

    result.data_ =
        static_cast<T *>(::operator new(sizeof(T) * n_row_ * n_col_));

    for (SizeType i{}; i < n_row_; ++i) {
      for (SizeType j{}; j < n_col_; ++j) {
        new (result.data_ + j * n_row_ + i) T(*(data_ + i * n_row_ + j));
      }
    }

    return result;
  }

private:
  void DeleteData() {
    auto matrix_size{n_col_ * n_row_};
    for (SizeType i{}; i < matrix_size; ++i) {
      (data_ + i)->~T();
    }

    ::operator delete(data_);
    data_ = nullptr;
  }

  T *data_;
  SizeType n_row_;
  SizeType n_col_;
};

using Mat = Matrix<float>;

} // namespace math

#endif // MATRIX_HPP
