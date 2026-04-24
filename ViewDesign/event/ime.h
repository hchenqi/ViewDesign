#pragma once

#include "ViewDesign/geometry/point.h"
#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

class ViewBase;


struct Ime {
	static void Enable(ViewBase& view);
	static void Disable(ViewBase& view);
	static void SetPosition(ViewBase& view, Point point);

	static u16string GetString();
	static size_t GetCursorPosition();
};

constexpr Ime ime;


} // namespace ViewDesign
