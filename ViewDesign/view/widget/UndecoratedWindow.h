#pragma once

#include "ViewDesign/view/widget/DefaultWindow.h"


namespace ViewDesign {


class UndecoratedWindow : public DefaultWindow {
public:
	using child_type = DefaultWindow::child_type;

public:
	using Style = DefaultWindow::Style;

public:
	UndecoratedWindow(const u16string& title, const Style& style, child_type child);
};


} // namespace ViewDesign
