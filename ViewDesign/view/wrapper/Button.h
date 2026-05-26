#pragma once

#include "ViewDesign/view/wrapper/HitTestHelper.h"


namespace ViewDesign {


template<view_type View>
class Button : public HitSelf<View> {
protected:
	using Base = Button;

public:
	using HitSelf<View>::HitSelf;

	// state transition:
	// - State::Normal (initial)
	//   - FocusEvent::MouseEnter -> State::Hovered  OnHover()
	// - State::Hovered
	//   - FocusEvent::MouseLeave -> State::Normal   OnLeave()
	//   - MouseEvent::LeftDown   -> State::Pressed  OnPress()
	// - State::Pressed
	//   - FocusEvent::MouseLeave -> State::Normal   OnLeave()
	//   - MouseEvent::LeftUp     -> State::Hovered  OnHover(), OnClick()
protected:
	enum class State { Normal, Hovered, Pressed } state = State::Normal;
protected:
	virtual void OnHover() {}
	virtual void OnPress() {}
	virtual void OnLeave() {}
	virtual void OnClick() {}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: if (state == State::Hovered) { state = State::Pressed; OnPress(); } break;
		case MouseEvent::LeftUp: if (state == State::Pressed) { state = State::Hovered; OnHover(); OnClick(); } break;
		}
	}
	virtual void OnFocusEvent(FocusEvent event) override {
		switch (event) {
		case FocusEvent::MouseEnter: if (state == State::Normal) { state = State::Hovered; OnHover(); } break;
		case FocusEvent::MouseLeave: if (state == State::Hovered || state == State::Pressed) { state = State::Normal; OnLeave(); } break;
		}
	}
};


} // namespace ViewDesign
