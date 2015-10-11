#include "gtest/gtest.h"
#include "utils/utf8string.h"
#include <map>

// static const auto koala_utf8 = "\xF0\x9F\x90\xA8";
// static const auto *koala_utf16 = L"\uD83D\uDC28"; // Looks like multi symbol utf-8 is bugged in msvc
static auto* smile_utf8 = "\xE2\x98\xBA"; // http://www.fileformat.info/info/unicode/char/263a/index.htm
static auto* smile_utf16 = L"\u263A";
static auto* complex_phrase_utf8 = "Umbrella - \xe2\x98\x82, Cloud - \xe2\x98\x81, Sun - \xe2\x98\x80, Russian Letter a - \xd0\xb0";
static auto* complex_phrase_utf16 = L"Umbrella - \u2602, Cloud - \u2601, Sun - \u2600, Russian Letter a - \u0430";
static auto* four_char_symbols_utf8 = "\xf0\x90\x82\x80";
static auto* four_char_symbols_utf16 = reinterpret_cast<const wchar_t *>("\x00\xd8\x80\xdc\0");
TEST (utf8string, conversion) {
	std::string s{"abcdefghiklmnoprqrstvuxyz"};
	utf8string t{s};
	// char* -> utf8 simple (equality), can't test more than that because of different encodings
	EXPECT_STREQ(s.c_str(), t.c_str());

	std::vector<std::pair<const wchar_t *, const char *>> conversion_samples = {{smile_utf16, smile_utf8},{complex_phrase_utf16 , complex_phrase_utf8},
		{four_char_symbols_utf16, four_char_symbols_utf8}};
	for (auto& sample : conversion_samples) {
		auto utf8 = utf8string::from_raw_utf8(sample.second);
		for (auto it = utf8.begin(); it != utf8.end(); ++it)
		{ }
		// utf8 -> utf16 conversion
		EXPECT_TRUE(utf8.to_wstring() == sample.first);
		// utf16 -> utf8 conversion back
		EXPECT_EQ(utf8, utf8string(std::wstring{ sample.first }));
	}
}

TEST(utf8string, find) {
	auto u8str = utf8string::from_raw_utf8(complex_phrase_utf8);
	auto cloud = utf8string::from_raw_utf8("\xe2\x98\x82");
	auto it = u8str.find(cloud.begin());
	EXPECT_EQ(std::next (u8str.begin (), 11), it);
	auto sun = utf8string::from_raw_utf8("\xe2\x98\x80");
	it = u8str.find(sun.begin());
	EXPECT_EQ(std::next(u8str.begin(), 31), it);
}

