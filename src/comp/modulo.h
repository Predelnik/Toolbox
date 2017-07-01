#pragma once
#include "basic.h"

#include <ostream>

template <typename Type, Type Modulo>
struct modulo_int_t
{
  // int which performs all operation by modulo
  using self = modulo_int_t;

  modulo_int_t (Type arg) : x (arg) { x  = x % Modulo; if (x < 0) x += Modulo; }
  modulo_int_t () : x () {}

  self &operator+= (const self &other)
  {
    x += other.x;
    if (x >= Modulo)
      x -= Modulo;
    return *this;
  }
  friend self operator+ (const self &a, const self &b) { auto ret = a; ret += b; return ret; }

  self &operator-= (const self &other)
  {
    x -= other.x;
    if (x < 0)
      x += Modulo;
    return *this;
  }
  friend self operator- (const self &a, const self &b) { auto ret = a; ret -= b; return ret; }

  self &operator*= (const self &other)
  {
    x *= other.x;
    if (x >= Modulo)
      x %= Modulo;
    return *this;
  }
  friend self operator* (const self &a, const self &b) { auto ret = a; ret *= b; return ret; }

  self &operator/= (const self &other)
  {
    (*this) *= basic::pow (other, Modulo - 2);
    return *this;
  }
  friend self operator/ (const self &a, const self &b) { auto ret = a; ret /= b; return ret; }

  friend std::ostream& operator<<(std::ostream& os, const self& me)
  {
    os << me.x;
    return os;
  }

  friend std::istream& operator>>(std::istream& is, self& me)
  {
    is >> me.x;
    me.x %= Modulo;
    return is;
  }

  friend bool operator== (const self &a, const self &b) { return a.x == b.x; }
  friend bool operator!= (const self &a, const self &b) { return a.x != b.x; }

private:
  Type x;
};

namespace basic
{
  template <typename Type, Type Modulo>
  struct identity<modulo_int_t<Type, Modulo>> {
    static constexpr modulo_int_t<Type, Modulo> value (modulo_int_t<Type, Modulo>) { return 1; }
  };
}

using modulo_int = modulo_int_t<long long, 1000000007>;
