#pragma once

#include "../common/uncopyable.h"

#include <list>
#include <functional>


namespace ViewDesign {


template<class T>
class State : Uncopyable {
public:
	class Watcher : Uncopyable {
	private:
		friend class State;
	public:
		Watcher(const State& state, std::function<void(const T&)> callback) : state(&state), callback(callback) { state.AddWatcher(*this); }
		~Watcher() { if (state) { state->RemoveWatcher(*this); } }
	private:
		ref_ptr<const State> state;
		std::list<ref_ptr<Watcher>>::iterator index = {};
		std::function<void(const T&)> callback;
	private:
		void Callback(const T& value) const { if (callback) { callback(value); } }
	public:
		ref_ptr<const T> Get() const { return state ? &state->Get() : nullptr; }
	};

public:
	State(T value) : value(std::move(value)) {}
	~State() { for (auto& watcher : watcher_list) { if (watcher) { watcher->state = nullptr; } } }
private:
	T value;
	mutable std::list<ref_ptr<Watcher>> watcher_list;
private:
	void AddWatcher(Watcher& watcher) const {
		watcher_list.emplace_front(&watcher);
		watcher.index = watcher_list.begin();
	}
	void RemoveWatcher(Watcher& watcher) const {
		*(watcher.index) = nullptr;
	}
public:
	const T& Get() const {
		return value;
	}
	void Set(T value) {
		this->value = std::move(value);
		for (auto it = watcher_list.begin(); it != watcher_list.end();) {
			if (*it == nullptr) { it = watcher_list.erase(it); continue; }
			(*it++)->Callback(this->value);
		}
	}
};


} // namespace ViewDesign
