#include "catch.hpp"
#include "comp/multi_vector.h"

TEST_CASE("multi_vector") {
  {
    multi_vector<int, 3> v (3, 5, 7);
    v[1][2][3] = 23;
    REQUIRE(v[1][2][3] == 23);
    std::fill (v.begin (), v.end (), 16);
    REQUIRE(v[2][4][6] == 16);
  }
  {
    multi_vector<int, 2> v = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int cnt = 0;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        REQUIRE (v[i][j] == ++cnt);
  }

  {
    multi_vector<int, 3> v = {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    int cnt = 0;
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j)
        for (int k = 0; k < 2; ++k)
          REQUIRE (v[i][j][k] == ++cnt);
  }
}