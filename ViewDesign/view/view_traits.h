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
	static_assert(std::derived_from<WidthTrait, Relative> && std::derived_from<HeightTrait, Relative>, "invalid size trait");

	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};

template<class WidthTrait, class HeightTrait>
class ViewType : public ViewBase, public SizeTrait<WidthTrait, HeightTrait> {};


template<class View>
struct extract_size_trait {
	using width_trait = typename View::width_trait;
	using height_trait = typename View::height_trait;
};

template<class T> requires (!requires { typename T::width_trait; typename T::height_trait; })
struct extract_size_trait<T> {
	using width_trait = Relative;
	using height_trait = Relative;
};

template<class View>
struct extract_size_trait<alloc_ptr<View>> : extract_size_trait<View> {};

template<class View>
struct extract_size_trait<std::unique_ptr<View>> : extract_size_trait<View> {};

template<class T>
using extract_width_trait = typename extract_size_trait<T>::width_trait;

template<class T>
using extract_height_trait = typename extract_size_trait<T>::height_trait;

template<class T, class WidthTrait, class HeightTrait>
concept size_trait_compatible = std::derived_from<extract_width_trait<T>, WidthTrait> && std::derived_from<extract_height_trait<T>, HeightTrait>;


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
	template<class View> requires size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ref(View& ref) : view_ref<>(ref) {}
	template<class View> requires !size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ref(View& ref) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<class WidthTraitRef, class HeightTraitRef> requires size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>
	view_ref(view_ref<WidthTraitRef, HeightTraitRef> ref) : view_ref<>(ref) {}
	template<class WidthTraitRef, class HeightTraitRef> requires !size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>
	view_ref(view_ref<WidthTraitRef, HeightTraitRef> ref) { static_assert(size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>, "size traits incompatible"); }
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
	operator ViewBase& () const { return *get(); }
	operator ref_ptr<ViewBase>() const { return get(); }
};

template<class WidthTrait, class HeightTrait>
class view_ptr : public view_ptr<> {
public:
	view_ptr() : view_ptr<>() {}
	template<class View> requires size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ptr(std::unique_ptr<View> ptr) : view_ptr<>(std::move(ptr)) {}
	template<class View> requires !size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ptr(std::unique_ptr<View> ptr) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<class View> requires size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ptr(alloc_ptr<View> ptr) : view_ptr<>(ptr) {}
	template<class View> requires !size_trait_compatible<View, WidthTrait, HeightTrait>
	view_ptr(alloc_ptr<View> ptr) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<class WidthTraitPtr, class HeightTraitPtr> requires size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>
	view_ptr(view_ptr<WidthTraitPtr, HeightTraitPtr> ptr) : view_ptr<>(std::move(ptr)) {}
	template<class WidthTraitPtr, class HeightTraitPtr> requires !size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>
	view_ptr(view_ptr<WidthTraitPtr, HeightTraitPtr> ptr) { static_assert(size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>, "size traits incompatible"); }
};


template<class WidthTrait, class HeightTrait>
struct extract_size_trait<view_ref<WidthTrait, HeightTrait>> {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};

template<class WidthTrait, class HeightTrait>
struct extract_size_trait<view_ptr<WidthTrait, HeightTrait>> {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};


struct Vertical {};
struct Horizontal {};
struct Bidirectional {};


} // namespace ViewDesign
