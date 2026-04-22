#include "ViewDesign/common/unicode_helper.h"


#if __has_include(<icu.h>) || __has_include(<unicode/ubrk.h>)

#if __has_include(<icu.h>)
#include <icu.h>
#else
#include <unicode/ubrk.h>
#endif

#include <cassert>


namespace ViewDesign {

namespace {

UErrorCode status = U_ZERO_ERROR;

inline UBreakIterator* AsUBreakIterator(void* iter) { return static_cast<UBreakIterator*>(iter); }

} // namespace

static_assert(sizeof(wchar_t) == sizeof(UChar));


WordBreakIterator::WordBreakIterator() {
	iter = ubrk_open(UBRK_WORD, nullptr, nullptr, 0, &status); assert(U_SUCCESS(status));
	str = nullptr; length = 0;
	begin = end = UBRK_DONE;
}

WordBreakIterator::~WordBreakIterator() {
	ubrk_close(AsUBreakIterator(iter));
}

void WordBreakIterator::SetText(const std::wstring& str) {
	ubrk_setText(AsUBreakIterator(iter), (const UChar*)str.c_str(), (uint)str.length(), &status); assert(U_SUCCESS(status));
	this->str = str.c_str(); this->length = str.length();
	begin = end = UBRK_DONE;
}

TextRange WordBreakIterator::Seek(size_t pos) {
	if (str == nullptr || pos >= length) { throw std::invalid_argument("invalid text position"); }
	begin = ubrk_preceding(AsUBreakIterator(iter), (uint)(pos + 1)); assert(begin != UBRK_DONE);
	end = ubrk_next(AsUBreakIterator(iter)); assert(end != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}

TextRange WordBreakIterator::Next() {
	if (str == nullptr || begin == -1) { throw std::invalid_argument("invalid text position"); }
	if (end >= length) { throw std::invalid_argument("cannot increment iterator past end"); }
	begin = end;
	end = ubrk_next(AsUBreakIterator(iter)); assert(end != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}

TextRange WordBreakIterator::Prev() {
	if (str == nullptr || begin == -1) { throw std::invalid_argument("invalid text position"); }
	if (begin == 0) { throw std::invalid_argument("cannot decrement iterator before begin"); }
	end = begin;
	begin = ubrk_preceding(AsUBreakIterator(iter), (uint)begin); assert(begin != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}


} // namespace ViewDesign

#else

namespace ViewDesign {


WordBreakIterator::WordBreakIterator() : iter(nullptr), str(nullptr), length(0), begin(0), end(0) {}

WordBreakIterator::~WordBreakIterator() {}

void WordBreakIterator::SetText(const std::wstring& str) {
	this->str = str.c_str(); this->length = str.length();
	begin = end = 0;
}

TextRange WordBreakIterator::Seek(size_t pos) {
	if (str == nullptr || pos >= length) {
		throw std::invalid_argument("invalid text position");
	}
	switch (GetUTF16CharType(str[pos])) {
	case UTF16CharType::SurrogateHigh:
		if (pos + 1 >= length || GetUTF16CharType(str[pos + 1]) != UTF16CharType::SurrogateLow) {
			throw std::invalid_argument("invalid text position");
		}
		begin = pos; end = pos + 2;
		break;
	case UTF16CharType::SurrogateLow:
		if (pos == 0 || GetUTF16CharType(str[pos - 1]) != UTF16CharType::SurrogateHigh) {
			throw std::invalid_argument("invalid text position");
		}
		begin = pos - 1; end = pos + 1;
		break;
	default:
		begin = pos; end = pos + 1;
		break;
	}
	return TextRange(begin, end - begin);
}

TextRange WordBreakIterator::Next() {
	if (str == nullptr) { throw std::invalid_argument("invalid text position"); }
	if (end >= length) { throw std::invalid_argument("cannot increment iterator past end"); }
	return Seek(end);
}

TextRange WordBreakIterator::Prev() {
	if (str == nullptr) { throw std::invalid_argument("invalid text position"); }
	if (begin == 0) { throw std::invalid_argument("cannot decrement iterator before begin"); }
	return Seek(begin - 1);
}


} // namespace ViewDesign

#endif
