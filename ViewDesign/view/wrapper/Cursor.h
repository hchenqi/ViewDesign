#pragma once

#include "ViewDesign/view/ViewBase.h"


namespace ViewDesign {


template<class View, CursorStyle cursor = CursorStyle::Arrow> requires std::derived_from<View, ViewBase>
class CustomizedCursor : public View {
protected:
	using Base = CustomizedCursor;

public:
	CustomizedCursor(auto&&... args) : View(std::forward<decltype(args)>(args)...) {
		this->cursor = GetCursor(cursor);
	}
};


} // namespace ViewDesign
