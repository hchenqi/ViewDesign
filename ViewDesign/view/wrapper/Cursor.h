#pragma once

#include "ViewDesign/view/ViewBase.h"


namespace ViewDesign {


template<class View, Cursor cursor = Cursor::Arrow> requires std::derived_from<View, ViewBase>
class CustomizedCursor : public View {
protected:
	using Base = CustomizedCursor;

public:
	CustomizedCursor(auto&&... args) : View(std::forward<decltype(args)>(args)...) {
		this->cursor = cursor;
	}
};


} // namespace ViewDesign
