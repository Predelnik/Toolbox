#pragma once
#include <type_traits>

namespace basic
{
template <typename T, typename = void>
struct identity;

template <typename T>
struct identity<T, std::enable_if_t<std::is_integral<T>::value>> {
  static constexpr T value (T) { return 1; }
};

template <typename T, typename PowType>
T pow (T val, PowType pow)
// taking into nth power with log (pow) complexity
{
  auto mult = val;
  auto ret = identity<T>::value (val);
  while (pow > 0)
  {
    if (pow % 2 == 1) {
        --pow;
        ret *= mult;
      }
    else {
      mult *= mult;
      pow /= 2;
    }
  }
  return ret;
}
}
