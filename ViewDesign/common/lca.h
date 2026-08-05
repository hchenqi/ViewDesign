#pragma once

#include <concepts>
#include <type_traits>


namespace ViewDesign {


template <class T>
struct parent {
	using type = typename T::parent;
};


template <class A, class B>
struct lca;

template <class A, class B> requires (std::derived_from<B, A>)
struct lca<A, B> {
    using type = A;
};

template <class A, class B> requires (!std::derived_from<B, A> && std::derived_from<A, B>)
struct lca<A, B> {
    using type = B;
};

template <class A, class B> requires (!std::derived_from<B, A> && !std::derived_from<A, B>)
struct lca<A, B> {
    using type = typename lca<typename parent<A>::type, B>::type;
};


} // namespace ViewDesign
