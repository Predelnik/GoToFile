#include "winapi.h"
#include <assert.h>
#include <sstream>
#include <CommCtrl.h>

static std::wstring getClassName(HWND hwnd) {
	static const int max_class_name = 256;
	std::vector<wchar_t> buf(max_class_name);
	GetClassName(hwnd, buf.data(), max_class_name);
	return {buf.data()};
}

namespace winapi
{
	WinBase::~WinBase() {
	}

	void WinBase::setEnabled(bool enabled) {
		EnableWindow(m_hwnd, enabled ? TRUE : FALSE);
	}

	void WinBase::init(HWND hwnd) {
		m_hwnd = hwnd;
		checkHwnd();
	}

	void CheckBox::checkHwnd() {
		assert(getClassName(m_hwnd) == L"Button");
		assert(GetWindowStyle(m_hwnd) & BS_AUTOCHECKBOX); // exception?
	}

	void CheckBox::SetChecked(bool value) {
		assert(m_hwnd != nullptr);
		Button_SetCheck(m_hwnd, value ? BST_CHECKED : BST_UNCHECKED);
	}

	bool CheckBox::IsChecked() const {
		assert(m_hwnd != nullptr);
		return Button_GetCheck(m_hwnd) == BST_CHECKED;
	}

	void Edit::checkHwnd() {
		assert(getClassName(m_hwnd) == L"Edit");
	}

	void Edit::SetText(const std::wstring& ws) {
		assert(m_hwnd != nullptr);
		Button_SetText(m_hwnd, ws.c_str ());
	}

	std::wstring Edit::GetText() const {
		assert(m_hwnd != nullptr);
		auto len = Button_GetTextLength(m_hwnd);
		std::vector<wchar_t> buf(len + 1);
		Button_GetText(m_hwnd, buf.data (), buf.size ());
		return {buf.data()};
	}

	void Button::checkHwnd() {
		assert(getClassName(m_hwnd) == L"Button");
	}

	std::wstring GetProductAndVersion(HMODULE module) {
		// get the filename of the executable containing the version resource
		TCHAR szFilename[MAX_PATH + 1] = {0};
		if (GetModuleFileName(module, szFilename, MAX_PATH) == 0) {
			return {};
		}

		// allocate a block of memory for the version info
		DWORD dummy;
		DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
		if (dwSize == 0) {
			return {};
		}
		std::vector<BYTE> data(dwSize);

		// load the version info
		if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0])) {
			return {};
		}

		UINT uiVerLen = 0;
		VS_FIXEDFILEINFO* pFixedInfo = 0; // pointer to fixed file info structure
		// get the fixed file info (language-independent)
		if (VerQueryValue(&data[0], TEXT("\\"), (void**)&pFixedInfo, (UINT *)&uiVerLen) == 0) {
			return false;
		}

		std::wstringstream ss;
		ss << "Version: " << HIWORD(pFixedInfo->dwProductVersionMS) << "."
			<< LOWORD(pFixedInfo->dwProductVersionMS) << "."
			<< HIWORD(pFixedInfo->dwProductVersionLS) << "."
			<< LOWORD(pFixedInfo->dwProductVersionLS) << ".";
		return ss.str();
	}

	bool FilterSyslinks(LPARAM lParam) {
		switch (reinterpret_cast<LPNMHDR>(lParam)->code) {
		case NM_CLICK: // Fall through to the next case.
		case NM_RETURN:
			{
				PNMLINK pNMLink = reinterpret_cast<PNMLINK>(lParam);
				LITEM item = pNMLink->item;

				ShellExecute(nullptr, L"open", item.szUrl, nullptr, nullptr, SW_SHOW);

				return TRUE;
			}
		}
		return false;
	}
}

