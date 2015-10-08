#pragma once

namespace winapi
{
	class WinBase {
	public:
		virtual ~WinBase();
		void setEnabled(bool enabled);
		void init(HWND hwnd);

	private:
		virtual void checkHwnd() = 0;

	protected:

		HWND m_hwnd = nullptr;
	};

	class CheckBox : public WinBase {
	public:
		void checkHwnd() override;
		void SetChecked(bool value);
		bool IsChecked();
	};

	class Edit : public WinBase {
	public:
		void checkHwnd() override;
		void SetText(const std::wstring& ws);
		std::wstring GetText();
	};

	class Button : public WinBase {
	public:
		void checkHwnd() override;
	};

	std::wstring GetProductAndVersion(HMODULE module);
	bool FilterSyslinks(LPARAM lParam);
}

