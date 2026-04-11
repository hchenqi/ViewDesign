#pragma once

#include "timer.h"
#include "../common/coroutine.h"

#include <memory>


namespace ViewDesign {


class Timeout : private Timer {
public:
	Timeout() : Timer([&]() { function ? Reset()() : Stop(); }) {}
	~Timeout() {}

private:
	std::function<void(void)> function;
private:
	std::function<void(void)> Reset() { return std::move(function); }

public:
	void Set(std::function<void(void)> function, uint delay) { this->function = function; Timer::Set(delay); }  // in milliseconds
	void Cancel() { Stop(); Reset(); }
};


inline std::shared_ptr<Timeout> SetTimeout(std::function<void(void)> function, uint delay) {
	std::shared_ptr<Timeout> timeout(new Timeout());
	timeout->Set([timeout, function]() { function(); }, delay);
	return timeout;
}


inline TaskAwaitable<> SetTimeout(uint delay) {
	return SetTask([=](Continuation<> continuation) { SetTimeout([=]() { continuation(); }, delay); });
}


} // namespace ViewDesign
