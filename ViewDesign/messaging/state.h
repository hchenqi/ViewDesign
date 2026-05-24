#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <functional>
#include <list>
#include <stdexcept>


namespace ViewDesign {


template<class T>
class State : Uncopyable {
public:
	class Watcher : Uncopyable {
	private:
		friend class State;
	public:
		Watcher(const State& state, std::function<void(const T&)> callback) : state(&state), index(state.AddWatcher(*this)), callback(std::move(callback)) {}
		~Watcher() { if (state) { state->RemoveWatcher(index); } }
	private:
		ref_ptr<const State> state;
		std::list<ref_ptr<Watcher>>::iterator index;
		std::function<void(const T&)> callback;
	public:
		const T& Get() const { if (state == nullptr) { throw std::logic_error("state destructed"); } return state->Get(); }
	};

public:
	State(auto&&... args) : value(std::forward<decltype(args)>(args)...) {}
	~State() { for (auto& watcher : watcher_list) { watcher->state = nullptr; } }

private:
	T value;
public:
	const T& Get() const {
		return value;
	}
	void Set(auto&& arg) {
		this->value = std::forward<decltype(arg)>(arg);
		OnValueUpdate();
	}
	void Update(auto func) {
		func(this->value);
		OnValueUpdate();
	}

private:
	mutable std::list<ref_ptr<Watcher>> watcher_list;
private:
	std::list<ref_ptr<Watcher>>::iterator AddWatcher(Watcher& watcher) const {
		return watcher_list.emplace(watcher_list.end(), &watcher);
	}
	void RemoveWatcher(std::list<ref_ptr<Watcher>>::iterator index) const {
		watcher_list.erase(index);
	}
public:
	void OnValueUpdate() {
		for (auto it = watcher_list.begin(); it != watcher_list.end(); ) {
			(*it++)->callback(value);
		}
	}
};


} // namespace ViewDesign
