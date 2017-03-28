#include <algorithm>

#include "catch.hpp"

#include <enum_containers/enum_vector.h>

namespace {
enum class AnimalType;
struct A;
}

struct C {
  ebc::enum_vector<AnimalType, A>
      ea; // checking instaniation without full definition
};

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

struct A {};
}

TEST_CASE("enum_vector") {
  C x;
  ebc::enum_vector<AnimalType, int> ea;
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
  ebc::enum_vector<empty_enum, double> e;
  REQUIRE(e.empty());
  REQUIRE(3 == ea.size());
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
  ea.at(AnimalType::crow) = 23;
  REQUIRE(ea[AnimalType::crow] == 23);
}
