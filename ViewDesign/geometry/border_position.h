#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/style/cursor_style.h"


namespace ViewDesign {


enum class BorderPosition {
	None,
	Left,
	Top,
	Right,
	Bottom,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	_Count,
};

inline BorderPosition HitTestBorderPosition(Size size, float border_width, Point point) {
	float x1 = border_width, y1 = border_width;
	float x2 = size.width - border_width, y2 = size.height - border_width;
	float x = point.x, y = point.y;
	if (x < x1) {
		if (y < y1) {
			return BorderPosition::TopLeft;
		} else if (y < y2) {
			return BorderPosition::Left;
		} else {
			return BorderPosition::BottomLeft;
		}
	} else if (x < x2) {
		if (y < y1) {
			return BorderPosition::Top;
		} else if (y < y2) {
			return BorderPosition::None;
		} else {
			return BorderPosition::Bottom;
		}
	} else {
		if (y < y1) {
			return BorderPosition::TopRight;
		} else if (y < y2) {
			return BorderPosition::Right;
		} else {
			return BorderPosition::BottomRight;
		}
	}
}

inline CursorStyle GetResizeCursorStyle(BorderPosition border_position) {
	switch (border_position) {
	case BorderPosition::Left: return CursorStyle::ResizeWE;
	case BorderPosition::Top: return CursorStyle::ResizeNS;
	case BorderPosition::Right: return CursorStyle::ResizeWE;
	case BorderPosition::Bottom: return CursorStyle::ResizeNS;
	case BorderPosition::TopLeft: return CursorStyle::ResizeNWSE;
	case BorderPosition::TopRight: return CursorStyle::ResizeNESW;
	case BorderPosition::BottomLeft: return CursorStyle::ResizeNESW;
	case BorderPosition::BottomRight: return CursorStyle::ResizeNWSE;
	default: return CursorStyle::NoChange;
	}
}


} // namespace ViewDesign
