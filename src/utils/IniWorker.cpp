#include "IniWorker.h"

IniWorker::IniWorker(const std::wstring& appName, const std::wstring& fileName, Action action) : m_appName{appName}, m_fileName{fileName}, m_action{action} {
}

static const int initialBufSize = 256;
static const int maxBufSize = 100 * 1024;

bool IniWorker::process(const std::wstring& name, std::wstring& value) const {
	switch (m_action) {
	case Action::save:
		return WritePrivateProfileString(m_appName.data(), name.data(), value.data(), m_fileName.data()) != FALSE;
	case Action::load:
		{
			int bufSize = initialBufSize;
			while (true) {
				std::vector<wchar_t> buf(bufSize);
				auto ret = GetPrivateProfileString(m_appName.data(), name.data(), value.data(), buf.data(), buf.size(), m_fileName.data());
				if (ret == 0)
					return false;

				if (static_cast<int>(ret) < bufSize - 1) {
					value = buf.data();
					return true;
				}
				if (bufSize > maxBufSize)
					return false;
				bufSize *= 2;
			}
		}
	}
	return false;
}

bool IniWorker::process(const std::wstring& name, int& value) const {
	switch (m_action) {
	case Action::save:
		{
			auto str = std::to_wstring(value);
			return process(name, str);
		}
	case Action::load:
		{
			std::wstring str;
			if (!process(name, str))
				return false;
			size_t idx;
			try {
				auto new_val = std::stoi(str, &idx, 10);
				value = new_val;
			}
			catch (...) {
				return false;
			}
			break;
		}
	}
	return false;
}

bool IniWorker::process(const std::wstring& name, bool& value) const {
	switch (m_action) {
	case Action::save:
		{
			std::wstring str(m_bool_bimap.from().at(value));
			return process(name, str);
		}
	case Action::load:
		{
			std::wstring str;
			bool ret = process(name, str);
			if (!ret)
				return false;
			auto it = m_bool_bimap.to().find(str);
			if (it != m_bool_bimap.to().end()) {
				value = it->second;
				return true;
			}
			return false;
		}
	}
	return false;
}

