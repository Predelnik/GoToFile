#include "NppInterface.h"
#include "Notepad_plus_msgs.h"
#include <assert.h>
#include "PluginInterface.h"
#include "utils/macro.h"

NppInterface::NppInterface(const NppData* nppData) : m_nppData{*nppData} {
}

LRESULT NppInterface::SendMsgToNpp(UINT Msg, WPARAM wParam, LPARAM lParam) {
	return SendMessage(m_nppData._nppHandle, Msg, wParam, lParam);
}

LRESULT NppInterface::sendMsgToScintilla(ViewType view, UINT Msg, WPARAM wParam, LPARAM lParam) {
	auto handle = m_nppData._scintillaMainHandle;
	switch (view) {
	case ViewType::primary:
		handle = m_nppData._scintillaMainHandle;
		break;
	case ViewType::secondary:
		handle = m_nppData._scintillaSecondHandle;
		break;
	default: break;
	}
	return SendMessage(handle, Msg, wParam, lParam);
}

int NppInterface::ToIndex(ViewType target) {
	switch (target) {
	case ViewType::primary: return 0;
	case ViewType::secondary: return 1;
	}
	assert(false);
	return 0;
}

NppInterface::ViewType NppInterface::OtherView(ViewType view) {
	switch (view) {
	case ViewType::primary: return ViewType::secondary;
	case ViewType::secondary: return ViewType::primary;
	}
	assert(false);
	return ViewType::primary;
}

NppInterface::ViewType NppInterface::ActiveView() {
	int curScintilla = 0;
	SendMsgToNpp(NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&curScintilla));
	if (curScintilla == 0)
		return ViewType::primary;
	else if (curScintilla == 1)
		return ViewType::secondary;

	assert (false);
	return ViewType::primary;
}

bool NppInterface::OpenDocument(const std::wstring& filename) {
	return SendMsgToNpp(NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(filename.data())) == TRUE;
}

bool NppInterface::IsOpened(const std::wstring& filename) {
	auto filenames = GetOpenFilenames(ViewTarget::both);
	return std::find(filenames.begin(), filenames.end(), filename) != filenames.end();
}

NppInterface::codepage NppInterface::GetEncoding(ViewType view) {
	auto CodepageId = static_cast<int>(sendMsgToScintilla(view, SCI_GETCODEPAGE, 0, 0));
	if (CodepageId == SC_CP_UTF8)
		return codepage::utf8;
	else
		return codepage::ansi;
}

void NppInterface::ActivateDocument(int index, ViewType view) {
	SendMsgToNpp(NPPM_ACTIVATEDOC, ToIndex(view), index);
}

void NppInterface::ActivateDocument(const std::wstring& filepath, ViewType view) {
	auto fnames = GetOpenFilenames(ViewTarget::both);
	auto it = std::find(fnames.begin(), fnames.end(), filepath);
	if (it == fnames.end()) // exception prbbly
		return;

	ActivateDocument(it - fnames.begin(), view);
}

std::wstring NppInterface::ActiveFilePath() {
	std::vector<wchar_t> buf(MAX_PATH);
	SendMsgToNpp(NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(buf.data()));
	return {buf.begin(), buf.end()};
}

void NppInterface::SwitchToFile(const std::wstring& path) {
	SendMsgToNpp(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(path.data()));
}

std::vector<std::wstring> NppInterface::GetOpenFilenames(ViewTarget viewTarget) {
	int enumVal = -1;

	switch (viewTarget) {
	case ViewTarget::primary:
		enumVal = PRIMARY_VIEW;
		break;
	case ViewTarget::secondary:
		enumVal = SECOND_VIEW;
		break;
	case ViewTarget::both:
		enumVal = ALL_OPEN_FILES;
		break;
	}

	ASSERT_RETURN(enumVal >= 0, {});
	int count = SendMsgToNpp(NPPM_GETNBOPENFILES, 0, enumVal);

	wchar_t** paths;
	paths = new wchar_t *[count];
	for (int i = 0; i < count; ++i)
		paths[i] = new wchar_t[MAX_PATH];

	int msg = -1;
	switch (viewTarget) {
	case ViewTarget::primary:
		msg = NPPM_GETOPENFILENAMESPRIMARY;
		break;
	case ViewTarget::secondary:
		msg = NPPM_GETOPENFILENAMESSECOND;
		break;
	case ViewTarget::both:
		msg = NPPM_GETOPENFILENAMES;
		break;
	}

	ASSERT_RETURN(msg >= 0, {}); {
		auto ret = SendMsgToNpp(msg, reinterpret_cast<WPARAM>(paths), count);
		ASSERT_RETURN(ret == count, {});
	}

	std::vector<std::wstring> res;
	for (int i = 0; i < count; ++i) {
		res.push_back(paths[i]);
		delete paths[i];
	}
	delete paths;
	return res;
}

