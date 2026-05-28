#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <list>
#include <functional>
#include <stdexcept>


namespace ViewDesign {


template<class... Arg> requires (sizeof...(Arg) <= 1)
class Observable : Uncopyable {
protected:
	class Observer : Uncopyable {
	private:
		friend class Observable;
	public:
		Observer(const Observable& observable, std::function<void(Arg...)> callback) : observable(&observable), index(observable.AddObserver(*this)), callback(std::move(callback)) {}
		~Observer() { if (observable != nullptr) { observable->RemoveObserver(index); } }
	private:
		ref_ptr<const Observable> observable;
		std::list<ref_ptr<Observer>>::iterator index;
		std::function<void(Arg...)> callback;
	};

protected:
	~Observable() {
		if (current != observer_list.end()) {
			std::terminate();
		}
		for (auto observer : observer_list) {
			observer->observable = nullptr;
		}
	}

private:
	mutable std::list<ref_ptr<Observer>> observer_list;
	mutable std::list<ref_ptr<Observer>>::iterator current = observer_list.end();
private:
	std::list<ref_ptr<Observer>>::iterator AddObserver(Observer& observer) const {
		return observer_list.emplace(observer_list.end(), &observer);
	}
	void RemoveObserver(std::list<ref_ptr<Observer>>::iterator index) const {
		if (index == current) {
			*index = nullptr;
		} else {
			observer_list.erase(index);
		}
	}

protected:
	void Notify(Arg... arg) const {
		if (current != observer_list.end()) {
			throw std::logic_error("Observable: Notify() called within a Notify()");
		}
		try {
			for (current = observer_list.begin(); current != observer_list.end();) {
				(*current)->callback(arg...);
				if (*current == nullptr) {
					current = observer_list.erase(current);
				} else {
					current++;
				}
			}
		} catch (...) {
			if (*current == nullptr) {
				observer_list.erase(current);
			}
			current = observer_list.end();
			throw;
		}
	}
};


} // namespace ViewDesign
