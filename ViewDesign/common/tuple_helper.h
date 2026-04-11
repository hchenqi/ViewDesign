#pragma once

#include "core.h"

#include <tuple>


namespace ViewDesign {


template <std::size_t K, typename Tuple, std::size_t... Is>
auto _move_rotate_tuple_impl(Tuple&& t, std::index_sequence<Is...>) {
	return std::make_tuple(std::move(std::get<(Is + K) % sizeof...(Is)>(t))...);
}

template <std::size_t K, typename... Args>
auto move_rotate_tuple(std::tuple<Args...>&& t) {
	return _move_rotate_tuple_impl<K>(std::move(t), std::make_index_sequence<sizeof...(Args)>{});
}


template <typename... SrcArgs, typename... DestArgs, std::size_t... Is>
void _move_assign_tuple_impl(std::tuple<SrcArgs...>&& source, std::tuple<DestArgs...>& destination, std::index_sequence<Is...>) {
	((std::get<Is>(destination) = std::move(std::get<Is>(source))), ...);
}

template <typename... SrcArgs, typename... DestArgs>
void move_assign_tuple(std::tuple<SrcArgs...>&& source, std::tuple<DestArgs...>& destination) {
	static_assert(sizeof...(SrcArgs) == sizeof...(DestArgs), "Tuples must have the same size");
	_move_assign_tuple_impl(std::move(source), destination, std::index_sequence_for<SrcArgs...>{});
}


} // namespace ViewDesign
