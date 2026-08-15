#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait = Auto, class HeightTrait = Auto>
class MaxFrame;


template<>
class MaxFrame<Auto, Auto> : public ViewFrame, public SizeTrait<Auto, Auto> {
public:
	MaxFrame(Size size, view_ptr<Bounded, Bounded> child) : ViewFrame(std::move(child)), child_type(ChildType::BoundedBounded), size(UpdateChildSizeRef(this->child, size)) {}
	MaxFrame(float width, view_ptr<Bounded, Auto> child) : ViewFrame(std::move(child)), child_type(ChildType::BoundedAuto), size(UpdateChildSizeRef(this->child, Size(width, length_zero))) {}
	MaxFrame(float height, view_ptr<Auto, Bounded> child) : ViewFrame(std::move(child)), child_type(ChildType::AutoBounded), size(UpdateChildSizeRef(this->child, Size(length_zero, height))) {}
protected:
	enum class ChildType { BoundedBounded, BoundedAuto, AutoBounded } child_type;
	Size size;
protected:
	void CheckChildType(ChildType child_type) const {
		if (this->child_type != child_type) {
			throw std::logic_error("MaxFrame: child type mismatch");
		}
	}
public:
	void SetSize(Size size) {
		CheckChildType(ChildType::BoundedBounded);
		SizeUpdated(this->size = UpdateChildSizeRef(child, size));
	}
	void SetWidth(float width) {
		CheckChildType(ChildType::BoundedAuto);
		SizeUpdated(size = UpdateChildSizeRef(child, Size(width, length_zero)));
	}
	void SetHeight(float height) {
		CheckChildType(ChildType::AutoBounded);
		SizeUpdated(size = UpdateChildSizeRef(child, Size(length_zero, height)));
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(size = child_size); }
};


template<class WidthTrait> requires (!IsAuto<WidthTrait>)
class MaxFrame<WidthTrait, Auto> : public ViewFrame, public SizeTrait<WidthTrait, Auto> {
public:
	MaxFrame(float height, view_ptr<WidthTrait, Bounded> child) : ViewFrame(std::move(child)), size_ref(0.0f, height) {}
protected:
	Size size_ref;
	Size size;
public:
	void SetHeight(float height) {
		if (size_ref.height != height) {
			size_ref.height = height;
			SizeUpdated(size = UpdateChildSizeRef(child, size_ref));
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (this->size_ref.width != size_ref.width) {
			this->size_ref.width = size_ref.width;
			size = UpdateChildSizeRef(child, this->size_ref);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		SizeUpdated(size = child_size);
	}
};


template<class HeightTrait> requires (!IsAuto<HeightTrait>)
class MaxFrame<Auto, HeightTrait> : public ViewFrame, public SizeTrait<Auto, HeightTrait> {
public:
	MaxFrame(float width, view_ptr<Bounded, HeightTrait> child) : ViewFrame(std::move(child)), size_ref(width, 0.0f) {}
protected:
	Size size_ref;
	Size size;
public:
	void SetWidth(float width) {
		if (size_ref.width != width) {
			size_ref.width = width;
			SizeUpdated(size = UpdateChildSizeRef(child, size_ref));
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (this->size_ref.height != size_ref.height) {
			this->size_ref.height = size_ref.height;
			size = UpdateChildSizeRef(child, this->size_ref);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		SizeUpdated(size = child_size);
	}
};


} // namespace ViewDesign
