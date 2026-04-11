#pragma once

#include "../geometry/point.h"


namespace ViewDesign {


struct MouseEvent {
	Point point;
	enum : uchar {
		LeftDown,
		LeftUp,
		RightDown,
		RightUp,
		MiddleDown,
		MiddleUp,
		Move,
		WheelVertical,
		WheelHorizontal,
	}type;
	union {
		struct {
			bool left : 1;
			bool right : 1;
			bool shift : 1;
			bool ctrl : 1;
			bool middle : 1;
			bool xbutton1 : 1;
			bool xbutton2 : 1;
		};
		uchar _key_state;
	};
	short wheel_delta;
};


} // namespace ViewDesign
