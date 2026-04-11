#pragma once

#include "../common/core.h"


namespace ViewDesign {


enum class NotifyMsg : uchar {
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
