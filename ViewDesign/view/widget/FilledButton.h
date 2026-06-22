#pragma once

#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Button.h"
#include "ViewDesign/view/wrapper/Background.h"

#include <functional>


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class FilledButton : public Button<DefaultBackground<Placeholder<WidthTrait, HeightTrait>>> {
public:
	using Base = Button<DefaultBackground<Placeholder<WidthTrait, HeightTrait>>>;

public:
	struct Style {
		struct BackgroundStyle {
		public:
			Color _normal = color_transparent;
			Color _hovered = ColorCode::Gray;
			Color _pressed = ColorCode::DimGray;
		public:
			BackgroundStyle& normal(Color normal) { _normal = normal; return *this; }
			BackgroundStyle& hovered(Color hovered) { _hovered = hovered; return *this; }
			BackgroundStyle& pressed(Color pressed) { _pressed = pressed; return *this; }
		} background;
	};

public:
	FilledButton(Style style, std::function<void()> callback, auto&&... args) : Base(std::forward<decltype(args)>(args)...), style(style), callback(std::move(callback)) {
		Base::background = style.background._normal;
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
