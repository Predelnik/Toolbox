#pragma once

#include <vector>

template <typename LhsType, typename RhsType>
auto gcd (LhsType lhs, RhsType rhs)
{
  while (lhs > 0 && rhs > 0)
    {
      if (lhs > rhs)
        lhs %= rhs;
      else
        rhs %= lhs;
    }
  return lhs + rhs;
}

template <typename LhsType, typename RhsType>
auto lcm (LhsType lhs, RhsType rhs)
{
  return lhs / gcd (lhs, rhs) * rhs;
}

template <typename IntType>
struct prime_decomposition_element
{
  IntType prime;
  std::size_t power;
};

template <typename IntType>
auto prime_decomposition (IntType number)
{
  std::vector<prime_decomposition_element<IntType>> ret;
  for (IntType i = 2; i * i <= number; ++i)
    {
      if (number % i == 0)
        {
          ret.push_back ({i, 0});
          do
          {
            ++ret.back ().power;
            number /= i;  
          }
          while (number % i == 0);
        }
    }
  if (number > 1)
    ret.push_back ({number, 1});
  return ret;
}


