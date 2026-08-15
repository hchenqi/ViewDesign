#pragma once

#include "ViewDesign/view/ViewBase.h"


namespace ViewDesign {


struct Relative {};
struct Bounded : Relative { using parent = Relative; };
struct Fixed : Bounded { using parent = Bounded; };
struct Auto : Relative { using parent = Relative; };

template<class T> concept IsRelative = std::same_as<T, Relative>;
template<class T> concept IsBounded = std::same_as<T, Bounded>;
template<class T> concept IsFixed = std::same_as<T, Fixed>;
template<class T> concept IsAuto = std::same_as<T, Auto>;


template<class T>
concept size_trait = std::derived_from<T, Relative>;


template<size_trait WidthTrait, size_trait HeightTrait>
class view_ref;

template<size_trait WidthTrait, size_trait HeightTrait>
class view_ptr;


template<size_trait WidthTrait, size_trait HeightTrait>
struct SizeTrait {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};


template<class T>
struct extract_size_trait;

template<class T>
using extract_width_trait = typename extract_size_trait<T>::width_trait;

template<class T>
using extract_height_trait = typename extract_size_trait<T>::height_trait;


template<view_type View> requires (!requires { typename View::width_trait; typename View::height_trait; })
struct extract_size_trait<View> {
	using width_trait = Relative;
	using height_trait = Relative;
};

template<view_type View> requires (requires { typename View::width_trait; typename View::height_trait; })
struct extract_size_trait<View> {
	using width_trait = typename View::width_trait;
	using height_trait = typename View::height_trait;
};

template<view_type View>
struct extract_size_trait<owner_ptr<View>> : extract_size_trait<View> {};

template<view_type View>
struct extract_size_trait<std::unique_ptr<View>> : extract_size_trait<View> {};

template<size_trait WidthTrait, size_trait HeightTrait>
struct extract_size_trait<view_ref<WidthTrait, HeightTrait>> {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};

template<size_trait WidthTrait, size_trait HeightTrait>
struct extract_size_trait<view_ptr<WidthTrait, HeightTrait>> {
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;
};


template<class T, class WidthTrait, class HeightTrait>
concept size_trait_equal = std::same_as<extract_width_trait<T>, WidthTrait>&& std::same_as<extract_height_trait<T>, HeightTrait>;

template<class T1, class T2>
concept size_trait_equal_with = size_trait_equal<T1, extract_width_trait<T2>, extract_height_trait<T2>>;

template<class T, class WidthTrait, class HeightTrait>
concept size_trait_compatible = std::derived_from<extract_width_trait<T>, WidthTrait>&& std::derived_from<extract_height_trait<T>, HeightTrait>;

template<class T1, class T2>
concept size_trait_compatible_with = size_trait_compatible<T1, extract_width_trait<T2>, extract_height_trait<T2>>;


template<class T1, class T2>
concept compatible_unique_ptr_type = unique_ptr_type<T1> && size_trait_compatible_with<T1, T2>;


template<size_trait WidthTrait, size_trait HeightTrait>
class view_ref : public view_ref_any {
public:
	template<view_type View> requires (size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ref(View& ref) : view_ref_any(ref) {}
	template<view_type View> requires (!size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ref(View& ref) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<class WidthTraitRef, class HeightTraitRef> requires (size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>)
	view_ref(view_ref<WidthTraitRef, HeightTraitRef> ref) : view_ref_any(ref) {}
	template<class WidthTraitRef, class HeightTraitRef> requires (!size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>)
	view_ref(view_ref<WidthTraitRef, HeightTraitRef> ref) { static_assert(size_trait_compatible<view_ref<WidthTraitRef, HeightTraitRef>, WidthTrait, HeightTrait>, "size traits incompatible"); }
};

template<size_trait WidthTrait, size_trait HeightTrait>
class view_ptr : public view_ptr_any {
public:
	view_ptr() : view_ptr_any() {}
	template<view_type View> requires (size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ptr(owner_ptr<View> ptr) : view_ptr_any(ptr) {}
	template<view_type View> requires (!size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ptr(owner_ptr<View> ptr) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<view_type View> requires (size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ptr(std::unique_ptr<View> ptr) : view_ptr_any(std::move(ptr)) {}
	template<view_type View> requires (!size_trait_compatible<View, WidthTrait, HeightTrait>)
	view_ptr(std::unique_ptr<View> ptr) { static_assert(size_trait_compatible<View, WidthTrait, HeightTrait>, "size traits incompatible"); }
	template<class WidthTraitPtr, class HeightTraitPtr> requires (size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>)
	view_ptr(view_ptr<WidthTraitPtr, HeightTraitPtr> ptr) : view_ptr_any(std::move(ptr)) {}
	template<class WidthTraitPtr, class HeightTraitPtr> requires (!size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>)
	view_ptr(view_ptr<WidthTraitPtr, HeightTraitPtr> ptr) { static_assert(size_trait_compatible<view_ptr<WidthTraitPtr, HeightTraitPtr>, WidthTrait, HeightTrait>, "size traits incompatible"); }
};


} // namespace ViewDesign
