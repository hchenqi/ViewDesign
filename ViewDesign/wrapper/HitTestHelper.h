#pragma once

#include "../frame/WndFrame.h"


namespace ViewDesign {


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitSelf : public Wnd {
protected:
	using Base = HitSelf;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override { return this; }
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitSelfFallback : public Wnd {
protected:
	using Base = HitSelfFallback;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(event)) {
			return wnd;
		}
		event = event_copy;
		return this;
	}
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitSelfFallbackNext : public Wnd {
protected:
	using Base = HitSelfFallbackNext;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(event)) {
			if (wnd == this) {
				return this;
			} else {
				if (wnd = WndObject::HitTestChild(*wnd, event)) {
					return wnd;
				}
			}
		}
		event = event_copy;
		return this;
	}
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitSelfFallbackRecursive : public Wnd {
protected:
	using Base = HitSelfFallbackRecursive;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(event)) {
			if (wnd == this) {
				return this;
			} else {
				for (;;) {
					ref_ptr<WndObject> next = WndObject::HitTestChild(*wnd, event);
					if (next == nullptr) {
						break;
					} else if (next == wnd) {
						return wnd;
					} else {
						wnd = next;
					}
				}
			}
		}
		event = event_copy;
		return this;
	}
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitThrough : public Wnd {
protected:
	using Base = HitThrough;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override { return nullptr; }
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitThroughFallback : public Wnd {
protected:
	using Base = HitThroughFallback;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(event); wnd != this) {
			return wnd;
		}
		return nullptr;
	}
};


template<class Frame> requires std::is_base_of_v<WndFrame, Frame>
class HitThroughMargin : public Frame {
protected:
	using Base = HitThroughMargin;

public:
	using Frame::Frame;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		if (Frame::GetChildRegion().Contains(event.point)) {
			event.point -= Frame::GetChildOffset();
			return WndFrame::HitTest(event);
		}
		return nullptr;
	}
};


} // namespace ViewDesign
