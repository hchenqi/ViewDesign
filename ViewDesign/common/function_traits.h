#pragma once

#include "core.h"

#include <tuple>


namespace ViewDesign {


template<class Func>
struct function_traits : public function_traits<decltype(&Func::operator())> {};

template<class Ret, class... Args>
struct function_traits<Ret(Args...)> {
	using argument_type_tuple = std::tuple<Args...>;

	template<size_t index>
	using argument_type = std::tuple_element_t<index, argument_type_tuple>;

	using return_type = Ret;
};

template<class Ret, class... Args>
struct function_traits<Ret(*)(Args...)> : public function_traits<Ret(Args...)> {};

template<class Ret, class Class, class... Args>
struct function_traits<Ret(Class::*)(Args...)> : public function_traits<Ret(Args...)> {};

template<class Ret, class Class, class... Args>
struct function_traits<Ret(Class::*)(Args...) const> : public function_traits<Ret(Args...)> {};


} // namespace ViewDesign
