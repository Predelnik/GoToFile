#pragma once

class EditorInterface;
struct Settings;

class GotoFileManager {
public:
	explicit GotoFileManager(EditorInterface *iface, const Settings *settings);
	void GoToFile();

private:
	void TryOpenFile(const std::wstring& filename);
	std::wstring extract_filename(const std::vector<char>& buf, int index);

private:
	EditorInterface &m_iface;
	const Settings &m_settings;
};
