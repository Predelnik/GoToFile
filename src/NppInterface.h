#pragma once
#include <bemapiset.h>
#include <winuser.h>
#include <codecvt>
#include <vector>

struct NppData;

class NppInterface {
public:
	NppInterface(const NppData* nppData);

	LRESULT SendMsgToNpp(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

	enum class ViewType {
		primary,
		secondary,
	};

	LRESULT sendMsgToScintilla(ViewType view, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

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
	std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget = ViewTarget::both);

	ViewType ActiveView();
	bool OpenDocument(const std::wstring& filename);
	bool IsOpened(const std::wstring& filename);
	codepage GetEncoding(ViewType view);
	void ActivateDocument(int index, ViewType view);
	void ActivateDocument(const std::wstring& file_path, ViewType view);
	std::wstring ActiveFilePath();
	void SwitchToFile(const std::wstring& path);

private:
	const NppData& m_nppData;
};

