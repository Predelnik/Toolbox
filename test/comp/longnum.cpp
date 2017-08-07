#include "comp/longnum.h"
#include "catch.hpp"

TEST_CASE("longnum")
{
  longnum<int, 10> x = 255;
  longnum<int, 10> y = 65;
  REQUIRE(x + y == 320);
  y = -12;
  REQUIRE(x + y == 243);
  REQUIRE(x - y == 267);
  REQUIRE(x - x == 0);
  REQUIRE(x + x == 510);
  REQUIRE(x + x + x + x == 1020);
}