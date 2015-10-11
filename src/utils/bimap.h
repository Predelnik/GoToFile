#pragma once

namespace detail
{
	template <typename TypeA, typename TypeB>
	auto reversed(const std::pair<TypeA, TypeB>& p) {
		return std::make_pair(p.second, p.first);
	}
}

template <typename TypeA, typename TypeB>
class unordered_bimap {
public:
	unordered_bimap(std::initializer_list<std::pair<TypeA, TypeB>> list) {
		for (auto& p : list) {
			m_from.insert(p);
			m_to.insert(detail::reversed(p));
		}
	}

	unordered_bimap(std::initializer_list<std::pair<TypeB, TypeA>> list) {
		for (auto& p : list) {
			m_to.insert(p);
			m_from.insert(detail::reversed(p));

		}
	}

	bool insert(const std::pair<TypeA, TypeB>& p) {
		if (m_from.count(p.first) > 0 || m_to.count(p.second) > 0)
			return false;

		bool ret = m_from.insert(p).second;
		ret = ret && m_to.insert(detail::reversed(p));
		return ret;
	}

	bool insert(const std::pair<TypeB, TypeA>& p) {
		if (m_to.count(p.first) > 0 || m_from.count(p.second) > 0)
			return false;

		bool ret = m_to.insert(p);
		ret = ret && m_from.insert(detail::reversed(p));
		return ret;
	}

	const auto& from() const {
		return m_from;
	}

	const auto& to() const {
		return m_to;
	}

private:
	std::unordered_map<TypeA, TypeB> m_from;
	std::unordered_map<TypeB, TypeA> m_to;
};

