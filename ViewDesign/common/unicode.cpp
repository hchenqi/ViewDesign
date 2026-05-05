#include "ViewDesign/common/unicode.h"

#if defined(VIEWDESIGN_PLATFORM_ICU)

#include <unicode/unistr.h>


namespace ViewDesign {


u8string to_u8string(const u16string& str) {
	return icu::UnicodeString::readOnlyAlias(str).toUTF8String<u8string>();
}

u16string to_u16string(const u8string& str) {
	icu::UnicodeString s = icu::UnicodeString::fromUTF8(str);
	return u16string(s.getBuffer(), s.length());
}


} // namespace ViewDesign

#endif
