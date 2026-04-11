#pragma once

#include "../frame/ViewFrame.h"

#include <list>


namespace ViewDesign {


class OverlapFrame : protected ViewFrame {
private:
	friend class OverlapLayout;

public:
	OverlapFrame(view_ptr<Relative, Relative> child) : ViewFrame(std::move(child)) {}
	~OverlapFrame() {}

	// parent
protected:
	OverlapLayout& GetParent() const;
protected:
	void BringForward();
	void BringToFront();
	void SendBackward();
	void SendToBack();

	// layout
private:
	Rect region;  // only accessed by OverlapLayout
protected:
	void OverlapFrameRegionUpdated(Rect region);
protected:
	virtual Rect OnOverlapFrameSizeRefUpdate(Size size_ref) { UpdateChildSizeRef(child, size_ref); return Rect(point_zero, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}
private:
	void SizeUpdated() {}  // never used
	virtual Size OnSizeRefUpdate(Size size_ref) override final { return size_ref; }  // never used
};


class OverlapLayout : public ViewType<Assigned, Assigned> {
private:
	friend class OverlapFrame;

public:
	using frame_ptr = std::unique_ptr<OverlapFrame>;

public:
	OverlapLayout() {}
	~OverlapLayout() {}

	// child
protected:
	std::list<frame_ptr> frame_list;
protected:
	static OverlapFrame& AsFrame(ViewBase& child) { return static_cast<OverlapFrame&>(child); }
public:
	void AddChild(frame_ptr frame);
	void AddChild(alloc_ptr<OverlapFrame> frame) { AddChild(frame_ptr(frame)); }
	frame_ptr RemoveChild(OverlapFrame& frame);
public:
	void BringForward(OverlapFrame& frame);
	void BringToFront(OverlapFrame& frame);
	void SendBackward(OverlapFrame& frame);
	void SendToBack(OverlapFrame& frame);

	// layout
protected:
	Size size = size_empty;
protected:
	void UpdateOverlapFrameChildSizeRef(OverlapFrame& frame, Size size_ref) { VerifyChild(frame); frame.region = frame.OnOverlapFrameSizeRefUpdate(size_ref); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;
	virtual void OnOverlapFrameChildRegionUpdate(ViewBase& child, Rect child_region);
private:
	void UpdateChildSizeRef() {}  // never used
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override final {}  // never used

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override;
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override;
};


inline OverlapLayout& OverlapFrame::GetParent() const { return static_cast<OverlapLayout&>(ViewBase::GetParent()); }
inline void OverlapFrame::BringForward() { GetParent().BringForward(*this); }
inline void OverlapFrame::BringToFront() { GetParent().BringToFront(*this); }
inline void OverlapFrame::SendBackward() { GetParent().SendBackward(*this); }
inline void OverlapFrame::SendToBack() { GetParent().SendToBack(*this); }
inline void OverlapFrame::OverlapFrameRegionUpdated(Rect region) { if (HasParent()) { GetParent().OnOverlapFrameChildRegionUpdate(*this, region); } }


} // namespace ViewDesign
