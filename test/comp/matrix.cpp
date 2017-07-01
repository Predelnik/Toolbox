#include "catch.hpp"
#include "comp/matrix.h"
#include <comp/modulo.h>

TEST_CASE("matrix") {
  {
    matrix<int> m1 = {{2, 0, 0}, {0, 2, 0}, {0, 0, 2}};
    matrix<int> m2 = {{0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
    matrix<int> m3 = {{2, 1, 1}, {1, 2, 1}, {1, 1, 2}};
    REQUIRE(m1 + m2 == m3);
    REQUIRE(m3 - m1 == m2);
    REQUIRE(m3 - m2 == m1);
  }
  {
    matrix<int> m2 = {{0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
    auto id = basic::identity<matrix<int>>::value (m2);
    REQUIRE (m2 * id == m2);
  }
  {
    matrix<int> m1 = {{1, 2, 3}, {4, 5, 6}};
    matrix<int> m2 = {{7, 8}, {9, 10}, {11, 12}};
    matrix<int> m3 = {{58, 64}, {139, 154}};
    REQUIRE (m1 * m2 == m3);
  }
  {
    auto fib = [](int i)
    {
      matrix<long long> f_m = {{0, 1}, {1, 1}};
      return (std::vector<long long> {1, 1} * basic::pow (f_m, i - 1))[0];
    };
    REQUIRE (fib (1) == 1);
    REQUIRE (fib (2) == 1);
    REQUIRE (fib (3) == 2);
    REQUIRE (fib (50) == 12586269025ll);
    REQUIRE (fib (62) == 4052739537881ll);
  }
  {
    auto fib = [](int i)
    {
      matrix<modulo_int> f_m = {{0, 1}, {1, 1}};
      return (std::vector<modulo_int> {1, 1} * basic::pow (f_m, i - 1))[0];
    };
    REQUIRE (fib (100) == 687995182ll); // high order Fibonacci numbers mod 1e9+7
    REQUIRE (fib (200) == 349361645ll);
  }

  // TODO: write multiplication tests for vectors
}