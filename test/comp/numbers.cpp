#include "comp/numbers.h"
#include "catch.hpp"

TEST_CASE("gcd-lcm") {
  REQUIRE(3 == gcd (6, 9));
  REQUIRE(1 == gcd (25, 1));
  REQUIRE (42 == lcm (7, 6));
  REQUIRE (30 == lcm (15, 6));
}

TEST_CASE("prime-decomp") {
  {
    int x = 255;  
    auto p = prime_decomposition(x);
    REQUIRE (p.size () == 3);
    REQUIRE (p[0].prime == 3);
    REQUIRE (p[0].power == 1);
    REQUIRE (p[1].prime == 5);
    REQUIRE (p[1].power == 1);
    REQUIRE (p[2].prime == 17);
    REQUIRE (p[2].power == 1);
  }
  {
    int x = 32;  
    auto p = prime_decomposition(x);
    REQUIRE (p.size () == 1);
    REQUIRE (p[0].prime == 2);
    REQUIRE (p[0].power == 5);
  }
}