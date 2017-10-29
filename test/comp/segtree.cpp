#include "comp/segtree.h"
#include "catch.hpp"

struct add_op
{
    bool is_null () const { return val == 0; }
    int apply (int prev_range_value, size_t range_length) const
    {
      return prev_range_value + static_cast<int> (range_length) * val;   
    }
    friend add_op operator+ (const add_op &lhs, const add_op &rhs)
    {
        return {lhs.val + rhs.val};
    }
    int val;
};

TEST_CASE("segtree")
{
  std::vector<int> a = {1, 2, 3, 4, 5};
  using st = segment_tree<int, std::plus<>, add_op>;
  st t (std::move (a));
  REQUIRE (t.query(0, 2) == 6);
  REQUIRE (t.query(1, 4) == 14);
  t.update(0, 3, {3});
  REQUIRE (t.query(0, 1) == 9);
  REQUIRE (t.query (0, 4) == 27);
  t.update(0, 4, {-1});
  REQUIRE (t.query(3, 3) == 6);
  REQUIRE (t.query (0, 4) == 22);
  t.update(4, 4, {1000});
  REQUIRE (t.query (0, 4) == 1022);
}