#pragma once
#include "controls/StaticDialog/StaticDialog.h"

class AboutDialog : public StaticDialog {
public:
	void open();
private:
	BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
