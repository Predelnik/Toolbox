#pragma once
#include <map>
#include <optional>

namespace bimap_details
{
	template <typename TypeA, typename TypeB>
	std::pair<TypeB, TypeA> inverse_pair (const std::pair<TypeA, TypeB> &p) { return {p.second, p.first}; }
}

template <typename TypeA, typename TypeB, template <typename...> class MapA = std::map, template<typename...> class MapB = std::map>
class bimap
{
public:
	bimap () = default;
	bimap (std::initializer_list<std::pair<TypeA, TypeB>> il)
	{
		for (auto &&p : il)
		{
			a_to_b.insert (p);
			b_to_a.insert (bimap_details::inverse_pair (p));
		}
	}

	template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, int> = 0>
	bimap (std::initializer_list<std::pair<TypeB, TypeA>> il)
	{
		for (auto &&p : il)
		{
			b_to_a.insert (p);
			a_to_b.insert (bimap_details::inverse_pair (p));
		}
	}

	void insert (const std::pair<TypeA, TypeB> &p)
	{
		a_to_b.insert (p);
		b_to_a.insert (bimap_details::inverse_pair (p));
	}

  template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, int> = 0>
	void insert (const std::pair<TypeB, TypeA> &p)
	{
		b_to_a.insert (p);
		a_to_b.insert (bimap_details::inverse_pair (p));
	}

	template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, int> = 0>
	TypeB at (const TypeA &key) { return a_to_b.at (key); }

	template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, unsigned int> = 0> // unsigned int is hack for MSVC
	TypeA at (const TypeB &key) { return b_to_a.at (key); }

	template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, int> = 0>
	std::optional<TypeB> value (const TypeA &key) { auto it = a_to_b.find (key); if (it != a_to_b.end ()) return it->second; return std::nullopt; }

	template <typename AreSame = typename std::is_same<TypeA, TypeB>::type, std::enable_if_t<!AreSame::value, unsigned int> = 0>
	std::optional<TypeA> value (const TypeB &key) { auto it = b_to_a.find (key); if (it != b_to_a.end ()) return it->second; return std::nullopt; }

	std::optional<TypeA> toAvalue (const TypeB &key) { auto it = b_to_a.find (key); if (it != b_to_a.end ()) return it->second; return std::nullopt; }
	std::optional<TypeB> toBvalue (const TypeA &key) { auto it = a_to_b.find (key); if (it != a_to_b.end ()) return it->second; return std::nullopt; }

  std::size_t size () { return a_to_b.size (); }

private:
	MapA<TypeA, TypeB> a_to_b;
	MapB<TypeB, TypeA> b_to_a;
};
