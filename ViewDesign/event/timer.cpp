#include "timer.h"

#include <unordered_map>
#include <cassert>

#include <Windows.h>


namespace ViewDesign {

namespace {

struct TimerSyncMap : public std::unordered_map<HANDLE, Timer&> {
	~TimerSyncMap() { while (!empty()) { begin()->second.Stop(); } }
}timer_sync_map;


void TimerCallbackSync(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4) {
	if (auto it = timer_sync_map.find(reinterpret_cast<HANDLE>(Arg3)); it != timer_sync_map.end()) {
		it->second.callback();
	}
}

HANDLE SetTimerSync(uint period, Timer& timer_object) {
	HANDLE timer = (HANDLE)SetTimer(NULL, NULL, period, TimerCallbackSync);
	timer_sync_map.emplace(timer, timer_object);
	return timer;
}

void ResetTimerSync(HANDLE timer, uint period) {
	assert(timer_sync_map.find(timer) != timer_sync_map.end());
	SetTimer(NULL, (UINT_PTR)timer, period, TimerCallbackSync);
}

void KillTimerSync(HANDLE timer) {
	auto it = timer_sync_map.find(timer);
	assert(it != timer_sync_map.end());
	KillTimer(NULL, reinterpret_cast<UINT_PTR>(timer));
	timer_sync_map.erase(it);
}

} // namespace


void Timer::Set(uint period) {
	if (!IsSet()) {
		timer = SetTimerSync(period, *this);
	} else {
		ResetTimerSync(timer, period);
	}
}

void Timer::Stop() {
	if (!IsSet()) { return; }
	KillTimerSync(timer);
	timer = nullptr;
}


} // namespace ViewDesign
