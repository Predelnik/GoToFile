#pragma once
#include <assert.h>
// To mock things and stuff

struct toolbarIcons;

class EditorInterface {
public:
	enum class ViewType {
		primary,
		secondary,

		COUNT,
	};

	enum class Codepage {
		ansi,
		utf8,

		COUNT,
	};

	enum class ViewTarget {
		primary,
		secondary,
		both,
	};

	static ViewType OtherView(ViewType view) {
		switch (view) {
		case ViewType::primary: return ViewType::secondary;
		case ViewType::secondary: return ViewType::primary;
		}
		assert(false);
		return ViewType::primary;
	}

	// non-const
	virtual bool OpenDocument(std::wstring filename) = 0;
	virtual void ActivateDocument(int index, ViewType view) = 0;
	virtual void ActivateDocument(const std::wstring& filepath, ViewType view) = 0;
	virtual void SwitchToFile(const std::wstring& path) = 0;
	virtual void MoveActiveDocumentToOtherView() = 0;
	virtual void AddToolbarIcon(int cmdId, const toolbarIcons* toolBarIconsPtr) = 0;

	// const
	virtual std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget = ViewTarget::both) const = 0;
	virtual ViewType ActiveView() const = 0;
	virtual bool IsOpened(const std::wstring& filename) const = 0;
	virtual Codepage GetEncoding(ViewType view) const = 0;
	virtual std::wstring ActiveDocumentPath() const = 0;
	virtual std::wstring ActiveFileDirectory() const = 0;
	virtual std::wstring PluginConfigDir() const = 0;
	virtual std::vector<char> SelectedText(ViewType view) const = 0;
	virtual std::vector<char> GetCurrentLine(ViewType view) const = 0;
	virtual int GetCurrentPos(ViewType view) const = 0;
	virtual int GetCurrentLineNumber(ViewType view) const = 0;
	virtual int LineFromPosition(ViewType view, int position) const = 0;
	virtual int PositionFromLine(ViewType view, int line) const = 0;
	virtual HWND appHandle() = 0;

	int GetCurrentPosInLine(ViewType view) const {
		return GetCurrentPos(view) - PositionFromLine(view, GetCurrentLineNumber(view));
	}

	virtual ~EditorInterface() {
	}
};

