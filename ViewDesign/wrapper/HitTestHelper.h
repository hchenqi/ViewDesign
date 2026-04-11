#pragma once

#include "../frame/ViewFrame.h"


namespace ViewDesign {


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitSelf : public View {
protected:
	using Base = HitSelf;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return this; }
};


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitSelfFallback : public View {
protected:
	using Base = HitSelfFallback;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<ViewBase> view = View::HitTest(event)) {
			return view;
		}
		event = event_copy;
		return this;
	}
};


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitSelfFallbackNext : public View {
protected:
	using Base = HitSelfFallbackNext;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<ViewBase> view = View::HitTest(event)) {
			if (view == this) {
				return this;
			} else {
				if (view = ViewBase::HitTestChild(*view, event)) {
					return view;
				}
			}
		}
		event = event_copy;
		return this;
	}
};


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitSelfFallbackRecursive : public View {
protected:
	using Base = HitSelfFallbackRecursive;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<ViewBase> view = View::HitTest(event)) {
			if (view == this) {
				return this;
			} else {
				for (;;) {
					ref_ptr<ViewBase> next = ViewBase::HitTestChild(*view, event);
					if (next == nullptr) {
						break;
					} else if (next == view) {
						return view;
					} else {
						view = next;
					}
				}
			}
		}
		event = event_copy;
		return this;
	}
};


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitThrough : public View {
protected:
	using Base = HitThrough;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return nullptr; }
};


template<class View> requires std::is_base_of_v<ViewBase, View>
class HitThroughFallback : public View {
protected:
	using Base = HitThroughFallback;

public:
	using View::View;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (ref_ptr<ViewBase> view = View::HitTest(event); view != this) {
			return view;
		}
		return nullptr;
	}
};


template<class Frame> requires std::is_base_of_v<ViewFrame, Frame>
class HitThroughMargin : public Frame {
protected:
	using Base = HitThroughMargin;

public:
	using Frame::Frame;

protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (Frame::GetChildRegion().Contains(event.point)) {
			event.point -= Frame::GetChildOffset();
			return ViewFrame::HitTest(event);
		}
		return nullptr;
	}
};


} // namespace ViewDesign
