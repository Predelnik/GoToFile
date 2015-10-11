#pragma once

#include "optional.h"

template <typename KeyType, typename ValueType>
ValueType value_or(const std::unordered_map<KeyType, ValueType>& m, const KeyType& key, const ValueType& defaultValue) {
	auto it = m.find(key);
	return it != m.end() ? *it : defaultValue;
}

template <typename ContainerType>
auto find_c (ContainerType &container, typename ContainerType::key_type key) -> optional<typename ContainerType::iterator> {
	auto it = container.find (key);
	if (it != container.end())
		return it;

	return nullopt;
}

class Uncopiable
{
	using self = Uncopiable;
protected:
	Uncopiable() {}
	~Uncopiable() {}
	Uncopiable(const self&) = delete;
	Uncopiable& operator=(const self&) = delete;
	Uncopiable(self&&) = default;
	Uncopiable& operator=(self&&) = default;
};

#define FIX_UNUSED(var) static_cast<void> (var)

