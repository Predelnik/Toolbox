#include "macros/remap_rvalues_for.h"
#include "catch.hpp"

namespace {
class test_class
{
public:
  test_class &f (int) & { return *this; }
  REMAP_RVALUES_FOR(f);
};

test_class get_test_class () { return {}; }
} // namespace

TEST_CASE("remap_rvalues_for")
{
  test_class x;
  static_assert(std::is_same_v<decltype (x.f (23)), test_class&>);
  static_assert(std::is_same_v<decltype (get_test_class ().f(23)), test_class&&>);
}