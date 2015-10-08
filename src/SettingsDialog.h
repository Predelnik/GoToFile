#pragma once
#include "StaticDialog/StaticDialog.h"
#include "utils/winapi.h"

struct Settings;

class SettingsDialog : public StaticDialog {
public:
	explicit SettingsDialog(Settings* settings);
	void open();

private:
	void InitHandles();
	void FillFromSettings();
	void FillToSettings();
	void updateControlAvailbility();
	BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	winapi::CheckBox m_openInOtherViewCheckBox;
	winapi::CheckBox m_swithToNewlyOpenedFilesCheckbox;
	winapi::Edit m_stopCharctersEdit;
	winapi::CheckBox m_openLargeFilesInOtherProgramCheckbox;
	winapi::Edit m_largeFileSizeLimitEdit;
	winapi::Edit m_customEditorPathEdit;
	winapi::Button m_customEditorBrowseButton;
	Settings& m_settings;
};

