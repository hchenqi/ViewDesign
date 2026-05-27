#pragma once

#include "ViewDesign/messaging/observable.h"


namespace ViewDesign {


template<class T>
class State : public Observable<const T&> {
public:
	using Watcher = typename Observable<const T&>::Observer;

public:
	State(auto&&... args) : value(std::forward<decltype(args)>(args)...) {}

private:
	T value;
public:
	const T& Get() const { return value; }
	void Set(auto&& arg) { value = std::forward<decltype(arg)>(arg); Observable<const T&>::Notify(value); }
	void Update(auto func) { func(value); Observable<const T&>::Notify(value); }
};


} // namespace ViewDesign
