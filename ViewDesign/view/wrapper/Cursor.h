#pragma once

#include "../ViewBase.h"


namespace ViewDesign {


template<class View, Cursor cursor = Cursor::Arrow> requires std::is_base_of_v<ViewBase, View>
class CustomizedCursor : public View {
protected:
	using Base = CustomizedCursor;

public:
	CustomizedCursor(auto&&... args) : View(std::forward<decltype(args)>(args)...) {
		this->cursor = cursor;
	}
};


} // namespace ViewDesign
