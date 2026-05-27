#pragma once

#include "ViewDesign/messaging/observable.h"


namespace ViewDesign {


class Signal : public Observable<> {
public:
	using Listener = Observable<>::Observer;

public:
	void Notify() const {
		Observable<>::Notify();
	}
};


} // namespace ViewDesign
