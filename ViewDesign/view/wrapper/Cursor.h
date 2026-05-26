#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {


template<CursorStyle cursor, view_type View>
class CustomizedCursor : public View {
protected:
	using Base = CustomizedCursor;

public:
	CustomizedCursor(auto&&... args) : View(std::forward<decltype(args)>(args)...) {
		this->cursor = GetCursor(cursor);
	}
};


} // namespace ViewDesign
