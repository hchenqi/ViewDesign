#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <list>
#include <functional>


namespace ViewDesign {


template<class... Arg> requires (sizeof...(Arg) <= 1)
class Observable : Uncopyable {
protected:
	class Observer : Uncopyable {
	private:
		friend class Observable;
	public:
		Observer(const Observable& observable, std::function<void(Arg...)> callback) : observable(&observable), index(observable.AddObserver(*this)), callback(std::move(callback)) {}
		~Observer() { if (observable) { observable->RemoveObserver(index); } }
	private:
		ref_ptr<const Observable> observable;
		std::list<ref_ptr<Observer>>::iterator index;
		std::function<void(Arg...)> callback;
	};

protected:
	~Observable() { for (auto observer : observer_list) { observer->observable = nullptr; } }

private:
	mutable std::list<ref_ptr<Observer>> observer_list;
private:
	std::list<ref_ptr<Observer>>::iterator AddObserver(Observer& observer) const {
		return observer_list.emplace(observer_list.end(), &observer);
	}
	void RemoveObserver(std::list<ref_ptr<Observer>>::iterator index) const {
		observer_list.erase(index);
	}

protected:
	void Notify(Arg... arg) const {
		for (auto it = observer_list.begin(); it != observer_list.end();) {
			(*it++)->callback(arg...);
		}
	}
};


} // namespace ViewDesign
