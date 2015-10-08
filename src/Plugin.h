#pragma once

//
// All difinitions of plugin interface
//
#include "PluginInterface.h"

const std::wstring nppPluginName = L"GoToFile";

const int nbFunc = 4;

enum class CustomGUIMessage;

//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModuleArg);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void pluginInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command
//
bool setNextCommand(TCHAR* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk = nullptr, bool check0nInit = false);

void SaveSettings();
void LoadSettings();
HANDLE getHModule();
