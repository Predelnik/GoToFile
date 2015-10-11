#include "npp/NppInterface.h"

#include "Plugin.h"
#include "Notepad_plus_msgs.h"
#include "Settings.h"
#include "SettingsDialog.h"

#include "utils/raii.h"

#include <filesystem>
#include "utils/IniWorker.h"

#include "AboutDialog.h"
#include "GotoFileManager.h"

std::wstring configFileName = L"GoToFile.ini";
FuncItem funcItem[nbFunc];
//
// The data of Notepad++ that you can use in your plugin commands
//
NppData g_nppData;
std::wstring g_iniFilePath;
HANDLE hModule = nullptr;
Settings g_settings;
NppInterface g_iface(&g_nppData);
SettingsDialog g_settingsDialog(&g_settings);
AboutDialog g_aboutDialog;
GotoFileManager g_fileManager (&g_iface, &g_settings);

FuncItem* get_funcItem() {
	return funcItem;
}

void SaveSettings() {
	IniWorker worker{nppPluginName, g_iniFilePath, IniWorker::Action::save};
	g_settings.ProcessSettings(worker);
}

void LoadSettings() {
	IniWorker worker{nppPluginName, g_iniFilePath, IniWorker::Action::load};
	g_settings.ProcessSettings(worker);
}


void InitDialogs() {
	auto init = [&](auto& dlg) {
		dlg.init(static_cast<HINSTANCE>(hModule), g_nppData._nppHandle);
	};
	init(g_settingsDialog);
	init(g_aboutDialog);
}

void pluginInit(HANDLE hModuleArg) {
	hModule = hModuleArg;
}

HANDLE getHModule() {
	return hModule;
}

void pluginCleanUp() {
}

void GotoFile() {
	g_fileManager.GoToFile();
}

void OpenSettingsDialog() {
	g_settingsDialog.open();
}

void OpenAboutDialog() {
	g_aboutDialog.open();
}

void prepareIniFile(const std::wstring& path) {
	namespace fs = std::experimental::filesystem::v1;
	if (!fs::exists(path)) {
		// UTF16-LE BOM(FFFE)
		WORD wBOM = 0xFEFF;
		DWORD NumberOfBytesWritten;

		HANDLE hFile = ::CreateFile(path.data(), GENERIC_WRITE, 0,
		                            nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		::WriteFile(hFile, &wBOM, sizeof(WORD), &NumberOfBytesWritten, nullptr);
		::CloseHandle(hFile);
	}
}

void CommandMenuInit() {
	//
	// Firstly we get the parameters from your plugin config file (if any)
	//

	// get path of plugin configuration

	namespace fs = std::experimental::filesystem::v1;
	auto config_dir = g_iface.PluginConfigDir();
	// if config path doesn't exist, we create it
	if (!fs::exists(config_dir)) {
		fs::create_directories(config_dir);
	}

	// make your plugin config file full file path name
	g_iniFilePath = fs::path(config_dir) / configFileName;

	prepareIniFile(g_iniFilePath);

	//--------------------------------------------//
	//-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
	//--------------------------------------------//
	// with function :
	// setCommand(int index,                      // zero based number to indicate the order of command
	//            TCHAR *commandName,             // the command name that you want to see in plugin menu
	//            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
	//            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
	//            bool check0nInit                // optional. Make this menu item be checked visually
	//            );
	ShortcutKey* shKey = new ShortcutKey;
	shKey->_isAlt = true;
	shKey->_isCtrl = false;
	shKey->_isShift = false;
	shKey->_key = 0x41 + 'g' - 'a';
	setNextCommand(TEXT("Go to File Under Cursor"), GotoFile, shKey, false);
	setNextCommand(TEXT("---"), nullptr, nullptr, false);
	setNextCommand(TEXT("Settings..."), OpenSettingsDialog, nullptr, false);
	setNextCommand(TEXT("About"), OpenAboutDialog, nullptr, false);
}

//
// Initialization of your plug-in commands
// You should fill your plug-ins commands here
void pluginInit() {
	CommandMenuInit();
	LoadSettings();
	InitDialogs();
}

std::unique_ptr<ToolbarIconsWrapper> GotoFileIcon;

void AddIcons() {
	// GotoFileIcon = std::make_unique<ToolbarIconsWrapper> ((HINSTANCE)hModule, MAKEINTRESOURCE(IDB_AUTOCHECK2), IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS);
	g_iface.AddToolbarIcon(funcItem[0]._cmdID, GotoFileIcon->get());
}

void commandMenuCleanUp() {
	delete funcItem[0]._pShKey; // TODO: Wrap it in some RAII
}

//
// Function that initializes plug-in commands
//
static int counter = 0;

bool setNextCommand(TCHAR* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk, bool check0nInit) {
	if (counter >= nbFunc)
		return false;

	if (!pFunc) {
		counter++;
		return false;
	}

	lstrcpy(funcItem[counter]._itemName, cmdName);
	funcItem[counter]._pFunc = pFunc;
	funcItem[counter]._init2Check = check0nInit;
	funcItem[counter]._pShKey = sk;
	counter++;

	return true;
}

