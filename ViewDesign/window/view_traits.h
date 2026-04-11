#pragma once

#include "ViewBase.h"

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
class ViewType : public ViewBase, public LayoutType<WidthType, HeightType> {};


template<class WidthType = Relative, class HeightType = Relative>
class view_ref;

template<>
class view_ref<Relative, Relative> : public std::reference_wrapper<ViewBase> {
public:
	template<class View>
	view_ref(View& ref) : std::reference_wrapper<ViewBase>(ref) {}
public:
	operator ViewBase& () const { return get(); }
	operator ref_ptr<ViewBase>() const { return &get(); }
	ref_ptr<ViewBase> operator->() const { return &get(); }
};

template<class WidthType, class HeightType>
class view_ref : public view_ref<> {
public:
	template<class View> requires (std::is_base_of_v<WidthType, typename View::width_type> && std::is_base_of_v<HeightType, typename View::height_type>)
	view_ref(View& ref) : view_ref<>(ref) {}
public:
	view_ref&& operator=(view_ref<>&& ref) { view_ref<>::operator=(ref); return *this; }
};


template<class WidthType = Relative, class HeightType = Relative>
class view_ptr;

template<>
class view_ptr<Relative, Relative> : public std::unique_ptr<ViewBase> {
public:
	view_ptr() {}
	template<class View>
	view_ptr(std::unique_ptr<View> ptr) : std::unique_ptr<ViewBase>(ptr.release()) {}
	template<class View>
	view_ptr(alloc_ptr<View> ptr) : view_ptr(std::unique_ptr<View>(ptr)) {}
public:
	operator ViewBase& () const { return **this; }
	operator ref_ptr<ViewBase>() const { return get(); }
};

template<class WidthType, class HeightType>
class view_ptr : public view_ptr<> {
public:
	view_ptr() {}
	template<class View> requires (std::is_base_of_v<WidthType, typename View::width_type> && std::is_base_of_v<HeightType, typename View::height_type>)
	view_ptr(std::unique_ptr<View> ptr) : view_ptr<>(std::move(ptr)) {}
	template<class View> requires (std::is_base_of_v<WidthType, typename View::width_type> && std::is_base_of_v<HeightType, typename View::height_type>)
	view_ptr(alloc_ptr<View> ptr) : view_ptr(std::unique_ptr<View>(ptr)) {}
public:
	view_ptr&& operator=(view_ptr<>&& ptr) { view_ptr<>::operator=(std::move(ptr)); return std::move(*this); }
};


template<class T>
struct extract_layout_type;

template<class WidthType, class HeightType>
struct extract_layout_type<view_ptr<WidthType, HeightType>> {
	using width_type = WidthType;
	using height_type = HeightType;
};

template<class View>
struct extract_layout_type<alloc_ptr<View>> {
	using width_type = typename View::width_type;
	using height_type = typename View::height_type;
};

template<class View>
struct extract_layout_type<std::unique_ptr<View>> : public extract_layout_type<alloc_ptr<View>> {};

template<class T>
using extract_width_type = typename extract_layout_type<T>::width_type;

template<class T>
using extract_height_type = typename extract_layout_type<T>::height_type;


struct Vertical {};
struct Horizontal {};
struct Bidirectional {};


} // namespace ViewDesign
