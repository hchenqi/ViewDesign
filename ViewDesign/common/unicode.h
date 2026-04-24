#pragma once

#include <string>


namespace ViewDesign {


using u16char = char16_t;
using u16string = std::u16string;


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


using u8char = char8_t;
using u8string = std::u8string;


} // namespace ViewDesign
