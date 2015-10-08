#include "utf8string.h"
#include "iconv.h"

inline bool utf8_is_lead(char c) {
	return (((c & 0x80) == 0) // 0xxxxxxx
		|| ((c & 0xC0) == 0xC0 && (c & 0x20) == 0) // 110xxxxx
		|| ((c & 0xE0) == 0xE0 && (c & 0x10) == 0) // 1110xxxx
		|| ((c & 0xF0) == 0xF0 && (c & 0x08) == 0) // 11110xxx
		|| ((c & 0xF8) == 0xF8 && (c & 0x04) == 0) // 111110xx
		|| ((c & 0xFC) == 0xFC && (c & 0x02) == 0));
}


inline const char* utf8_inc(const char* string) {
	const char* temp;
	temp = string + 1;
	while (*temp && !utf8_is_lead(*temp))
		temp++;

	return temp;
}

inline size_t utf8_len(const char* String) {
	const char* It = String;
	size_t Size = 0;
	while (*It) {
		Size++;
		It = utf8_inc(It);
	}
	return Size;
}

int utf8_char_len(char c) {
	if ((c & 0x80) == 0)
		return 1;
	else if ((c & 0xC0) > 0 && (c & 0x20) == 0)
		return 2;
	else if ((c & 0xE0) > 0 && (c & 0x10) == 0)
		return 3;
	else if ((c & 0xF0) > 0 && (c & 0x08) == 0)
		return 4;
	else if ((c & 0xF8) > 0 && (c & 0x04) == 0)
		return 5;
	else if ((c & 0xFC) > 0 && (c & 0x02) == 0)
		return 6;
	return 0;
}

bool utf8_are_first_chars_equal(const char* Str1, const char* Str2) {
	int FirstCharSize1 = utf8_char_len(*Str1);
	int FirstCharSize2 = utf8_char_len(*Str2);
	if (FirstCharSize1 != FirstCharSize2)
		return FALSE;
	return (strncmp(Str1, Str2, FirstCharSize1) == 0);
}


const std::string utf8_encoding = "UTF-8";
const std::string char_encoding = "CHAR";
const std::string wchar_t_encoding = "UTF-16LE";
const std::string ignore_tag = "//IGNORE";

std::wstring utf8string::to_wstring() const {
	iconv_t converter = iconv_open((wchar_t_encoding + ignore_tag).c_str(), utf8_encoding.c_str());
	size_t inSize = m_str.size() + 1;
	size_t outSize = (utf8_len(m_str.c_str()) + 1) * 2 * sizeof (wchar_t);
	std::vector<wchar_t> outBuf(outSize);
	char* outBufPtr = reinterpret_cast<char *>(data(outBuf));
	const char* inBufPtr = m_str.c_str();
	size_t res = iconv(converter, &inBufPtr, &inSize, &outBufPtr, &outSize);
	iconv_close(converter);

	if (res == (size_t) (-1)) {
		return std::wstring();
	}

	return std::wstring(outBuf.data()); // here goes additional copy
}

utf8string::utf8string(const char* raw_utf8): m_str{raw_utf8} {
}

utf8string::iterator::self& utf8string::iterator::operator++() {
	base_it += utf8_char_len(*base_it);
	return *this;
}

utf8string::iterator::self& utf8string::iterator::operator--() {
	auto cur_it = std::prev(base_it);
	while (!utf8_is_lead(*cur_it)) --cur_it;
	base_it = cur_it;
	return *this;
}

utf8string::iterator::iterator(base_t base_it_arg): base_it{base_it_arg} {
	// assert(utf8_is_lead(*base_it)); // Impossible with ITERATOR_DEBUG_LEVEL >= 2 due to check because of end ()
}

utf8string::utf8string(iterator start, iterator finish) : utf8string{std::string(start.base_it, finish.base_it)} {
}

struct utf8string::iterator utf8string::find(struct iterator char_ref) {
	for (auto it = begin(); it != end(); ++it) {
		if (std::equal(it.base_it, std::next(it).base_it, char_ref.base_it, std::next(char_ref).base_it))
			return it;
	}
	return end();
}

template <typename CHAR>
std::string get_encoding();

template <>
std::string get_encoding<wchar_t>() {
	return wchar_t_encoding;
}

template <>
std::string get_encoding<char>() {
	return char_encoding;
}

template <typename CHAR>
utf8string::utf8string(const std::basic_string<CHAR>& strArg) {
	std::string target_enc = utf8_encoding + ignore_tag;
	std::string source_enc = get_encoding<CHAR>().c_str();
	iconv_t converter = iconv_open(target_enc.c_str(), source_enc.c_str());
	size_t inSize = (strArg.length() + 1) * sizeof (CHAR);
	size_t outSize = 6 * strArg.length() + 1; // Maximum Possible UTF-8 size
	std::vector<char> outBuf(outSize);
	char* outBufPtr = outBuf.data();
	const char* inBufPtr = reinterpret_cast<const char *>(strArg.data());
	size_t res = iconv(converter, &inBufPtr, &inSize, &outBufPtr, &outSize);
	iconv_close(converter);
	if (res == static_cast<size_t>(-1))
		return;

	m_str.assign(outBuf.data());
}

template utf8string::utf8string(const std::wstring& strArg);
template utf8string::utf8string(const std::string& strArg);

