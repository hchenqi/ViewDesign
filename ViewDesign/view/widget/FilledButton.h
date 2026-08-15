#pragma once

#include "ViewDesign/view/wrapper/Button.h"

#include <functional>


namespace ViewDesign {


struct FilledButtonStyle {
	struct BackgroundStyle {
	public:
		Color _normal = ColorCode::White;
		Color _hovered = ColorCode::LightGray;
		Color _pressed = ColorCode::Gray;
	public:
		BackgroundStyle& normal(Color normal) { _normal = normal; return *this; }
		BackgroundStyle& hovered(Color hovered) { _hovered = hovered; return *this; }
		BackgroundStyle& pressed(Color pressed) { _pressed = pressed; return *this; }
	} background;
};


template<view_type View> requires (requires(View view, Color color) { view.SetBackground(color); })
class FilledButton : public Button<View> {
private:
	using Base = Button<View>;

public:
	using Style = FilledButtonStyle;

public:
	FilledButton(Style style, std::function<void()> callback, auto&&... args) : Base(std::forward<decltype(args)>(args)...), style(style), callback(std::move(callback)) {
		Base::SetBackground(style.background._normal);
	}

protected:
	Style style;
	std::function<void()> callback;

protected:
	virtual void OnHover() override { Base::SetBackground(style.background._hovered); }
	virtual void OnPress() override { Base::SetBackground(style.background._pressed); }
	virtual void OnLeave() override { Base::SetBackground(style.background._normal); }
protected:
	virtual void OnClick() override { callback(); }
};


} // namespace ViewDesign
