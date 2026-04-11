#pragma once

#include "../common/core.h"


namespace ViewDesign {


enum class FocusEvent : uchar {
	MouseEnter,
	MouseLeave,
	MouseOver,
	MouseOut,
	FocusIn,
	FocusOut,
	Focus,
	Blur,
};


} // namespace ViewDesign
