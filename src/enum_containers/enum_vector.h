#include <vector>

// essenitaly the same as enum_array (not resizable like std:vector) but created on heap and allocated in the constructor (ValueT, EnumT could be forward declared)

namespace ebc
{
	template <typename EnumT, typename ValueT>
	// COUNT could be changed to something more suitable
	class enum_vector : std::vector<ValueT> {
	public:
		using self = enum_vector;
		using parent_t = std::vector<ValueT>;
		using parent_t::value_type;
		using parent_t::size_type;
		using parent_t::difference_type;
		using parent_t::reference;
		using parent_t::const_reference;
		using parent_t::pointer;
		using parent_t::const_pointer;
		using parent_t::iterator;
		using parent_t::const_iterator;
		using parent_t::const_reverse_iterator;

		enum_vector() : parent_t(static_cast<size_t> (EnumT::COUNT)) {
		}

		reference at(EnumT index) {
			return parent_t::at(static_cast<size_t>(index));
		}

		const_reference at(EnumT index) const {
			return parent_t::at(static_cast<size_t>(index));
		}

		reference operator[](EnumT index) {
			return parent_t::operator[](static_cast<size_t>(index));
		}

		const_reference operator[](EnumT index) const {
			return parent_t::operator[](static_cast<size_t>(index));
		}

		using parent_t::front;
		using parent_t::back;
		using parent_t::data;

		using parent_t::begin;
		using parent_t::end;
		using parent_t::cbegin;
		using parent_t::cend;
		using parent_t::rbegin;
		using parent_t::rend;
		using parent_t::crbegin;
		using parent_t::crend;

		using parent_t::empty;
		using parent_t::size;
		using parent_t::max_size;

		void fill(const ValueT& value) {
			for (auto& dst : *this)
				dst = value;
		}

		void swap(self& other) {
			parent_t::swap(static_cast<parent_t &>(other));
		}

		friend bool operator==(const self& lhs, const self& rhs) {
			return static_cast<const parent_t &>(lhs) ==
				static_cast<const parent_t &>(rhs);
		}

		friend bool operator<(const self& lhs, const self& rhs) {
			return static_cast<const parent_t &>(lhs) <
				static_cast<const parent_t &>(rhs);
		}

		friend bool operator>(const self& lhs, const self& rhs) {
			return static_cast<const parent_t &>(lhs) >
				static_cast<const parent_t &>(rhs);
		}

		friend bool operator<=(const self& lhs, const self& rhs) {
			return static_cast<const parent_t &>(lhs) <=
				static_cast<const parent_t &>(rhs);
		}

		friend bool operator>=(const self& lhs, const self& rhs) {
			return static_cast<const parent_t &>(lhs) >=
				static_cast<const parent_t &>(rhs);
		}

		friend void swap(self& lhs, self& rhs) {
			using std::swap;
			return swap(static_cast<parent_t &>(lhs), static_cast<parent_t &>(rhs));
		}
	};
}

