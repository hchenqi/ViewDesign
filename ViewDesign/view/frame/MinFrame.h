#pragma once

#include "../ViewFrame.h"


namespace ViewDesign {


template<class WidthType = Auto, class HeightType = Auto>
class MinFrame;


template<>
class MinFrame<Auto, Auto> : public ViewFrame, public LayoutType<Auto, Auto> {
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
		child_size = Size(std::max(size_min.width, child_size.width), std::max(size_min.height, child_size.height));
		if (size != child_size) { size = child_size; SizeUpdated(size); }
	}
};


template<>
class MinFrame<Assigned, Auto> : public ViewFrame, public LayoutType<Assigned, Auto> {
public:
	MinFrame(float height_min, view_ptr<Assigned, Relative> child) : ViewFrame(std::move(child)), height_min(height_min) {}
protected:
	Size size;
	float height_min;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.width != size_ref.width) {
			size.width = size_ref.width;
			size.height = std::max(height_min, UpdateChildSizeRef(child, Size(size_ref.width, height_min)).height);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		float height = std::max(height_min, child_size.height);
		if (size.height != height) { size.height = height; SizeUpdated(size); }
	}
};


template<>
class MinFrame<Auto, Assigned> : public ViewFrame, public LayoutType<Auto, Assigned> {
public:
	MinFrame(float width_min, view_ptr<Relative, Assigned> child) : ViewFrame(std::move(child)), width_min(width_min) {}
protected:
	Size size;
	float width_min;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size.height != size_ref.height) {
			size.height = size_ref.height;
			size.width = std::max(width_min, UpdateChildSizeRef(child, Size(width_min, size_ref.height)).width);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		float width = std::max(width_min, child_size.width);
		if (size.width != width) { size.width = width; SizeUpdated(size); }
	}
};


template<class Size, class T>
MinFrame(Size, T) -> MinFrame<std::conditional_t<IsAssigned<extract_width_type<T>>, Assigned, Auto>, std::conditional_t<IsAssigned<extract_height_type<T>>, Assigned, Auto>>;


} // namespace ViewDesign
