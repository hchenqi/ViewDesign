#include "ViewDesign/common/word_iterator.h"

#include <cassert>
#include <stdexcept>

#if defined(VIEWDESIGN_ICU) || defined(VIEWDESIGN_WIN32)

#if defined(VIEWDESIGN_ICU)
#include <unicode/ubrk.h>
#else
#include <icu.h>
#endif


namespace ViewDesign {

namespace {

UErrorCode status = U_ZERO_ERROR;

inline UBreakIterator* AsUBreakIterator(void* iter) { return static_cast<UBreakIterator*>(iter); }

} // namespace


WordIterator::WordIterator() {
	iter = ubrk_open(UBRK_WORD, nullptr, nullptr, 0, &status); assert(U_SUCCESS(status));
	str = nullptr; length = 0;
	begin = end = UBRK_DONE;
}

WordIterator::~WordIterator() {
	ubrk_close(AsUBreakIterator(iter));
}

void WordIterator::SetText(const u16string& str) {
	ubrk_setText(AsUBreakIterator(iter), str.c_str(), (int)str.length(), &status); assert(U_SUCCESS(status));
	this->str = str.c_str(); this->length = str.length();
	begin = end = UBRK_DONE;
}

TextRange WordIterator::Seek(size_t pos) {
	if (str == nullptr || pos >= length) { throw std::invalid_argument("invalid text position"); }
	begin = ubrk_preceding(AsUBreakIterator(iter), (int)(pos + 1)); assert(begin != UBRK_DONE);
	end = ubrk_next(AsUBreakIterator(iter)); assert(end != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}

TextRange WordIterator::Next() {
	if (str == nullptr || begin == -1) { throw std::invalid_argument("invalid text position"); }
	if (end >= length) { throw std::invalid_argument("cannot increment iterator past end"); }
	begin = end;
	end = ubrk_next(AsUBreakIterator(iter)); assert(end != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}

TextRange WordIterator::Prev() {
	if (str == nullptr || begin == -1) { throw std::invalid_argument("invalid text position"); }
	if (begin == 0) { throw std::invalid_argument("cannot decrement iterator before begin"); }
	end = begin;
	begin = ubrk_preceding(AsUBreakIterator(iter), (int)begin); assert(begin != UBRK_DONE);
	assert(end > begin);
	return TextRange(begin, end - begin);
}


} // namespace ViewDesign

#else

namespace ViewDesign {


WordIterator::WordIterator() : iter(nullptr), str(nullptr), length(0), begin(0), end(0) {}

WordIterator::~WordIterator() {}

void WordIterator::SetText(const u16string& str) {
	this->str = str.c_str(); this->length = str.length();
	begin = end = 0;
}

TextRange WordIterator::Seek(size_t pos) {
	if (str == nullptr || pos >= length) {
		throw std::invalid_argument("invalid text position");
	}
	switch (GetU16UnitType(str[pos])) {
	case U16UnitType::SurrogateHigh:
		if (pos + 1 >= length || GetU16UnitType(str[pos + 1]) != U16UnitType::SurrogateLow) {
			throw std::invalid_argument("invalid text position");
		}
		begin = pos; end = pos + 2;
		break;
	case U16UnitType::SurrogateLow:
		if (pos == 0 || GetU16UnitType(str[pos - 1]) != U16UnitType::SurrogateHigh) {
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

TextRange WordIterator::Next() {
	if (str == nullptr) { throw std::invalid_argument("invalid text position"); }
	if (end >= length) { throw std::invalid_argument("cannot increment iterator past end"); }
	return Seek(end);
}

TextRange WordIterator::Prev() {
	if (str == nullptr) { throw std::invalid_argument("invalid text position"); }
	if (begin == 0) { throw std::invalid_argument("cannot decrement iterator before begin"); }
	return Seek(begin - 1);
}


} // namespace ViewDesign

#endif
