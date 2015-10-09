#pragma once
// To mock things and stuff

class EditorInterface {
public:
	enum class ViewType {
		primary,
		secondary,
	};

	enum class codepage {
		ansi,
		utf8
	};

	enum class ViewTarget {
		primary,
		secondary,
		both,
	};

	// non-const
	virtual bool OpenDocument(const std::wstring& filename) = 0;
	virtual void ActivateDocument(int index, ViewType view) = 0;
	virtual void ActivateDocument(const std::wstring& file_path, ViewType view) = 0;
	virtual void SwitchToFile(const std::wstring& path) = 0;
	virtual void MoveActiveDocumentToOtherView() = 0;

	// const
	virtual std::vector<std::wstring> GetOpenFilenames(ViewTarget viewTarget = ViewTarget::both) const = 0;
	virtual ViewType ActiveView() const = 0;
	virtual bool IsOpened(const std::wstring& filename) const = 0;
	virtual codepage GetEncoding(ViewType view) const = 0;
	virtual std::wstring ActiveDocumentPath() const = 0;
	virtual std::wstring ActiveFileDirectory() const = 0;
	virtual std::vector<char> SelectedText(ViewType view) const = 0;
	virtual std::vector<char> GetCurrentLine(ViewType view) const = 0;
	virtual int GetCurrentPos(ViewType view) const = 0;
	virtual int GetCurrentLineNumber(ViewType view) const = 0;
	virtual int LineFromPosition(ViewType view, int position) const = 0;
	virtual int PositionFromLine(ViewType view, int line) const = 0;

	int GetCurrentPosInLine(ViewType view) const { return GetCurrentPos(view) - PositionFromLine(view, GetCurrentLineNumber(view)); }

	virtual ~EditorInterface() {}
};