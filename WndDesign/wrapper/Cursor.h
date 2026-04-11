#pragma once

#include "../window/WndObject.h"


namespace ViewDesign {


template<class Wnd, Cursor cursor = Cursor::Arrow> requires std::is_base_of_v<WndObject, Wnd>
class CustomizedCursor : public Wnd {
protected:
	using Base = CustomizedCursor;

public:
	template <typename... Args>
	CustomizedCursor(Args&&... args) : Wnd(std::forward<Args>(args)...) {
		this->cursor = cursor;
	}
};


} // namespace ViewDesign
