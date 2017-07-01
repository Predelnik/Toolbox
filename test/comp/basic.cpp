#include "catch.hpp"
#include "comp/basic.h"

TEST_CASE("pow") {
  REQUIRE (basic::pow (3, 3) == 27);
  REQUIRE (basic::pow (5, 0) == 1);
  REQUIRE (basic::pow (25, 1) == 25);
}