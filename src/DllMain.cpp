#include <windows.h>

#include "Plugin.h"

#ifdef VLD_BUILD
#include <vld.h>
#endif //VLD_BUILD

static_assert (UNICODE, "Non-unicode version is not supported");

extern FuncItem funcItem[nbFunc];
extern NppData g_nppData;
HANDLE HModule;

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD reasonForCall,
                      LPVOID) {
	HModule = hModule;

	switch (reasonForCall) {
	case DLL_PROCESS_ATTACH:
		pluginInit(hModule);
		break;

	case DLL_PROCESS_DETACH:
		//_CrtDumpMemoryLeaks();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

WPARAM LastHwnd = NULL;
LPARAM LastCoords = 0;

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData) {
	g_nppData = notpadPlusData;
	pluginInit();
}

extern "C" __declspec(dllexport) const wchar_t* getName() {
	return nppPluginName.data();
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF) {
	*nbF = nbFunc;
	return funcItem;
}


extern "C" __declspec (dllexport) void beNotified(SCNotification* notifyCode) {
	switch (notifyCode->nmhdr.code) {
	case NPPN_SHUTDOWN:
		pluginCleanUp();
		break;

	case NPPN_READY:
		break;

	default:
		return;
	}
}

// Here you can process the Npp Messages
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {
	//   switch (Message)
	//   {
	//   }

	return FALSE;
}

extern "C" __declspec(dllexport) BOOL isUnicode() {
	return TRUE;
}

