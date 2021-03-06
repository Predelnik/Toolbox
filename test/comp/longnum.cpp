#include "comp/longnum.h"
#include "catch.hpp"

TEST_CASE("longnum_addition")
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

TEST_CASE("longnum_multiplication")
{
  {
    longnum<int, 10> x = 13, y = 3;
    REQUIRE (x * y == 39);
  }
  {
    longnum<int, 10> x = 173, y = 324;
    REQUIRE (x * y == 56052);
  }
  {
    longnum<int, 10> x = -2, y = 100000;
    REQUIRE (x * y == -200000);
  }
}

TEST_CASE("longnum_division")
{
  {
    longnum<int, 10> x = 39, y = 3;
    REQUIRE (x / y == 13);
  }
  {
    longnum<int, 10> x = 309, y = 3;
    REQUIRE (x / y == 103);
  }
    {
    longnum<int, 10> x = 56052, y = 173;
    REQUIRE (x / y == 324);
  }
  {
    longnum<int, 10> x = 1667492481620ll, y = 4;
    REQUIRE (x / y == 416873120405);
  }
  {
    longnum<int, 10> x = 50, y = 1;
    REQUIRE (x / y == 50);
  }
}