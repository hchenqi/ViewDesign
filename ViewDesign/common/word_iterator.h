#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/common/text_range.h"


namespace ViewDesign {


class WordIterator : Uncopyable {
private:
	owner_ptr<void> iter;
	ref_ptr<const u16char> str;
	size_t length;
	size_t begin, end;
public:
	WordIterator();
	~WordIterator();
public:
	void SetText(const u16string& str);
	TextRange Seek(size_t pos);
	TextRange Next();
	TextRange Prev();
};


} // namespace ViewDesign
