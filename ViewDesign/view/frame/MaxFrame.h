#pragma once

#include "../ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait = Auto, class HeightTrait = Auto>
class MaxFrame;


template<>
class MaxFrame<Auto, Auto> : public ViewFrame, public SizeTrait<Auto, Auto> {
public:
	MaxFrame(Size size_max, view_ptr<Relative, Relative> child) : ViewFrame(std::move(child)), size_max(size_max) {
		Size child_size = UpdateChildSizeRef(this->child, size_max);
		size = Size(std::min(size_max.width, child_size.width), std::min(size_max.height, child_size.height));
	}
protected:
	Size size;
	Size size_max;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(std::min(size_max.width, child_size.width), std::min(size_max.height, child_size.height));
		SizeUpdated(size);
	}
};


template<class WidthTrait> requires (!IsAuto<WidthTrait>)
class MaxFrame<WidthTrait, Auto> : public ViewFrame, public SizeTrait<WidthTrait, Auto> {
public:
	MaxFrame(float height_max, view_ptr<WidthTrait, Relative> child) : ViewFrame(std::move(child)), height_max(height_max) {}
protected:
	Size size;
	float height_max;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = UpdateChildSizeRef(child, Size(size_ref.width, height_max));
		size.height = std::min(height_max, size.height);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(child_size.width, std::min(height_max, child_size.height));
		SizeUpdated(size);
	}
};


template<class HeightTrait> requires (!IsAuto<HeightTrait>)
class MaxFrame<Auto, HeightTrait> : public ViewFrame, public SizeTrait<Auto, HeightTrait> {
public:
	MaxFrame(float width_max, view_ptr<Relative, HeightTrait> child) : ViewFrame(std::move(child)), width_max(width_max) {}
protected:
	Size size;
	float width_max;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = UpdateChildSizeRef(child, Size(width_max, size_ref.height));
		size.width = std::min(width_max, size.width);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		size = Size(std::min(width_max, child_size.width), child_size.height);
		SizeUpdated(size);
	}
};


template<class T>
MaxFrame(Size, T) -> MaxFrame<Auto, Auto>;


} // namespace ViewDesign
