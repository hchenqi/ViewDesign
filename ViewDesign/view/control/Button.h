#pragma once

#include "Placeholder.h"
#include "../wrapper/Background.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class Button : public SolidColorBackground<Placeholder<WidthTrait, HeightTrait>> {
public:
	using SolidColorBackground<Placeholder<WidthTrait, HeightTrait>>::SolidColorBackground;

	// style
protected:
	Color background_normal = this->background = Color::DarkGray;
	Color background_hover = Color::Gray;
	Color background_press = Color::DimGray;

	// paint
private:
	void SetBackgroundColor(Color color) { if (this->background != color) { this->background = color; this->Redraw(region_infinite); } }

	// event
private:
	enum class State { Normal, Hover, Press } state = State::Normal;
private:
	void OnHover() { SetBackgroundColor(background_hover); }
	void OnPress() { SetBackgroundColor(background_press); }
	void OnLeave() { SetBackgroundColor(background_normal); }
protected:
	virtual void OnClick() {}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: state = State::Press; OnPress(); break;
		case MouseEvent::LeftUp: if (state == State::Press) { state = State::Hover; OnHover(); OnClick(); } break;
		}
	}
	virtual void OnFocusEvent(FocusEvent event) override {
		switch (event) {
		case FocusEvent::MouseEnter: if (state == State::Normal) { state = State::Hover; OnHover(); } break;
		case FocusEvent::MouseLeave: state = State::Normal; OnLeave(); break;
		}
	}
};


} // namespace ViewDesign
