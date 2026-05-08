#include "ViewDesign/event/timer.h"

#include <unordered_map>
#include <cassert>

#include <windows.h>


namespace ViewDesign {

namespace {

using Handle = Timer::Handle;

struct TimerSyncMap : public std::unordered_map<Handle, Timer&> {
	~TimerSyncMap() { while (!empty()) { begin()->second.Stop(); } }
}timer_sync_map;


void CALLBACK TimerCallbackSync(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4) {
	if (auto it = timer_sync_map.find(reinterpret_cast<Handle>(Arg3)); it != timer_sync_map.end()) {
		it->second.callback();
	}
}

Handle SetTimerSync(uint period, Timer& timer_object) {
	Handle timer = (Handle)SetTimer(NULL, (UINT_PTR)NULL, period, TimerCallbackSync);
	timer_sync_map.emplace(timer, timer_object);
	return timer;
}

void ResetTimerSync(Handle timer, uint period) {
	assert(timer_sync_map.find(timer) != timer_sync_map.end());
	SetTimer(NULL, (UINT_PTR)timer, period, TimerCallbackSync);
}

void KillTimerSync(Handle timer) {
	auto it = timer_sync_map.find(timer);
	assert(it != timer_sync_map.end());
	KillTimer(NULL, (UINT_PTR)timer);
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
