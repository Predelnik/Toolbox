#pragma once

#include "multi_vector.h"
#include "basic.h"

template <typename T>
struct matrix {
  // simple class representing n x m matrix
private:
  using self = matrix;

public:
  matrix (std::size_t n, std::size_t m) : data (n, m) {}
  matrix (std::initializer_list<std::initializer_list<T>> list) : data (list) {}
  std::size_t n () const { return data.size (0); }
  std::size_t m () const { return data.size (1); }
  self &operator+= (const self &other)
  {
    assert (n () == other.n () && m () == other.m ());
    for (int i = 0; i < n (); ++i)
      for (int j = 0; j < m (); ++j)
        data[i][j] += other.data[i][j];
    return *this;
  }
  friend self operator+ (const self &lhs, const self &rhs) { auto ret = lhs; ret += rhs; return ret; }

  self &operator-= (const self &other)
  {
    assert (n () == other.n () && m () == other.m ());
    for (int i = 0; i < n (); ++i)
      for (int j = 0; j < m (); ++j)
        data[i][j] -= other.data[i][j];
    return *this;
  }
  friend self operator- (const self &lhs, const self &rhs) { auto ret = lhs; ret -= rhs; return ret; }

  self &operator*= (const self &other)
  {
    assert (m () == n () && other.m () == other.n () && other.n () == n ());
    (*this) = (*this) * other;
    return *this;
  }

  friend self operator* (const self &lhs, const self &rhs)
  {
    assert (lhs.m () == rhs.n ());
    matrix res (lhs.n (), rhs.m ());
    for (int i = 0; i < lhs.n (); ++i)
      for (int j = 0; j < rhs.m (); ++j)
        for (int k = 0; k < lhs.m (); ++k)
          res.data[i][j] += lhs.data[i][k] * rhs.data[k][j];
    return res;
  }

  friend std::vector<T> operator* (const std::vector<T> &lhs, const self &rhs)
  {
    assert (lhs.size () == rhs.n ());
    std::vector<T> res (rhs.m ());
    for (int i = 0; i < rhs.m (); ++i)
      for (int j = 0; j < rhs.n (); ++j)
        res[i] += lhs[j] * rhs.data[j][i];
    return res;
  }

  friend std::vector<T> operator* (const self &lhs, const std::vector<T> &rhs)
  {
    assert (rhs.size () == lhs.m ());
    std::vector<T> res (rhs.n ());
    for (int i = 0; i < lhs.n (); ++i)
      for (int j = 0; j < lhs.m (); ++j)
        res[i] += lhs.data[i][j] * rhs[j];
    return res;
  }

  friend bool operator== (const self &lhs, const self &rhs) { return lhs.data == rhs.data; }
  friend bool operator!= (const self &lhs, const self &rhs) { return lhs.data != rhs.data; }
  friend bool operator> (const self &lhs, const self &rhs) { return lhs.data > rhs.data; }
  friend bool operator>= (const self &lhs, const self &rhs) { return lhs.data >= rhs.data; }
  friend bool operator< (const self &lhs, const self &rhs) { return lhs.data < rhs.data; }
  friend bool operator<= (const self &lhs, const self &rhs) { return lhs.data <= rhs.data; }

public:
  multi_vector<T, 2> data;
};

namespace basic
{
template <typename T>
struct identity<matrix<T>> {
  static constexpr matrix<T> value (const matrix<T> &mat) {
    assert (mat.m () == mat.n () && mat.n () > 0);
    matrix<T> res (mat.n (), mat.m ());
    for (int i = 0; i < mat.n (); ++i) {
      res.data[i][i] = identity<T>::value (mat.data[0][0]);
    }
    return res;
  }
};
}