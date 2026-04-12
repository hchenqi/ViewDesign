#pragma once

#include "uncopyable.h"
#include "../geometry/text_range.h"

#include <string>


namespace ViewDesign {


enum class UTF16CharType : uchar {
	Single,
	SurrogateLow,
	SurrogateHigh,
};

constexpr UTF16CharType GetUTF16CharType(wchar ch) {
	if (ch >= L'\xD800' && ch <= L'\xDBFF') { return UTF16CharType::SurrogateLow; }
	if (ch >= L'\xDC00' && ch <= L'\xDFFF') { return UTF16CharType::SurrogateHigh; }
	return UTF16CharType::Single;
}

constexpr size_t GetUTF16CharLength(wchar ch) {
	switch (GetUTF16CharType(ch)) {
	case UTF16CharType::Single: return 1;
	case UTF16CharType::SurrogateLow:
	case UTF16CharType::SurrogateHigh: return 2;
	default: return 0;
	}
}


class WordBreakIterator : Uncopyable {
private:
	alloc_ptr<void> iter;
	ref_ptr<const wchar> str; size_t length;
	size_t begin, end;
public:
	WordBreakIterator();
	~WordBreakIterator();
public:
	void SetText(const std::wstring& str);
	TextRange Seek(size_t pos);
	TextRange Next();
	TextRange Prev();
};


} // namespace ViewDesign
