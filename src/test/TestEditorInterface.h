#pragma once

#include <npp/EditorInterface.h>

#include "utils/enum_array.h"
#include "utils/optional.h"
#include "utils/utf8string.h"

class TestEditorInterface : public EditorInterface {

public:
	TestEditorInterface();

	// non const
	bool OpenDocument(std::wstring filename) override;
	void ActivateDocument(int index, ViewType view) override;
	void ActivateDocument(const std::wstring& filepath, ViewType view) override;
	void SwitchToFile(const std::wstring& path) override;
	void MoveActiveDocumentToOtherView() override;
	void AddToolbarIcon(int cmdId, const toolbarIcons* toolBarIconsPtr) override;
	// const
	std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget) const override;
	ViewType ActiveView() const override;
	bool IsOpened(const std::wstring& filename) const override;
	Codepage GetEncoding(ViewType view) const override;
	std::wstring ActiveDocumentPath() const override;
	std::wstring ActiveFileDirectory() const override;
	std::wstring PluginConfigDir() const override;
	std::vector<char> SelectedText(ViewType view) const override;
	std::vector<char> GetCurrentLine(ViewType view) const override;
	int GetCurrentPos(ViewType view) const override;
	int GetCurrentLineNumber(ViewType view) const override;
	int LineFromPosition(ViewType view, int position) const override;
	int PositionFromLine(ViewType view, int line) const override;
	HWND appHandle() override;

	void SetString(const utf8string &str) { m_str = str; }
	void SetSelection(int start, optional<int> end = nullopt) { m_selectionStart = start; m_selectionEnd = end; }
	void RandomizeLineAndOffset();
	void SetViewCodepage(ViewType view, Codepage cp);
	void CloseActiveFile();

private:
	struct ViewData {
		using FileListType = std::unordered_set<std::wstring>;
		FileListType fileList;
		Codepage codepage = Codepage::utf8;
		optional<FileListType::const_iterator> activeFile;
	};

	ViewType m_activeView = ViewType::primary;
	ebc::enum_array<ViewType, ViewData> m_viewData;

	ViewData &ActiveViewData() { return m_viewData[m_activeView]; };
	const ViewData &ActiveViewData() const { return m_viewData[m_activeView]; };

	utf8string m_str;
	int m_lineNumber = 0;
	int m_offset = 0;
	int m_selectionStart = 0;
	optional<int> m_selectionEnd; // nullopt means
	std::default_random_engine m_eng;
};
