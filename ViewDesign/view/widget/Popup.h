#pragma once

#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


class Popup : public Window {
public:
	using child_type = view_ptr<Bounded, Bounded>;

public:
	Popup(Point point, child_type child);

protected:
	Point point;

protected:
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override {
		return Clamp(Rect(point, UpdateChildSizeRef(*child, size_ref)), Rect(point_zero, size_ref));
	}

protected:
	virtual void OnFocusEvent(FocusEvent event) override {
		switch (event) {
		case FocusEvent::Blur: Close(); break;
		}
	}
};


inline void ShowPopup(ViewBase& view, Point point, view_ptr<Bounded, Bounded> content) {
	desktop.AddWindow(
		new Popup(
			desktop.ConvertDescendentPoint(view, point),
			std::move(content)
		)
	).SetFocus();
}


} // namespace ViewDesign
