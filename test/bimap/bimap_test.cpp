#include "catch.hpp"
#include "bimap/bimap.h"

#include <string>
#include <unordered_map>

TEST_CASE("bimap") {
  {
    bimap<int, std::string> m{{3, "abc"}, {7, "dbe"}};
    REQUIRE(m.at("abc") == 3);
    REQUIRE_THROWS_AS(m.at("asdasd"), std::out_of_range);
    REQUIRE(m.at(7) == "dbe");
  }
  {
    bimap<std::string, std::optional<int>, std::unordered_map,
          std::unordered_map> m;
    m.insert ({std::nullopt, "936"});
    m.insert ({"addd", 123});
    REQUIRE(m.size () == 2);
    REQUIRE (*m.value(std::nullopt) == "936");
    REQUIRE (*m.toBvalue("addd") == 123);
    REQUIRE_FALSE (m.toBvalue("asdsadsa"));
    REQUIRE_FALSE (m.toAvalue(12233));
  }
  {
    bimap<int, int> m {{8, 88}, {117, 48}};
    m.insert ({7, 777});
    REQUIRE (m.toAvalue(88) == 8);
    REQUIRE (m.toBvalue(117) == 48);
    REQUIRE_FALSE (m.toBvalue(1327));
  }
}
