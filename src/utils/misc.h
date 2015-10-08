template <typename KeyType, typename ValueType>
ValueType value_or(const std::unordered_map<KeyType, ValueType>& m, const KeyType& key, const ValueType& defaultValue) {
	auto it = m.find(key);
	return it != m.end() ? *it : defaultValue;
}

#define FIX_UNUSED(var) static_cast<void> (var)

