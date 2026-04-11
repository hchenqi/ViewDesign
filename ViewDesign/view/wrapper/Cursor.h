#pragma once

#include "../ViewBase.h"


namespace ViewDesign {


template<class View, Cursor cursor = Cursor::Arrow> requires std::is_base_of_v<ViewBase, View>
class CustomizedCursor : public View {
protected:
	using Base = CustomizedCursor;

public:
	template <typename... Args>
	CustomizedCursor(Args&&... args) : View(std::forward<Args>(args)...) {
		this->cursor = cursor;
	}
};


} // namespace ViewDesign
