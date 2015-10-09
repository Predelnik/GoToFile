#include "npp/NppInterface.h"

#include "Plugin.h"
#include "Notepad_plus_msgs.h"
#include "Settings.h"
#include "SettingsDialog.h"

#include "utils/raii.h"

#include <filesystem>
#include "utils/IniWorker.h"

#include "AboutDialog.h"

std::wstring configFileName = L"GoToFile.ini";
FuncItem funcItem[nbFunc];
//
// The data of Notepad++ that you can use in your plugin commands
//
NppData g_nppData;
std::wstring g_iniFilePath;
HANDLE hModule = nullptr;
Settings g_Settings;
NppInterface iface(&g_nppData);
SettingsDialog g_settingsDialog(&g_Settings);
AboutDialog g_aboutDialog;

FuncItem* get_funcItem() {
	return funcItem;
}

void SaveSettings() {
	IniWorker worker{nppPluginName, g_iniFilePath, IniWorker::Action::save};
	g_Settings.ProcessSettings(worker);
}

void LoadSettings() {
	IniWorker worker{nppPluginName, g_iniFilePath, IniWorker::Action::load};
	g_Settings.ProcessSettings(worker);
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

void TryOpenFile(const std::wstring& filename) {
	auto curPath = iface.ActiveFileDirectory();
	namespace fs = std::experimental::filesystem::v1;
	auto tryPath = [&](fs::path path) {
		if (path.empty())
			return false;
		std::error_code ec;

		if (fs::is_regular_file(path)) {
			path = fs::canonical(path, ec);
			if (ec)
				return false;

			if (fs::file_size(path) > g_Settings.largeFileSizeLimit * 1024 * 1024) {
				return reinterpret_cast<int>(ShellExecute(g_nppData._nppHandle, L"open", g_Settings.customEditorPath.to_wstring().data(), path.c_str(), nullptr, SW_SHOW)) > 32;
			}

			auto initialFileName = iface.ActiveDocumentPath();
			if (iface.IsOpened(path))
				return false;

			if (!iface.OpenDocument(path))
				return false;

			iface.SwitchToFile(path);

			if (g_Settings.openInOtherView)
				iface.MoveActiveDocumentToOtherView();

			iface.SwitchToFile(initialFileName);
			if (g_Settings.switchToNewlyOpenedFiles)
				iface.SwitchToFile(path);
		}

		return false;
	};

	// absolute path
	if (tryPath(filename)) return;

	// relative path
	if (tryPath(fs::absolute(filename, curPath))) return;
}

std::wstring extract_filename(const std::vector<char>& buf, int index) {
	auto activeView = iface.ActiveView();
	switch (iface.GetEncoding(activeView)) {
	case NppInterface::codepage::ansi:
		{
			int start = index, end = index;
			auto isPosInStopChar = [&](int Index) {
				return g_Settings.IsStopChar(buf[Index]);
			};
			int n = static_cast<int>(buf.size());
			if (start == n)
				--start;
			while (!isPosInStopChar(start) && start > 0)
				--start;
			if (isPosInStopChar(start))
				++start;

			while (end < n && !isPosInStopChar(end))
				++end;

			if (start < end)
				return {std::wstring(buf.begin() + start, buf.begin() + end)};
		}
		break;
	case NppInterface::codepage::utf8:
		{
			utf8string u8str{buf.data()};
			utf8string::iterator cur_it(u8str.base().begin() + index);
			auto start = cur_it, end = cur_it;
			auto isPosInStopChar = [&](auto it) {
				return g_Settings.IsStopChar(it);
			};
			if (start == u8str.end())
				--start;
			while (!isPosInStopChar(start) && start != u8str.begin())
				--start;
			if (isPosInStopChar(start))
				++start;

			while (end != u8str.end() && !isPosInStopChar(end))
				++end;

			if (start < end)
				return utf8string(start, end).to_wstring();
		}
		break;
	}


	return {};
}

void GotoFile() {
	auto activeView = iface.ActiveView();
	auto buf = iface.SelectedText (activeView);
	if (!buf.empty ()) // Because it includes terminating '\0'
	{
		switch (iface.GetEncoding(activeView)) {
		case NppInterface::codepage::ansi:
			{
				std::string str(buf.data());
				TryOpenFile({str.begin(), str.end()});
			}
			break;
		case NppInterface::codepage::utf8:
			{
				TryOpenFile(utf8string(std::string(buf.data())).to_wstring());
			}
			break;
		}

		return;
	}
	TryOpenFile(extract_filename(iface.GetCurrentLine(activeView), iface.GetCurrentPosInLine(activeView)));
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
	std::vector<wchar_t> buf(MAX_PATH);
	::SendMessage(g_nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(buf.data()));
	namespace fs = std::experimental::filesystem::v1;

	std::wstring config_dir{buf.data()};
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
	setNextCommand(TEXT("---"), NULL, NULL, false);
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
	::SendMessage(g_nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[0]._cmdID, (LPARAM) GotoFileIcon->get());
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

