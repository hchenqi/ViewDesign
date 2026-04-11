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
		child_size = Size(std::min(size_max.width, child_size.width), std::min(size_max.height, child_size.height));
		if (size != child_size) { size = child_size; SizeUpdated(size); }
	}
};


template<>
class MaxFrame<Fixed, Auto> : public ViewFrame, public SizeTrait<Fixed, Auto> {
public:
	MaxFrame(float height_max, view_ptr<Fixed, Relative> child) : ViewFrame(std::move(child)), height_max(height_max) {}
protected:
	Size size;
	float height_max;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.width != size_ref.width) {
			size.width = size_ref.width;
			size.height = std::min(height_max, UpdateChildSizeRef(child, Size(size_ref.width, height_max)).height);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		float height = std::min(height_max, child_size.height);
		if (size.height != height) { size.height = height; SizeUpdated(size); }
	}
};


template<>
class MaxFrame<Auto, Fixed> : public ViewFrame, public SizeTrait<Auto, Fixed> {
public:
	MaxFrame(float width_max, view_ptr<Relative, Fixed> child) : ViewFrame(std::move(child)), width_max(width_max) {}
protected:
	Size size;
	float width_max;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.height != size_ref.height) {
			size.height = size_ref.height;
			size.width = std::min(width_max, UpdateChildSizeRef(child, Size(width_max, size_ref.height)).width);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		float width = std::min(width_max, child_size.width);
		if (size.width != width) { size.width = width; SizeUpdated(size); }
	}
};


template<class Size, class T>
MaxFrame(Size, T) -> MaxFrame<std::conditional_t<IsFixed<extract_width_trait<T>>, Fixed, Auto>, std::conditional_t<IsFixed<extract_height_trait<T>>, Fixed, Auto>>;


} // namespace ViewDesign
