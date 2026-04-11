#pragma once

#include "../ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait = Auto, class HeightTrait = Auto>
class FixedFrame;


template<>
class FixedFrame<Auto, Auto> : public ViewFrame, public SizeTrait<Auto, Auto> {
public:
	FixedFrame(float width, view_ptr<Fixed, Auto> child) : ViewFrame(std::move(child)) {
		SetChildData<uint>(this->child, child_assigned_auto);
		size = Size(width, UpdateChildSizeRef(this->child, Size(width, length_min)).height);
	}
	FixedFrame(float height, view_ptr<Auto, Fixed> child) : ViewFrame(std::move(child)) {
		SetChildData<uint>(this->child, child_auto_assigned);
		size = Size(UpdateChildSizeRef(this->child, Size(length_min, height)).width, height);
	}
	FixedFrame(Size size, view_ptr<Fixed, Fixed> child) : ViewFrame(std::move(child)) {
		SetChildData<uint>(this->child, child_assigned_assigned);
		this->size = size; UpdateChildSizeRef(this->child, size);
	}
protected:
	enum {
		child_assigned_auto,
		child_auto_assigned,
		child_assigned_assigned,
	};
protected:
	Size size;
public:
	void SetSize(Size size) {
		switch (GetChildData<uint>(child)) {
		case child_assigned_auto:
			if (this->size.width != size.width) {
				this->size = Size(size.width, UpdateChildSizeRef(this->child, Size(size.width, length_min)).height);
				SizeUpdated(this->size);
				Redraw(region_infinite);
			} break;
		case child_auto_assigned:
			if (this->size.height != size.height) {
				this->size = Size(UpdateChildSizeRef(this->child, Size(length_min, size.height)).width, size.height);
				SizeUpdated(this->size);
				Redraw(region_infinite);
			} break;
		case child_assigned_assigned:
			if (this->size != size) {
				this->size = size; UpdateChildSizeRef(this->child, size);
				SizeUpdated(this->size);
				Redraw(region_infinite);
			} break;
		}
	}
	void SetWidth(float width) { SetSize(Size(width, size.height)); }
	void SetHeight(float height) { SetSize(Size(size.width, height)); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		switch (GetChildData<uint>(child)) {
		case child_assigned_auto: if (size.height != child_size.height) { size.height = child_size.height; SizeUpdated(size); } break;
		case child_auto_assigned: if (size.width != child_size.width) { size.width = child_size.width; SizeUpdated(size); } break;
		}
	}
};


template<>
class FixedFrame<Auto, Fixed> : public ViewFrame, public SizeTrait<Auto, Fixed> {
public:
	FixedFrame(float width, view_ptr<Fixed, Fixed> child) : ViewFrame(std::move(child)), size(width, 0.0f) {}
protected:
	Size size;
public:
	void SetWidth(float width) {
		if (size.width != width) {
			size.width = width;
			UpdateChildSizeRef(child, size);
			SizeUpdated(size);
			Redraw(region_infinite);
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.height != size_ref.height) { size.height = size_ref.height; UpdateChildSizeRef(child, size); }
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}
};


template<>
class FixedFrame<Fixed, Auto> : public ViewFrame, public SizeTrait<Fixed, Auto> {
public:
	FixedFrame(float height, view_ptr<Fixed, Fixed> child) : ViewFrame(std::move(child)), size(0.0f, height) {}
protected:
	Size size;
public:
	void SetHeight(float height) {
		if (size.height != height) {
			size.height = height;
			UpdateChildSizeRef(child, size);
			SizeUpdated(size);
			Redraw(region_infinite);
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.width != size_ref.width) { size.width = size_ref.width; UpdateChildSizeRef(child, size); }
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}
};


} // namespace ViewDesign
