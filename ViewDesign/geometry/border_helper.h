#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/style/cursor_style.h"


namespace ViewDesign {


enum class BorderPosition : uint {
	None = 0,
	Left = 10,			// HTLEFT
	Top = 12,			// HTTOP
	Right = 11,			// HTRIGHT
	Bottom = 15,		// HTBOTTOM
	LeftTop = 13,		// HTTOPLEFT
	RightTop = 14,		// HTTOPRIGHT
	LeftBottom = 16,	// HTBOTTOMLEFT
	RightBottom = 17	// HTBOTTOMRIGHT
};

inline BorderPosition HitTestBorderPosition(Size size, float border_width, Point point) {
	float x1 = border_width, y1 = border_width;
	float x2 = size.width - border_width, y2 = size.height - border_width;
	float x = point.x, y = point.y;
	if (x < x1) {
		if (y < y1) {
			return BorderPosition::LeftTop;
		} else if (y < y2) {
			return BorderPosition::Left;
		} else {
			return BorderPosition::LeftBottom;
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
			return BorderPosition::RightTop;
		} else if (y < y2) {
			return BorderPosition::Right;
		} else {
			return BorderPosition::RightBottom;
		}
	}
}

inline CursorStyle GetBorderPositionCursorStyle(BorderPosition border_position) {
	switch (border_position) {
	case BorderPosition::Left: return CursorStyle::ResizeWE;
	case BorderPosition::Top: return CursorStyle::ResizeNS;
	case BorderPosition::Right: return CursorStyle::ResizeWE;
	case BorderPosition::Bottom: return CursorStyle::ResizeNS;
	case BorderPosition::LeftTop: return CursorStyle::ResizeNWSE;
	case BorderPosition::RightTop: return CursorStyle::ResizeNESW;
	case BorderPosition::LeftBottom: return CursorStyle::ResizeNESW;
	case BorderPosition::RightBottom: return CursorStyle::ResizeNWSE;
	default: return CursorStyle::NoChange;
	}
}


} // namespace ViewDesign
