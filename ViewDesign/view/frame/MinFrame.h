#pragma once

#include "ViewDesign/view/ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait = Auto, class HeightTrait = Auto>
class MinFrame;


template<>
class MinFrame<Auto, Auto> : public ViewFrame, public SizeTrait<Auto, Auto> {
public:
	MinFrame(Size size_min, view_ptr<Relative, Relative> child) : ViewFrame(std::move(child)), size_min(size_min) {
		Size child_size = UpdateChildSizeRef(this->child, size_min);
		size = Size(std::max(size_min.width, child_size.width), std::max(size_min.height, child_size.height));
	}
protected:
	Size size;
	Size size_min;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(std::max(size_min.width, child_size.width), std::max(size_min.height, child_size.height));
		SizeUpdated(size);
	}
};


template<class WidthTrait> requires (!IsAuto<WidthTrait>)
class MinFrame<WidthTrait, Auto> : public ViewFrame, public SizeTrait<WidthTrait, Auto> {
public:
	MinFrame(float height_min, view_ptr<WidthTrait, Relative> child) : ViewFrame(std::move(child)), height_min(height_min) {}
protected:
	Size size;
	float height_min;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = UpdateChildSizeRef(child, Size(size_ref.width, height_min));
		size.height = std::max(height_min, size.height);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(child_size.width, std::max(height_min, child_size.height));
		SizeUpdated(size);
	}
};


template<class HeightTrait> requires (!IsAuto<HeightTrait>)
class MinFrame<Auto, HeightTrait> : public ViewFrame, public SizeTrait<Auto, HeightTrait> {
public:
	MinFrame(float width_min, view_ptr<Relative, HeightTrait> child) : ViewFrame(std::move(child)), width_min(width_min) {}
protected:
	Size size;
	float width_min;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = UpdateChildSizeRef(child, Size(width_min, size_ref.height));
		size.width = std::max(width_min, size.width);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(std::max(width_min, child_size.width), child_size.height);
		SizeUpdated(size);
	}
};


template<class T>
MinFrame(Size, T) -> MinFrame<Auto, Auto>;


} // namespace ViewDesign
