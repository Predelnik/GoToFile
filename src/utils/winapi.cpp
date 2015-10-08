#include "winapi.h"
#include <assert.h>

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

	bool CheckBox::IsChecked() {
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

	std::wstring Edit::GetText() {
		assert(m_hwnd != nullptr);
		auto len = Button_GetTextLength(m_hwnd);
		std::vector<wchar_t> buf(len + 1);
		Button_GetText(m_hwnd, buf.data (), buf.size ());
		return {buf.data()};
	}

	void Button::checkHwnd() {
		assert(getClassName(m_hwnd) == L"Button");
	}
}

