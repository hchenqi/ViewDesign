#pragma once


namespace ViewDesign {


template<class T>
struct Holder {
	Holder(auto&&... args) : value(std::forward<decltype(args)>(args)...) {}

	T value;
};


} // namespace ViewDesign
