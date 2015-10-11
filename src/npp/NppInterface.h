#pragma once
#include "npp/EditorInterface.h"

#include <vector>

struct NppData;

class NppInterface : public EditorInterface {
public:
	explicit NppInterface(const NppData* nppData);

	static int ToIndex(ViewType target);

	// non-const
	bool OpenDocument(std::wstring filename) override;
	void ActivateDocument(int index, ViewType view) override;
	void ActivateDocument(const std::wstring& filepath, ViewType view) override;
	void SwitchToFile(const std::wstring& path) override;
	void MoveActiveDocumentToOtherView() override;
	void AddToolbarIcon(int cmdId, const toolbarIcons* toolBarIconsPtr) override;

	// const
	std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget = ViewTarget::both) const override;
	ViewType ActiveView() const override;
	bool IsOpened(const std::wstring& filename) const override;
	Codepage GetEncoding(ViewType view) const override;
	std::wstring ActiveDocumentPath() const override;
	std::wstring ActiveFileDirectory() const override;
	std::vector<char> SelectedText(ViewType view) const override;
	std::vector<char> GetCurrentLine(ViewType view) const override;
	int GetCurrentPos(ViewType view) const override;
	int GetCurrentLineNumber(ViewType view) const override;
	int LineFromPosition(ViewType view, int position) const override;
	std::wstring PluginConfigDir() const override;
	int PositionFromLine(ViewType view, int line) const override;
	HWND appHandle() override;

private:
	// these functions are const per se
	LRESULT SendMsgToNpp(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	LRESULT sendMsgToScintilla(ViewType view, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	std::wstring getDirMsg(UINT msg) const;
	void doCommand(int id);

private:
	const NppData& m_nppData;
};

