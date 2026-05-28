#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"
#include "ViewDesign/view/control/Placeholder.h"

#include <list>
#include <functional>


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class MirroringFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	MirroringFrame(child_type child) : ViewFrame(std::move(child)) {}
	~MirroringFrame() { for (auto view : view_list) { view.get().frame = nullptr; } }

public:
	class MirrorView : public Placeholder<Auto, Auto> {
	private:
		friend class MirroringFrame;
	public:
		MirrorView(MirroringFrame& frame) : Placeholder(frame.size), frame(&frame), index(frame.AddMirrorView(*this)) {}
		~MirrorView() { if (frame != nullptr) { frame->RemoveMirrorView(index); } }
	private:
		ref_ptr<MirroringFrame> frame;
		std::list<std::reference_wrapper<MirrorView>>::iterator index;
	private:
		using Placeholder::SetSize;
	private:
		virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
			if (frame != nullptr) {
				frame->OnDraw(canvas, draw_region);
			}
		}
	};

private:
	std::list<std::reference_wrapper<MirrorView>> view_list;
private:
	std::list<std::reference_wrapper<MirrorView>>::iterator AddMirrorView(MirrorView& view) {
		return view_list.emplace(view_list.end(), std::ref(view));
	}
	void RemoveMirrorView(std::list<std::reference_wrapper<MirrorView>>::iterator index) {
		view_list.erase(index);
	}
public:
	std::unique_ptr<MirrorView> CreateMirrorView() { return create<MirrorView>(*this); }
private:
	void NotifySizeUpdate(Size size) {
		for (auto view : view_list) {
			view.get().SetSize(size);
		}
	}
	void NotifyRedraw(Rect redraw_region) {
		for (auto view : view_list) {
			view.get().Redraw(redraw_region);
		}
	}

	// layout
private:
	Size size = size_empty;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		Size child_size = UpdateChildSizeRef(child, size_ref);
		if (size != child_size) {
			size = child_size;
			NotifySizeUpdate(size);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (size != child_size) {
			size = child_size;
			SizeUpdated(size);
			NotifySizeUpdate(size);
		}
	}

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region);
		NotifyRedraw(child_redraw_region);
	}
};


template<class T>
MirroringFrame(T) -> MirroringFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign
