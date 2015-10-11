#pragma once
#include "utils/bimap.h"
#include "utf8string.h"

class IniWorker {
public:
	enum class Action {
		save,
		load,
	};

	explicit IniWorker(const std::wstring& appName, const std::wstring& fileName, Action action);
	bool process(const std::wstring& name, std::wstring& value) const;
	bool process(const std::wstring& name, int& value) const;

	bool process(const std::wstring& name, utf8string& value) const {
		switch (m_action) {
		case Action::save:
			{
				auto str = value.to_wstring();
				process(name, str);
			}
			return true;
		case Action::load:
			{
				std::wstring str;
				if (process(name, str)) {
					value = {str};
					return true;
				}
				return false;
			}
		}
		return false;
	}

	bool process(const std::wstring& name, bool& value) const;

private:
	std::wstring m_appName;
	std::wstring m_fileName;
	Action m_action;
	unordered_bimap<bool, std::wstring> m_bool_bimap = {{false, L"false"},{true, L"true"}};
};

