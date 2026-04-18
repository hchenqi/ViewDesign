#pragma once

#include "ViewBase.h"

#include <memory>


namespace ViewDesign {


struct Relative {};
struct Fixed : Relative {};
struct Auto : Relative {};

template<class T> concept IsFixed = std::same_as<T, Fixed>;
template<class T> concept IsAuto = std::same_as<T, Auto>;
template<class T> concept IsRelative = std::same_as<T, Relative>;

template<class WidthTrait, class HeightTrait>
struct SizeTrait {
	static_assert(std::is_base_of_v<Relative, WidthTrait> && std::is_base_of_v<Relative, HeightTrait>, "invalid size trait");

	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};

template<class WidthTrait, class HeightTrait>
class ViewType : public ViewBase, public SizeTrait<WidthTrait, HeightTrait> {};


template<class WidthTrait = Relative, class HeightTrait = Relative>
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

template<class WidthTrait, class HeightTrait>
class view_ref : public view_ref<> {
public:
	template<class View> requires (std::is_base_of_v<WidthTrait, typename View::width_trait> && std::is_base_of_v<HeightTrait, typename View::height_trait>)
	view_ref(View& ref) : view_ref<>(ref) {}
	template<class WidthTraitRef, class HeightTraitRef> requires (std::is_base_of_v<WidthTrait, WidthTraitRef> && std::is_base_of_v<HeightTrait, HeightTraitRef>)
	view_ref(view_ref<WidthTraitRef, HeightTraitRef> ref) : view_ref<>(ref) {}
public:
	explicit view_ref(view_ref<> ref) : view_ref<>(ref) {}
	view_ref& operator=(view_ref<> ref) { view_ref<>::operator=(ref); return *this; }
};


template<class WidthTrait = Relative, class HeightTrait = Relative>
class view_ptr;

template<>
class view_ptr<Relative, Relative> : public std::unique_ptr<ViewBase> {
public:
	view_ptr() {}
	template<class View>
	view_ptr(std::unique_ptr<View> ptr) : std::unique_ptr<ViewBase>(std::move(ptr)) {}
	template<class View>
	view_ptr(alloc_ptr<View> ptr) : std::unique_ptr<ViewBase>(ptr) {}
public:
	operator ViewBase& () const { return **this; }
	operator ref_ptr<ViewBase>() const { return get(); }
};

template<class WidthTrait, class HeightTrait>
class view_ptr : public view_ptr<> {
public:
	view_ptr() : view_ptr<>() {}
	template<class View> requires (std::is_base_of_v<WidthTrait, typename View::width_trait> && std::is_base_of_v<HeightTrait, typename View::height_trait>)
	view_ptr(std::unique_ptr<View> ptr) : view_ptr<>(std::move(ptr)) {}
	template<class View> requires (std::is_base_of_v<WidthTrait, typename View::width_trait> && std::is_base_of_v<HeightTrait, typename View::height_trait>)
	view_ptr(alloc_ptr<View> ptr) : view_ptr<>(ptr) {}
	template<class WidthTraitPtr, class HeightTraitPtr> requires (std::is_base_of_v<WidthTrait, WidthTraitPtr> && std::is_base_of_v<HeightTrait, HeightTraitPtr>)
	view_ptr(view_ptr<WidthTraitPtr, HeightTraitPtr> ptr) : view_ptr<>(std::move(ptr)) {}
public:
	explicit view_ptr(view_ptr<> ptr) : view_ptr<>(std::move(ptr)) {}
	view_ptr& operator=(view_ptr<> ptr) & { view_ptr<>::operator=(std::move(ptr)); return *this; }
	view_ptr&& operator=(view_ptr<> ptr) && { view_ptr<>::operator=(std::move(ptr)); return std::move(*this); }
};


template<class T>
struct extract_size_trait;

template<class WidthTrait, class HeightTrait>
struct extract_size_trait<view_ptr<WidthTrait, HeightTrait>> {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};

template<class View>
struct extract_size_trait<alloc_ptr<View>> {
	using width_trait = typename View::width_trait;
	using height_trait = typename View::height_trait;
};

template<class View>
struct extract_size_trait<std::unique_ptr<View>> : public extract_size_trait<alloc_ptr<View>> {};

template<class T>
using extract_width_trait = typename extract_size_trait<T>::width_trait;

template<class T>
using extract_height_trait = typename extract_size_trait<T>::height_trait;


struct Vertical {};
struct Horizontal {};
struct Bidirectional {};


} // namespace ViewDesign
