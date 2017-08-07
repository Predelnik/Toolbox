#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>

template <typename DigitType, DigitType DigitSize>
class longnum
{
  using self = longnum;
  enum class sign_t
  {
    plus,
    minus,
  };
  sign_t negate (sign_t sign)
  {
    switch (sign)
      {
      case sign_t::plus: return sign_t::minus;
      case sign_t::minus: return sign_t::plus;
      }
    return sign_t::plus;
  }

public:

  template <typename IntegralType>
  longnum (IntegralType number)
  {
    static_assert(std::is_integral<IntegralType>::value, "Please provide signed integral type");
    size_t cur_digit = 0;
    if (number < 0)
    {
      m_sign = sign_t::minus;
      number = -number;
    }
    while (number > 0)
    {
      assign_digit(cur_digit, number % DigitSize);
      number /= DigitSize;
      ++cur_digit;
    }
  }

  bool operator== (const self &other)
  {
    return (m_digits == other.m_digits);
  }

  self operator* (const self &other)
  {
    self ret = 0;

    for (size_t i = 0; i < this->digit_count(); ++i)
      {
        self summand = 0;
        size_t mem = 0;
        for (size_t j = 0; j < other.digit_count() + 1; ++j)
          {
            auto sum = digit (i) * other.digit (j) + mem;
            summand.assign_digit (i + j, sum % DigitSize);
            mem = sum / DigitSize;
          }
        summand.shrink_to_fit();
        ret += summand;
      }

    bool of_the_same_sign = (this->m_sign == other.m_sign);
    ret.m_sign = of_the_same_sign ? sign_t::plus : sign_t::minus;
    return ret;
  }

  self &operator*= (const self &other)
  {
    *this = *this * other;
    return *this;
  }

  self operator+ (const self &other)
  {
    auto total_count = std::max (digit_count(), other.digit_count());
    bool of_the_same_sign = (this->m_sign == other.m_sign);
    self ret = 0;
    DigitType mem = 0;
    ret.m_sign = this->m_sign;
    for (int i = 0; i < total_count + 1; ++i)
      {
        if (of_the_same_sign)
          {
            auto sum = mem + digit (i) + other.digit (i);
            ret.assign_digit(i, sum % DigitSize);
            mem = sum / DigitSize;
          }
        else
          {
            auto sum = mem + digit (i) - other.digit (i);
            if (sum < 0)
              {
                sum += DigitSize;
                mem = -1;
              }
            ret.assign_digit(i, sum);
          }
      }
    if (!of_the_same_sign && mem == -1)
      ret.m_sign = negate(ret.m_sign);
    ret.shrink_to_fit();
    return ret;
  }

  self &operator+= (const self &other)
  {
    *this = *this + other;
    return *this;
  }

  self operator- (self other)
  {
    other.m_sign = negate (other.m_sign);
    return *this + other;
  }

  self &operator-= (const self &other)
  {
    *this = *this - other;
    return *this;
  }

  DigitType digit (std::size_t i) const { return i < m_digits.size () ? m_digits[i] : 0;}
  void assign_digit (std::size_t i, DigitType value)
  {
    assert (value < DigitSize);
    if (m_digits.size () < i + 1)
      m_digits.resize (i + 1);
    m_digits[i] = value;
  }
  std::size_t digit_count () const { return m_digits.size (); }
  void shrink_to_fit ()
  {
    while (!m_digits.empty () && m_digits.back () == 0)
      m_digits.pop_back ();
  }

  sign_t sign () const { return m_sign; }

private:
  std::vector<DigitType> m_digits;
  sign_t m_sign = sign_t::plus;
};
