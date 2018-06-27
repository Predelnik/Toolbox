#include "catch.hpp"
#include "misc/move_only.h"

TEST_CASE("move_only") {
  {
    move_only<bool> a = true, b = true;
    // fails to compile:
    // a = b;
    a = std::move(b);
    REQUIRE(b.get() == false);

    b = std::move(a);
    REQUIRE(a.get() == false);
  }

  {
    move_only<bool> x;
    REQUIRE_FALSE(x.get());
  }
}