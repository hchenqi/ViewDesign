#pragma once

#include "ViewDesign/messaging/observable.h"


namespace ViewDesign {


class Signal : public Observable<> {
public:
	using Listener = Observable<>::Observer;

public:
	using Observable<>::Notify;
};


} // namespace ViewDesign
