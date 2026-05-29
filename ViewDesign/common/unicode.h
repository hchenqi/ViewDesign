#pragma once

#include <string>


namespace ViewDesign {


using u8char = char8_t;
using u16char = char16_t;
using u32char = char32_t;

using u8string = std::u8string;
using u16string = std::u16string;
using u32string = std::u32string;


struct u16pair : std::pair<u16char, u16char> {
	u16pair() {}
	u16pair(u16char first, u16char second) : std::pair<u16char, u16char>(first, second) {}
	bool empty() const { return first == 0; }
	bool single() const { return first != 0 && second == 0; }
	bool pair() const { return first != 0 && second != 0; }
	size_t length() const { return first == 0 ? 0 : second == 0 ? 1 : 2; }
};

inline u16pair to_u16pair(u32char ch) {
	if (ch <= 0xFFFF) {
		if (ch >= 0xD800 && ch <= 0xDFFF) {
			return { 0, 0 };
		} else {
			return { static_cast<u16char>(ch), 0 };
		}
	} else if (ch <= 0x10FFFF) {
		ch -= 0x10000;
		return { static_cast<u16char>(0xD800 + (ch >> 10)), static_cast<u16char>(0xDC00 + (ch & 0x3FF)) };
	} else {
		return { 0, 0 };
	}
}


enum class U16UnitType {
	Single,
	SurrogateHigh,
	SurrogateLow,
};

constexpr U16UnitType GetU16UnitType(u16char ch) {
	if (ch >= u'\xD800' && ch <= u'\xDBFF') { return U16UnitType::SurrogateHigh; }
	if (ch >= u'\xDC00' && ch <= u'\xDFFF') { return U16UnitType::SurrogateLow; }
	return U16UnitType::Single;
}


static_assert(sizeof(u8char) == sizeof(char));

inline char* as_char_str(u8char* str) { return reinterpret_cast<char*>(str); }
inline const char* as_char_str(const u8char* str) { return reinterpret_cast<const char*>(str); }

inline u8char* as_u8char_str(char* str) { return reinterpret_cast<u8char*>(str); }
inline const u8char* as_u8char_str(const char* str) { return reinterpret_cast<const u8char*>(str); }


template<class u16char> wchar_t* as_wchar_str(u16char* str) requires (sizeof(u16char) == sizeof(wchar_t)) { return reinterpret_cast<wchar_t*>(str); }
template<class u16char> const wchar_t* as_wchar_str(const u16char* str) requires (sizeof(u16char) == sizeof(wchar_t)) { return reinterpret_cast<const wchar_t*>(str); }


u8string to_u8string(const u16string& str);
u16string to_u16string(const u8string& str);


} // namespace ViewDesign
