#include "catch.hpp"

#include <enum_containers/enum_bitset.h>

enum class weather {
  sunny,
  rainy,
  overcast,

  COUNT,
};

TEST_CASE("enum_bitset") {
  ebc::enum_bitset<weather> bs;
  REQUIRE(bs.size() == 3);
  bs[weather::sunny] = true;
  bs[weather::overcast] = true;
  REQUIRE(bs.test(weather::sunny));
  REQUIRE_FALSE(bs.test(weather::rainy));
  const auto &cr = bs;
  REQUIRE(cr[weather::sunny]);
  REQUIRE(bs.test(weather::overcast));
  REQUIRE_FALSE(bs.all());
  REQUIRE_FALSE(bs.none());
  REQUIRE(bs.any());
  REQUIRE(bs.count() == 2u);
  bs.set(weather::rainy);
  REQUIRE(bs.all());
  REQUIRE_FALSE(bs.none());
  REQUIRE(bs.any());
  REQUIRE(3u == bs.count());
  bs.reset();
  REQUIRE_FALSE(bs.all());
  REQUIRE(bs.none());
  REQUIRE_FALSE(bs.any());
  REQUIRE(bs.count() == 0u);
  auto other = bs;
  other[weather::sunny] = true;
  REQUIRE(bs != other);
  bs = other;
  REQUIRE(bs == other);
  other.reset();
  bs &= other;
  REQUIRE(bs.none());
  other.set();
  bs |= other;
  REQUIRE(bs.all());
  bs.set(weather::overcast, false);
  bs.reset(weather::sunny);
  other.flip(weather::rainy);
  bs ^= other;
  REQUIRE(bs.all());
  bs = ~bs;
  REQUIRE(bs.none());
  bs.flip();
  REQUIRE(bs.all());
  bs = {weather::overcast, weather::sunny};
  other = {weather::sunny, weather::rainy};
  REQUIRE((bs & other).test(weather::sunny));
  REQUIRE((bs | other).all());
  REQUIRE_FALSE((bs ^ other).test(weather::sunny));
}
