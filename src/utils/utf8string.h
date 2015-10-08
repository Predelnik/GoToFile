// class for represinting string in UTF-8 encoding made in terms of simple std::string
#pragma once

class utf8string {
	using self = utf8string;
	using base_t = std::string;
	using size_type = base_t::size_type;

	std::string m_str;

public:
	utf8string() : m_str() {
	}

	utf8string(const char* raw_utf8); // constructor if we know that data is already in UTF-8
	template <class CHAR>
	utf8string(const std::basic_string<CHAR>& strArg); // explicit convert
	bool operator ==(const utf8string& other) const {
		return m_str == other.m_str;
	}

	friend self operator+(const self& lhs, const self& rhs) {
		utf8string s;
		s.m_str = lhs.m_str + rhs.m_str;
		return s;
	}

	const char* c_str() const {
		return m_str.c_str();
	}

	std::wstring to_wstring() const;

	std::string& base() {
		return m_str;
	};

	struct iterator : std::iterator<std::bidirectional_iterator_tag, char> {
	private:
		using self = iterator;
		using base_t = base_t::iterator;

	public:
		base_t base_it;
		self& operator++();
		self& operator--();

		iterator(base_t base_it_arg);

		bool operator==(const iterator& other) {
			return base_it == other.base_it;
		}

		bool operator!=(const iterator& other) {
			return base_it != other.base_it;
		}

		const char& operator *() const {
			return *base_it;
		}

		friend bool operator <(const self& lhs, const self& rhs) {
			return lhs.base_it < rhs.base_it;
		}

	private:
		friend class utf8string;
	};

	utf8string(iterator start, iterator finish);
	iterator find(self::iterator char_ref);

	iterator begin() {
		return {m_str.begin()};
	}

	iterator end() {
		return {m_str.end()};
	}
};

