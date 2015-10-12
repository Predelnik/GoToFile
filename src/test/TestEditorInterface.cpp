#include "TestEditorInterface.h"

#include <filesystem>

#include "utils/enum_range.h"
#include "utils/misc.h"

namespace fs = std::experimental::filesystem::v1;

TestEditorInterface::TestEditorInterface() : m_eng{std::random_device{}()} {

}

bool TestEditorInterface::OpenDocument(std::wstring filename) {
	filename = fs::canonical(filename);

	if (!fs::is_regular_file(filename))
		return false;

	for (auto view : enum_range<ViewType>()) {
		auto opt_it = find_c(m_viewData[view].fileList, filename);
		if (opt_it)
		   return true;
	}

	auto& data = ActiveViewData();
	auto p = data.fileList.insert(fs::canonical(filename));
	if (!data.activeFile)
		data.activeFile = p.first;
	return true;
}

void TestEditorInterface::ActivateDocument(int index, ViewType view) {
	auto& data = m_viewData[view];
	if (static_cast<int>(data.fileList.size()) >= index)
		return;

	data.activeFile = std::next(data.fileList.begin(), index);
}

void TestEditorInterface::ActivateDocument(const std::wstring& filepath, ViewType view) {
	auto& data = m_viewData[view];
	auto opt_it = find_c(data.fileList, filepath);
	if (opt_it)
		data.activeFile = opt_it;
}

void TestEditorInterface::SwitchToFile(const std::wstring& path) {
	for (auto view : enum_range<ViewType>()) {
		auto& data = m_viewData[view];
		auto opt_it = find_c(data.fileList, path);
		if (opt_it) {
			data.activeFile = opt_it;
			m_activeView = view;
			return;
		}
	}
}

void TestEditorInterface::MoveActiveDocumentToOtherView() {
	if (!m_viewData[m_activeView].activeFile)
		return;

	auto it = ActiveViewData().activeFile.value();
	auto file = *it;
	ActiveViewData().fileList.erase(it);
	ActiveViewData().activeFile = nullopt;
	if (!ActiveViewData().fileList.empty())
		ActiveViewData().activeFile = ActiveViewData().fileList.begin();

	m_activeView = OtherView(m_activeView);
	auto p = ActiveViewData().fileList.insert(file);
	ActiveViewData().activeFile = p.first;
}

void TestEditorInterface::AddToolbarIcon(int /*cmdId*/, const toolbarIcons* /*toolBarIconsPtr*/) {
	// Don't even think about it
	assert(false);
}

std::vector<std::wstring> TestEditorInterface::GetOpenFilenames(ViewTarget viewTarget) const {
	std::vector<std::wstring> ret;
	for (auto view : enum_range<ViewType>()) {
		ViewTarget target = ViewTarget::primary;
		switch (view) {
		case ViewType::primary:
			target = ViewTarget::primary;
			break;
		case ViewType::secondary:
			target = ViewTarget::secondary;
			break;
		case ViewType::COUNT:
			assert(false);
			break;
		}
		if (viewTarget == target || viewTarget == ViewTarget::both) {
			auto& list = m_viewData[view].fileList;
			std::copy(list.begin(), list.end(), std::back_inserter(ret));
		}
	}
	return ret;
}

EditorInterface::ViewType TestEditorInterface::ActiveView() const {
	return m_activeView;
}

bool TestEditorInterface::IsOpened(const std::wstring& filename) const {
	for (auto view : enum_range<ViewType>()) {
		if (m_viewData[view].fileList.count(filename) > 0)
			return true;
	}
	return false;
}

EditorInterface::Codepage TestEditorInterface::GetEncoding(ViewType view) const {
	return m_viewData[view].codepage;
}

std::wstring TestEditorInterface::ActiveDocumentPath() const {
	if (ActiveViewData().activeFile)
		return *ActiveViewData().activeFile.value();
	else
		return {};
}

std::wstring TestEditorInterface::ActiveFileDirectory() const {
	return fs::path(ActiveDocumentPath()).parent_path();
}

std::wstring TestEditorInterface::PluginConfigDir() const {
	// Problems?
	assert(false);
	return {};
}

std::vector<char> TestEditorInterface::SelectedText(ViewType /*view*/) const {
	if (!m_selectionEnd)
		return {};

	std::vector<char> res;
	std::copy(std::next(m_str.base().begin(), m_selectionStart), std::next(m_str.base().begin(), m_selectionEnd.value()), std::back_inserter(res));
	res.push_back('\0');
	return res;
}

std::vector<char> TestEditorInterface::GetCurrentLine(ViewType /*view*/) const {
	std::vector<char> res{m_str.base().begin(), m_str.base().end()};
	res.push_back('\0');
	return res;
}

int TestEditorInterface::GetCurrentPos(ViewType /*view*/) const {
	return m_offset + m_selectionStart;
}

int TestEditorInterface::GetCurrentLineNumber(ViewType /*view*/) const {
	return m_lineNumber;
}

int TestEditorInterface::LineFromPosition(ViewType /*view*/, int /*position*/) const {
	return m_lineNumber;
}

int TestEditorInterface::PositionFromLine(ViewType /*view*/, int /*line*/) const {
	return m_offset;
}

HWND TestEditorInterface::appHandle() {
	return nullptr;
}

void TestEditorInterface::RandomizeLineAndOffset() {
	std::uniform_int_distribution<> distr(0, 100000);
	m_lineNumber = distr(m_eng);
	m_offset = distr(m_eng);
}

void TestEditorInterface::SetViewCodepage(ViewType view, Codepage cp) {
	m_viewData[view].codepage = cp;
}

void TestEditorInterface::CloseActiveFile() {
	if (ActiveViewData().activeFile) {
		ActiveViewData().fileList.erase(ActiveViewData().activeFile.value());
		if (!ActiveViewData().fileList.empty())
			ActiveViewData().activeFile = ActiveViewData().fileList.begin();
		else
			ActiveViewData().activeFile = nullopt;
	}
	if (m_activeView == ViewType::secondary && ActiveViewData().fileList.empty())
		m_activeView = ViewType::primary;
}

