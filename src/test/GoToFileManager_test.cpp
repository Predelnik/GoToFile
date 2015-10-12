#include "gtest/gtest.h"

#include "TestEditorInterface.h"

#include "Settings.h"
#include "utils/misc.h"

#include <fstream>
#include <filesystem>
#include <GotoFileManager.h>
#include <utils/enum_range.h>

namespace fs = std::experimental::filesystem::v1;

class TestEditorInterface;

struct FiletructureKeeper {
	enum class EntityType {
		file,
		dir,
	};

	struct EntityKeeper : Uncopiable {
		using self = EntityKeeper;

		EntityKeeper(const std::wstring& name, EntityType type) : m_name(name) {
			switch (type) {
			case EntityType::file:
				{
					std::ofstream f(m_name);
					f << "1"; // 1 byte files
				}
				break;
			case EntityType::dir:
				fs::create_directory(m_name);
				break;
			}
		}

		EntityKeeper(self&&) = default;
		self& operator=(self&&) = default;

		~EntityKeeper() {
			fs::remove(m_name);
		}

	private:
		std::wstring m_name;
	};

	FiletructureKeeper() {
		auto add_file = [this](auto name) { m_entities.push_back(new EntityKeeper(name, EntityType::file)); };
		auto add_dir = [this](auto name) { m_entities.push_back(new EntityKeeper(name, EntityType::dir));  };
		add_file(L"a.txt");
		add_file(L"b.txt");
		add_dir(L"dir_a");
		add_file(L"dir_a\\c.txt");
	}

	~FiletructureKeeper() {
		for (auto it = m_entities.rbegin(); it != m_entities.rend(); ++it)
			delete (*it);
	}

private:
	std::vector<EntityKeeper*> m_entities; // manual construction to enforce destruction order
};

TEST (GoToFile, basic) {
	FiletructureKeeper keep;
	TestEditorInterface iface;
	Settings settings;
	GotoFileManager manager{&iface, &settings};
	using vt = EditorInterface::ViewType;

	auto cur_file = [&] {
		return fs::path(iface.ActiveDocumentPath()).filename().wstring();
	};

	EXPECT_TRUE (iface.OpenDocument(L"a.txt"));
	EXPECT_EQ(vt::primary, iface.ActiveView());
	EXPECT_EQ(L"a.txt", cur_file ());
	iface.RandomizeLineAndOffset();

	// This is macro just for better error reporting, don't stress it
#define CHECK                                     \
 	manager.GoToFile();                           \
	EXPECT_EQ(vt::secondary, iface.ActiveView()); \
	EXPECT_EQ(L"b.txt", cur_file());              \
	iface.CloseActiveFile();                      \
	EXPECT_EQ(L"a.txt", cur_file());              \
	iface.SetSelection (0);                       \
    iface.RandomizeLineAndOffset();

	settings.largeFileSizeLimit = 0; // checking that openLargeFilesInOtherProgram switch works

	for (auto enc : enum_range<EditorInterface::Codepage>()) {
		iface.SetViewCodepage(EditorInterface::ViewType::primary, enc);

		iface.SetString(utf8string::from_raw_utf8("b.txt"));
		CHECK;

		iface.SetString(utf8string::from_raw_utf8(R"(dir_a\../b.txt)")); // twisted path
		CHECK;

		iface.SetString(utf8string{fs::absolute(L"b.txt").wstring()}); // absolute path
		CHECK;

		iface.SetString(utf8string::from_raw_utf8("b.txt")); // absolute path
		iface.SetSelection(5);
		CHECK;

		iface.SetString(utf8string::from_raw_utf8("ssssb.txtssss")); // precise selection
		iface.SetSelection(4, 9);
		CHECK;

		iface.SetString(utf8string::from_raw_utf8("(********b.txt"""")")); // stopping on stop symbols
		iface.SetSelection(10);
		CHECK;

		iface.SetSelection(1);
		manager.GoToFile();
		EXPECT_EQ(L"a.txt", cur_file());
	}

	settings.openLargeFilesInOtherProgram = true;
	settings.customEditorPath = utf8string::from_raw_utf8("rundll32");
	iface.SetString(utf8string::from_raw_utf8("b.txt")); // running external program check
	manager.GoToFile();
	EXPECT_EQ(L"a.txt", cur_file());

	settings.switchToNewlyOpenedFiles = false;
	settings.openLargeFilesInOtherProgram = false;
	iface.SetString(utf8string::from_raw_utf8("b.txt"));
	manager.GoToFile();
	EXPECT_EQ(L"a.txt", cur_file());
	EXPECT_EQ(std::vector<std::wstring> { fs::canonical(L"b.txt") }, iface.GetOpenFilenames(EditorInterface::ViewTarget::secondary));
	iface.CloseActiveFile();
	iface.SwitchToFile(fs::canonical(L"b.txt"));
	iface.CloseActiveFile();

	iface.OpenDocument(L"dir_a/c.txt");
	iface.SetString(utf8string::from_raw_utf8("<../b.txt>"));
	iface.SetSelection(4);
	settings.openInOtherView = false;
	settings.switchToNewlyOpenedFiles = true;
	manager.GoToFile();
	EXPECT_EQ(L"b.txt", cur_file());
	EXPECT_EQ(vt::primary, iface.ActiveView());
	EXPECT_TRUE (iface.GetOpenFilenames(EditorInterface::ViewTarget::secondary).empty());
	iface.SetString(utf8string::from_raw_utf8("dir_a/c.txt"));
	manager.GoToFile();
	EXPECT_EQ(L"b.txt", cur_file());
	EXPECT_EQ(vt::primary, iface.ActiveView());
	iface.CloseActiveFile();

	// Checking that files relative to working dir should not work
	iface.SetString(utf8string::from_raw_utf8("??b.txt??"));
	manager.GoToFile();
	EXPECT_EQ(L"c.txt", cur_file());
	EXPECT_EQ(vt::primary, iface.ActiveView());
	EXPECT_EQ(1u, iface.GetOpenFilenames(EditorInterface::ViewTarget::primary).size ());
}

