#pragma once

#include "WndObject.h"

#include <memory>


namespace ViewDesign {


struct Relative {};
struct Assigned : Relative {};
struct Auto : Relative {};

template<class T> constexpr bool IsRelative = std::is_same_v<T, Relative>;
template<class T> constexpr bool IsAssigned = std::is_same_v<T, Assigned>;
template<class T> constexpr bool IsAuto = std::is_same_v<T, Auto>;

template<class WidthType, class HeightType>
struct LayoutType {
	static_assert(std::is_base_of_v<Relative, WidthType> && std::is_base_of_v<Relative, HeightType>, "invalid layout type");

	using width_type = WidthType;
	using height_type = HeightType;
};

template<class WidthType, class HeightType>
class WndType : public WndObject, public LayoutType<WidthType, HeightType> {};


template<class WidthType = Relative, class HeightType = Relative>
class child_ref;

template<>
class child_ref<Relative, Relative> : public std::reference_wrapper<WndObject> {
public:
	template<class ChildType>
	child_ref(ChildType& ref) : std::reference_wrapper<WndObject>(ref) {}
public:
	operator WndObject& () const { return get(); }
	operator ref_ptr<WndObject>() const { return &get(); }
	ref_ptr<WndObject> operator->() const { return &get(); }
};

template<class WidthType, class HeightType>
class child_ref : public child_ref<> {
public:
	template<class ChildType> requires (std::is_base_of_v<WidthType, typename ChildType::width_type>&& std::is_base_of_v<HeightType, typename ChildType::height_type>)
		child_ref(ChildType& ref) : child_ref<>(ref) {}
public:
	child_ref&& operator=(child_ref<>&& ref) { child_ref<>::operator=(ref); return *this; }
};


template<class WidthType = Relative, class HeightType = Relative>
class child_ptr;

template<>
class child_ptr<Relative, Relative> : public std::unique_ptr<WndObject> {
public:
	child_ptr() {}
	template<class ChildType>
	child_ptr(std::unique_ptr<ChildType> ptr) : std::unique_ptr<WndObject>(ptr.release()) {}
	template<class ChildType>
	child_ptr(alloc_ptr<ChildType> ptr) : child_ptr(std::unique_ptr<ChildType>(ptr)) {}
public:
	operator WndObject& () const { return **this; }
	operator ref_ptr<WndObject>() const { return get(); }
};

template<class WidthType, class HeightType>
class child_ptr : public child_ptr<> {
public:
	child_ptr() {}
	template<class ChildType> requires (std::is_base_of_v<WidthType, typename ChildType::width_type> && std::is_base_of_v<HeightType, typename ChildType::height_type>)
	child_ptr(std::unique_ptr<ChildType> ptr) : child_ptr<>(std::move(ptr)) {}
	template<class ChildType> requires (std::is_base_of_v<WidthType, typename ChildType::width_type> && std::is_base_of_v<HeightType, typename ChildType::height_type>)
	child_ptr(alloc_ptr<ChildType> ptr) : child_ptr(std::unique_ptr<ChildType>(ptr)) {}
public:
	child_ptr&& operator=(child_ptr<>&& ptr) { child_ptr<>::operator=(std::move(ptr)); return std::move(*this); }
};


template<class T>
struct extract_layout_type;

template<class WidthType, class HeightType>
struct extract_layout_type<child_ptr<WidthType, HeightType>> {
	using width_type = WidthType;
	using height_type = HeightType;
};

template<class ChildType>
struct extract_layout_type<alloc_ptr<ChildType>> {
	using width_type = typename ChildType::width_type;
	using height_type = typename ChildType::height_type;
};

template<class ChildType>
struct extract_layout_type<std::unique_ptr<ChildType>> : public extract_layout_type<alloc_ptr<ChildType>> {};

template<class T>
using extract_width_type = typename extract_layout_type<T>::width_type;

template<class T>
using extract_height_type = typename extract_layout_type<T>::height_type;


struct Vertical {};
struct Horizontal {};
struct Bidirectional {};


} // namespace ViewDesign
