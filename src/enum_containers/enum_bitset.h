#pragma once
#include <bitset>

// enum indexed std::bitset

namespace ebc
{
	template <typename EnumT, size_t Count = static_cast<size_t>(EnumT::COUNT)>
	// COUNT could be changed to something more suitable
	class enum_bitset : std::bitset<Count> {
	public:
		using self = enum_bitset;
		using parent_t = std::bitset<Count>;
		using parent_t::reference;

		enum_bitset() {
		}

		enum_bitset(std::initializer_list<EnumT> list) {
			for (auto& elem : list)
				set(elem);
		}

		enum_bitset(const enum_bitset&) = default;
		self& operator=(const enum_bitset&) = default;
		enum_bitset (enum_bitset &&) = default;
		self &operator= (enum_bitset &&) = default;
		~enum_bitset() = default;

		auto operator[](EnumT index) {
			return parent_t::operator[](static_cast<size_t>(index));
		}

		bool operator[](EnumT index) const {
			return parent_t::operator[](static_cast<size_t>(index));
		}

		bool test(EnumT index) const {
			return parent_t::test(static_cast<size_t>(index));
		}

		using parent_t::all;
		using parent_t::any;
		using parent_t::none;
		using parent_t::count;
		using parent_t::size;

		bool operator==(const self& other) const {
			return parent_t::operator==(static_cast<const parent_t &>(other));
		}

		bool operator!=(const self& other) const {
			return parent_t::operator!=(static_cast<const parent_t &>(other));
		}

		self operator~() const {
			self ret = *this;
			ret.flip();
			return ret;
		}

		self& operator&=(const self& other) {
			parent_t::operator&=(static_cast<const parent_t &>(other));
			return *this;
		}

		self& operator|=(const self& other) {
			parent_t::operator|=(static_cast<const parent_t &>(other));
			return *this;
		}

		self& operator^=(const self& other) {
			parent_t::operator^=(static_cast<const parent_t &>(other));
			return *this;
		}

		// bit-shifting is disallowed

		self& set(EnumT index, bool value = true) {
			parent_t::set(static_cast<size_t>(index), value);
			return *this;
		}

		self& set() {
			parent_t::set();
			return *this;
		}

		self& reset(EnumT index) {
			parent_t::reset(static_cast<size_t>(index));
			return *this;
		}

		self& reset() {
			parent_t::reset();
			return *this;
		}

		self& flip(EnumT index) {
			parent_t::flip(static_cast<size_t>(index));
			return *this;
		}

		self& flip() {
			parent_t::flip();
			return *this;
		}

		friend self operator&(const self& lhs, const self& rhs) {
			self ret = lhs;
			ret &= rhs;
			return ret;
		}

		friend self operator|(const self& lhs, const self& rhs) {
			self ret = lhs;
			ret |= rhs;
			return ret;
		}

		friend self operator^(const self& lhs, const self& rhs) {
			self ret = lhs;
			ret ^= rhs;
			return ret;
		}
	};
}

