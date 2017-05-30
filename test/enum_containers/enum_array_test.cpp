#include "catch.hpp"

#include "enum_containers/enum_array.h"

namespace {
enum class AnimalType {
  rabbit,
  fox,
  crow,

  COUNT,
};

enum class empty_enum {
  COUNT,
};
}

TEST_CASE("enum_array") {
  ebc::enum_array<AnimalType, int> ea;
  ea.fill(0);
  REQUIRE(std::all_of(ea.begin(), ea.end(),
                      [](const int &val) { return val == 0; }));
  ea[AnimalType::fox] = 4;
  ea[AnimalType::crow] = 666;
  const auto &cr = ea;
  REQUIRE(cr[AnimalType::fox] == 4);
  REQUIRE(cr.at(AnimalType::crow) == 666);
  REQUIRE(ea.front() == 0);
  REQUIRE(ea.back() == 666);
  REQUIRE(ea.data() == &cr[AnimalType::rabbit]);
  REQUIRE(*std::max_element(cr.cbegin(), cr.cend()) == 666);
  std::sort(ea.rbegin(), ea.rend());
  REQUIRE(ea[AnimalType::rabbit] == 666);
  REQUIRE(*std::min_element(cr.crbegin(), cr.crend()) == 0);
  REQUIRE_FALSE(cr.empty());
  ebc::enum_array<empty_enum, double> e;
  REQUIRE(e.empty());
  REQUIRE(ea.size() == 3);
  auto other = ea;
  other.fill(333);
  ea.swap(other);
  REQUIRE(std::all_of(ea.cbegin(), ea.cend(),
                      [](const int &val) { return val == 333; }));
  REQUIRE(other > ea);
  REQUIRE(ea < other);
  REQUIRE(other >= ea);
  REQUIRE(ea <= other);
  auto same = ea;
  REQUIRE(ea == same);
  REQUIRE(ea >= same);
  REQUIRE(ea <= same);
  swap(ea, other);
  REQUIRE(std::all_of(other.cbegin(), other.cend(),
                      [](const int &val) { return val == 333; }));
  {
    using std::get;
    REQUIRE(0 == get<AnimalType::crow>(ea));
    REQUIRE(666 == get<AnimalType::rabbit>(cr));
    REQUIRE(4 == get<AnimalType::fox>([&] { return ea; }()));
  }
  ea.at(AnimalType::crow) = 23;
  REQUIRE(23 == ea[AnimalType::crow]);
}
