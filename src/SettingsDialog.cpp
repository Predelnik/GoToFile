#include "SettingsDialog.h"
#include "resource.h"
#include "Settings.h"
#include "Plugin.h"

SettingsDialog::SettingsDialog(Settings* settings) : m_settings{ *settings }
{
}

void SettingsDialog::open()
{
	if (!isCreated())
	  create(IDD_SETTINGS);
	display();
	goToCenter();
}

void SettingsDialog::InitHandles()
{
	auto init = [&](auto &cntrl, auto handle) { cntrl.init(GetDlgItem(_hSelf, handle)); };
	init(m_openInOtherViewCheckBox, IDC_OPEN_IN_OTHER_VIEW_CHECKBOX);
	init(m_swithToNewlyOpenedFilesCheckbox, IDC_SWITCH_TO_NEWLY_OPENED_FILES_CHECKBOX);
	init(m_stopCharctersEdit, IDC_STOP_CHARACTERS_EDIT);
	init(m_openLargeFilesInOtherProgramCheckbox, IDC_OPEN_LARGE_FILES_IN_OTHER_PROGRAM_CHECKBOX);
	init(m_largeFileSizeLimitEdit, IDC_LARGE_FILE_SIZE_LIMIT_EDIT);
	init(m_customEditorPathEdit, IDC_CUSTOM_EDITOR_PATH_EDIT);
	init(m_customEditorBrowseButton, IDC_CUSTOM_EDITOR_PATH_BROWSE_BUTTON);
}

void SettingsDialog::FillFromSettings()
{
	m_openInOtherViewCheckBox.SetChecked(m_settings.openInOtherView);
	m_swithToNewlyOpenedFilesCheckbox.SetChecked(m_settings.switchToNewlyOpenedFiles);
	m_stopCharctersEdit.SetText(m_settings.additionalStopChars.to_wstring().c_str());
	m_openLargeFilesInOtherProgramCheckbox.SetChecked(m_settings.openLargeFilesInOtherProgram);
	m_largeFileSizeLimitEdit.SetText(std::to_wstring (m_settings.largeFileSizeLimit));
	m_customEditorPathEdit.SetText(m_settings.customEditorPath.to_wstring());
	updateControlAvailbility();
}

void SettingsDialog::FillToSettings()
{
	m_settings.openInOtherView = m_openInOtherViewCheckBox.IsChecked();
	m_settings.switchToNewlyOpenedFiles = m_swithToNewlyOpenedFilesCheckbox.IsChecked();
	m_settings.additionalStopChars = m_stopCharctersEdit.GetText();
	m_settings.openLargeFilesInOtherProgram = m_openLargeFilesInOtherProgramCheckbox.IsChecked();
	try { m_settings.largeFileSizeLimit = std::stoi (m_largeFileSizeLimitEdit.GetText()); } catch (...) {}
	m_settings.customEditorPath = m_customEditorPathEdit.GetText();
	SaveSettings();
}

void SettingsDialog::updateControlAvailbility()
{
	for (auto *control : std::initializer_list<winapi::WinBase *>{ &m_largeFileSizeLimitEdit, &m_customEditorPathEdit, &m_customEditorBrowseButton })
		control->setEnabled (m_openLargeFilesInOtherProgramCheckbox.IsChecked());
}

BOOL SettingsDialog::run_dlgProc(UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch (message)
	{
	case WM_INITDIALOG:
		InitHandles();
		return TRUE;
	case WM_SHOWWINDOW:
		{
			if (wParam)
			FillFromSettings();
	}

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_OPEN_LARGE_FILES_IN_OTHER_PROGRAM_CHECKBOX:
			if (HIWORD(wParam) == BN_CLICKED) {
				updateControlAvailbility();
			}
			break;
		case IDC_CUSTOM_EDITOR_PATH_BROWSE_BUTTON:
			{
				if (HIWORD(wParam) == BN_CLICKED) {
					OPENFILENAME s;
					memset(&s, 0, sizeof (s));
					s.lStructSize = sizeof(s);
					s.hwndOwner = _hSelf;
					s.lpstrFilter = L"Executables\0*.exe\0";
					s.lpstrTitle = L"Select Editor Executable";
					std::vector<wchar_t> buf(MAX_PATH);
					s.nMaxFile = MAX_PATH;
					buf[0] = '\0';
					s.lpstrFile = buf.data ();
					if (GetOpenFileName(&s))
					{
						m_customEditorPathEdit.SetText(s.lpstrFile);
					}
				}
			}
			break;
		case IDOK:
			if (HIWORD(wParam) == BN_CLICKED) {
				FillToSettings();
				display(false);
				return TRUE;
			}
			break;
		case IDCANCEL:
			if (HIWORD(wParam) == BN_CLICKED) {
				display(false);
				return TRUE;
			}
			break;
		}
	}
	}
	return FALSE;
}
