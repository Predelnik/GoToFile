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
		m_entities.emplace_back(L"a.txt", EntityType::file);
		m_entities.emplace_back(L"b.txt", EntityType::file);
		m_entities.emplace_back(L"dir_a", EntityType::dir);
	}

	~FiletructureKeeper() {
	}

private:
	std::vector<EntityKeeper> m_entities;
};

TEST (GoToFile, basic) {
	FiletructureKeeper keep;
	TestEditorInterface iface;
	Settings settings;
	GotoFileManager manager{&iface, &settings};
	using vt = EditorInterface::ViewType;

	auto cur_file = [&] {
		return fs::path(iface.ActiveDocumentPath()).filename();
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
	iface.SetSelection (0)

	for (auto enc : enum_range<EditorInterface::Codepage>())
	{
		iface.SetViewCodepage(EditorInterface::ViewType::primary, enc);

		iface.SetString(utf8string::from_raw_utf8("b.txt"));
		CHECK;

		iface.SetString(utf8string::from_raw_utf8(R"(dir_a\../b.txt)")); // twisted path
		CHECK;

		iface.SetString(utf8string{ fs::absolute(L"b.txt").wstring() }); // absolute path
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

	settings.largeFileSizeLimit = 0;
	iface.SetString(utf8string::from_raw_utf8("b.txt")); // running external program check
	manager.GoToFile();
	EXPECT_EQ(L"a.txt", cur_file());
}

