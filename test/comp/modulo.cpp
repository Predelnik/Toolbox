#include "comp/modulo.h"
#include "catch.hpp"

TEST_CASE("modulo") {
  modulo_int_t<int, 13> x (6);
  x += 3;
  REQUIRE(x == 9);
  x += 4;
  REQUIRE(x == 0);
  x -= 5;
  REQUIRE(x == 8);
  REQUIRE((x - 15) == 6);

  REQUIRE(x * 3 == 11);
  x *= 2;
  REQUIRE(x == 3);

  REQUIRE (basic::pow (x, 3) == 1);
  REQUIRE (basic::pow (x, 6) == 1);

  REQUIRE (x / 3 == 1);
  REQUIRE (x / 4 == 4); // 16 / 4
}