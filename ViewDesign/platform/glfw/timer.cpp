#include "ViewDesign/platform/glfw/timer.h"
#include "ViewDesign/platform/glfw/timer_queue.h"

#include <list>
#include <queue>

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {

namespace {


struct TimerEntry {
	ref_ptr<Timer> timer;
	uint period;
	uint64 due;

	size_t ref_count;
	std::list<TimerEntry>::iterator index;

	TimerEntry(Timer& timer, uint period, uint64 due) : timer(&timer), period(period), due(due), ref_count(0) {}
};

inline TimerEntry& AsTimerEntry(Handle handle) { return *static_cast<ref_ptr<TimerEntry>>(handle); }

std::list<TimerEntry> timer_list;


struct TimerQueueEntry {
	uint64 due;
	ref_ptr<TimerEntry> timer_entry;

	TimerQueueEntry(TimerEntry& timer_entry) : due(timer_entry.due), timer_entry(&timer_entry) { timer_entry.ref_count++; }

	auto operator<=>(const TimerQueueEntry& other) const = default;
};

std::priority_queue<TimerQueueEntry, std::vector<TimerQueueEntry>, std::greater<>> timer_queue;


inline uint64 GetTime() { return static_cast<uint64>(glfwGetTime() * 1000); }


} // namespace


Handle SetTimer(uint period, Timer& timer) {
	TimerEntry& entry = timer_list.emplace_front(timer, period, GetTime() + period); entry.index = timer_list.begin();
	timer_queue.emplace(entry);
	return &entry;
}

void ResetTimer(Handle handle, uint period) {
	TimerEntry& entry = AsTimerEntry(handle);
	entry.period = period;
	uint64 due = GetTime() + period;
	if (entry.due != due) {
		entry.due = due;
		timer_queue.emplace(entry);
	}
}

void KillTimer(Handle handle) {
	AsTimerEntry(handle).timer = nullptr;
}


double ProcessTimerQueue() {
	uint64 now = GetTime();

	while (!timer_queue.empty()) {
		TimerQueueEntry entry = timer_queue.top();

		if (entry.due > now) {
			return (entry.due - now) / 1000.0;
		}

		TimerEntry& timer_entry = *entry.timer_entry;

		timer_queue.pop();
		timer_entry.ref_count--;

		while (!timer_queue.empty()) {
			if (const TimerQueueEntry& next = timer_queue.top(); next == entry) {
				timer_queue.pop();
				timer_entry.ref_count--;
			} else {
				break;
			}
		}

		if (timer_entry.timer == nullptr) {
			goto skip;
		}

		if (entry.due != timer_entry.due) {
			goto skip;
		}

		timer_entry.timer->callback();

		if (timer_entry.timer == nullptr) {
			goto skip;
		}

		if (entry.due != timer_entry.due) {
			goto skip;
		}

		timer_entry.due += timer_entry.period;
		timer_queue.emplace(timer_entry);

	skip:
		if (timer_entry.ref_count == 0) {
			timer_list.erase(timer_entry.index);
		}
	}

	return 0.0;
}


} // namespace GLFW

} // namespace ViewDesign
