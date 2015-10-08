#pragma once
#include "utils/utf8string.h"

class IniWorker;

struct Settings {
	utf8string additionalStopChars = R"(;()'"<>[])"s;
	bool openInOtherView = true;
	bool switchToNewlyOpenedFiles = true;
	bool openLargeFilesInOtherProgram = false;
	int largeFileSizeLimit = 25;
	utf8string customEditorPath = "";

	bool IsStopChar(char ch);

	bool IsStopChar(utf8string::iterator it);

public:
	void ProcessSettings(IniWorker& worker);

private:
	utf8string StopChars() {
		return additionalStopChars + forbiddenChars;
	}

	utf8string forbiddenChars = R"(<>:"|?*)"s; // File which cannot be in windows filename or path
};

