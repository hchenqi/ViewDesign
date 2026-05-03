#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <functional>


namespace ViewDesign {


class Timer : Uncopyable {
public:
	using Handle = void*;
private:
	Handle timer;
public:
	std::function<void(void)> callback;
public:
	Timer(std::function<void(void)> callback) : timer(nullptr), callback(callback) {}
	~Timer() { Stop(); }
public:
	bool IsSet() const { return timer != nullptr; }
	void Set(uint period); // milliseconds
	void Stop();
};


} // namespace ViewDesign
