#include "AboutDialog.h"

#include "utils/winapi.h"

#include <resource.h>
#include "Plugin.h"

void AboutDialog::open()
{
	if (!isCreated())
		create(IDD_ABOUT);
	display();
	goToCenter();
}

BOOL AboutDialog::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		Static_SetText(::GetDlgItem(_hSelf, IDC_VERSION), winapi::GetProductAndVersion(static_cast<HMODULE>(getHModule())).data ());
		return TRUE;
	case WM_NOTIFY:
	{
		if (winapi::FilterSyslinks (lParam))
			return TRUE;
		return FALSE;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (HIWORD(wParam) == BN_CLICKED) {
				display(false);
				return TRUE;
			}
			break;
		}
		break;
	}
	return FALSE;
}

