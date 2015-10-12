#include "GotoFileManager.h"
#include "npp/EditorInterface.h"
#include "Settings.h"

#include <filesystem>

GotoFileManager::GotoFileManager(EditorInterface* m_iface, const Settings* settings) : m_iface{*m_iface}, m_settings{*settings} {
}

void GotoFileManager::TryOpenFile(const std::wstring& filename) {
	auto curPath = m_iface.ActiveFileDirectory();
	namespace fs = std::experimental::filesystem::v1;
	auto tryPath = [&](fs::path path) {
		if (path.empty())
			return false;
		std::error_code ec;

		if (fs::is_regular_file(path)) {
			path = fs::canonical(path, ec);
			if (ec)
				return false;

			if (m_settings.openLargeFilesInOtherProgram && fs::file_size(path) > m_settings.largeFileSizeLimit * 1024 * 1024) {
				return reinterpret_cast<int>(ShellExecute(m_iface.appHandle(), L"open", m_settings.customEditorPath.to_wstring().data(), path.c_str(), nullptr, SW_SHOW)) > 32;
			}

			auto initialFileName = m_iface.ActiveDocumentPath();
			if (m_iface.IsOpened(path))
				return true;

			if (!m_iface.OpenDocument(path))
				return false;

			m_iface.SwitchToFile(path);

			if (m_settings.openInOtherView)
				m_iface.MoveActiveDocumentToOtherView();

			m_iface.SwitchToFile(initialFileName);
			if (m_settings.switchToNewlyOpenedFiles)
				m_iface.SwitchToFile(path);

			return true;
		}

		return false;
	};

	// absolute path
	if (fs::path(filename).is_absolute() && tryPath(filename)) return;

	// relative path
	if (fs::path(filename).is_relative() && tryPath(fs::absolute(filename, curPath))) return;
}

std::wstring GotoFileManager::extract_filename(const std::vector<char>& buf, int index) {
	auto activeView = m_iface.ActiveView();
	switch (m_iface.GetEncoding(activeView)) {
	case EditorInterface::Codepage::ansi:
		{
			int start = index, end = index;
			auto isPosInStopChar = [&](int Index) {
				return m_settings.IsStopChar(buf[Index]);
			};
			int n = static_cast<int>(buf.size());
			if (start == n - 1)
				--start;
			while (!isPosInStopChar(start) && start > 0)
				--start;
			if (isPosInStopChar(start))
				++start;

			while (end < n && !isPosInStopChar(end))
				++end;

			if (start < end)
				return {std::wstring(buf.begin() + start, buf.begin() + end)};
		}
		break;
	case EditorInterface::Codepage::utf8:
		{
			auto u8str = utf8string::from_raw_utf8(buf.data());
			utf8string::iterator cur_it(u8str.base().begin() + index);
			auto start = cur_it, end = cur_it;
			auto isPosInStopChar = [&](auto it) {
				return m_settings.IsStopChar(it);
			};
			if (start == u8str.end())
				--start;
			while (!isPosInStopChar(start) && start != u8str.begin())
				--start;
			if (isPosInStopChar(start))
				++start;

			while (end != u8str.end() && !isPosInStopChar(end))
				++end;

			if (start < end)
				return utf8string(start, end).to_wstring();
		}
		break;
	case EditorInterface::Codepage::COUNT:
		assert(false); // wrong enum value
		break;
	}


	return {};
}


void GotoFileManager::GoToFile() {
	auto activeView = m_iface.ActiveView();
	auto buf = m_iface.SelectedText(activeView);
	if (!buf.empty()) // Because it includes terminating '\0'
	{
		switch (m_iface.GetEncoding(activeView)) {
		case EditorInterface::Codepage::ansi:
			{
				std::string str(buf.data());
				TryOpenFile({str.begin(), str.end()});
			}
			break;
		case EditorInterface::Codepage::utf8:
			{
				TryOpenFile(utf8string(std::string(buf.data())).to_wstring());
			}
			break;
		}

		return;
	}
	TryOpenFile(extract_filename(m_iface.GetCurrentLine(activeView), m_iface.GetCurrentPosInLine(activeView)));
}

