#include "Settings.h"
#include "utils/IniWorker.h"

static bool is_control(char c) {
	return c >= '\0' && c < ' ';
}

bool Settings::IsStopChar(char ch) const {
	utf8string str(std::string(1, ch));
	auto sc = StopChars();
	return is_control(ch) || sc.find(str.begin()) != sc.end();
}

bool Settings::IsStopChar(utf8string::iterator it) const {
	auto sc = StopChars();
	return is_control(*it) || sc.find(it) != sc.end();
}

void Settings::ProcessSettings(IniWorker& worker) {
	worker.process(L"openInOtherView", openInOtherView);
	worker.process(L"switchToNewlyOpenedFiles", switchToNewlyOpenedFiles);
	worker.process(L"additionalStopChars", additionalStopChars);
	worker.process(L"openLargeFilesInOtherProgram", openLargeFilesInOtherProgram);
	worker.process(L"largeFileSizeLimit", largeFileSizeLimit);
	worker.process(L"customEditorPath", customEditorPath);
}

