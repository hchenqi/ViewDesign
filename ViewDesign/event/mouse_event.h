#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/geometry/point.h"


namespace ViewDesign {


struct MouseEvent {
	Point point;
	short wheel_delta = 0;
	enum : uint8 {
		LeftDown,
		LeftUp,
		RightDown,
		RightUp,
		MiddleDown,
		MiddleUp,
		Move,
		WheelVertical,
		WheelHorizontal,
	} type;
	struct {
		bool ctrl : 1;
		bool shift : 1;
		bool alt : 1;
	};
};


} // namespace ViewDesign
