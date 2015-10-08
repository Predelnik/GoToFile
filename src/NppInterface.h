#pragma once
#include <bemapiset.h>
#include <winuser.h>
#include <codecvt>
#include <vector>

struct NppData;

class NppInterface {
public:
	NppInterface(const NppData* nppData);

	enum class ViewType {
		primary,
		secondary,
	};

	enum class codepage {
		ansi,
		utf8
	};

	static int ToIndex(ViewType target);

	enum class ViewTarget {
		primary,
		secondary,
		both,
	};

	static ViewType OtherView(ViewType view);

	// non-const
	bool OpenDocument(const std::wstring& filename);
	void ActivateDocument(int index, ViewType view);
	void ActivateDocument(const std::wstring& file_path, ViewType view);
	void SwitchToFile(const std::wstring& path);
	void MoveActiveDocumentToOtherView();

	// const
	std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget = ViewTarget::both) const;
	ViewType ActiveView() const;
	bool IsOpened(const std::wstring& filename) const;
	codepage GetEncoding(ViewType view) const;
	std::wstring ActiveDocumentPath() const;
	std::wstring ActiveFileDirectory() const;
	std::vector<char> SelectedText(ViewType view) const;
	std::vector<char> GetCurrentLine (ViewType view) const;
	int GetCurrentPos(ViewType view) const;
	int GetCurrentLineNumber(ViewType view) const;
	int LineFromPosition(ViewType view, int position) const;
	int PositionFromLine(ViewType view, int line) const;
	int GetCurrentPosInLine(ViewType view) const;

private:
	// these functions are const per se
	LRESULT SendMsgToNpp(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	LRESULT sendMsgToScintilla(ViewType view, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	std::wstring getDirMsg(UINT msg) const;
	void doCommand(int id);

private:
	const NppData& m_nppData;
};

