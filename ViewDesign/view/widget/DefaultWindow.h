#pragma once

#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/style/length_style_helper.h"


namespace ViewDesign {


class DefaultWindow : public Window {
public:
	using child_type = view_ptr<Fixed, Fixed>;

public:
	struct Style {
		LengthStyle width;
		LengthStyle height;
		PositionStyle position;

		Style() {
			width.normal(px(800)).min(px(200)).max(pct(100));
			height.normal(px(500)).min(px(200)).max(pct(100));
			position.setHorizontalCenter().setVerticalCenter();
		}
	};

public:
	DefaultWindow(const Style& style, const u16string& title, child_type child) : Window(title, std::move(child)), style(style) {}

	// style
protected:
	Style style;

	// layout
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override {
		return LengthStyleHelper::CalculateMinMaxSize(style.width, style.height, size_ref);
	}
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override {
		Rect region = LengthStyleHelper::CalculateRegion(style.width, style.height, style.position, size_ref);
		UpdateChildSizeRef(child, region.size);
		return region;
	}
};


} // namespace ViewDesign
