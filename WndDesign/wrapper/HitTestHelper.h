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
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override { return this; }
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitSelfFallback : public Wnd {
protected:
	using Base = HitSelfFallback;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		MouseMsg msg_copy = msg;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(msg)) {
			return wnd;
		}
		msg = msg_copy;
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
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		MouseMsg msg_copy = msg;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(msg)) {
			if (wnd == this) {
				return this;
			} else {
				if (wnd = WndObject::HitTestChild(*wnd, msg)) {
					return wnd;
				}
			}
		}
		msg = msg_copy;
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
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		MouseMsg msg_copy = msg;
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(msg)) {
			if (wnd == this) {
				return this;
			} else {
				for (;;) {
					ref_ptr<WndObject> next = WndObject::HitTestChild(*wnd, msg);
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
		msg = msg_copy;
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
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override { return nullptr; }
};


template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
class HitThroughFallback : public Wnd {
protected:
	using Base = HitThroughFallback;

public:
	using Wnd::Wnd;

protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		if (ref_ptr<WndObject> wnd = Wnd::HitTest(msg); wnd != this) {
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
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		if (Frame::GetChildRegion().Contains(msg.point)) {
			msg.point -= Frame::GetChildOffset();
			return WndFrame::HitTest(msg);
		}
		return nullptr;
	}
};


} // namespace ViewDesign
