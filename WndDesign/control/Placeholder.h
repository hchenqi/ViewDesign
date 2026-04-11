#pragma once

#include "../window/wnd_traits.h"


namespace ViewDesign {


template<class WidthType, class HeightType>
class Placeholder;


template<>
class Placeholder<Auto, Auto> : public WndType<Auto, Auto> {
public:
	Placeholder(Size size) : size(size) {}
protected:
	Size size;
public:
	void SetSize(Size size) { if (this->size != size) { SizeUpdated(this->size = size); } }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size; }
};


template<>
class Placeholder<Auto, Assigned> : public WndType<Auto, Assigned> {
public:
	Placeholder(float width) : size(width, 0.0f) {}
protected:
	Size size;
public:
	void SetWidth(float width) { if (size.width != width) { size.width = width; SizeUpdated(size); } }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { size.height = size_ref.height; return size; }
};


template<>
class Placeholder<Assigned, Auto> : public WndType<Assigned, Auto> {
public:
	Placeholder(float height) : size(0.0f, height) {}
protected:
	Size size;
public:
	void SetHeight(float height) { if (size.height != height) { size.height = height; SizeUpdated(size); } }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { size.width = size_ref.width; return size; }
};


template<>
class Placeholder<Assigned, Assigned> : public WndType<Assigned, Assigned> {};


} // namespace ViewDesign
