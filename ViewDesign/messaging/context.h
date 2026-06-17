#pragma once

#include "ViewDesign/view/ViewBase.h"


namespace ViewDesign {


class ContextProvider : Uncopyable {
private:
	ViewBase& view;
protected:
	ContextProvider(ViewBase& view);
	~ContextProvider();
private:
	template<class View> friend class Context;
private:
	static ref_ptr<ViewBase> GetNextProvider(ViewBase& view);
};


template<class View>
class Context {
private:
	ViewBase& consumer;
	mutable ref_ptr<ViewBase> provider;
public:
	Context(ViewBase& consumer) : consumer(consumer), provider(&consumer) {}
public:
	View& Get() const {
		if (provider == &consumer) {
			for (ref_ptr<ViewBase> view = &consumer;;) {
				view = ContextProvider::GetNextProvider(*view);
				if (view == nullptr) {
					provider = nullptr;
					break;
				}
				if (provider = dynamic_cast<ref_ptr<View>>(view); provider != nullptr) {
					break;
				}
			}
		}
		if (provider == nullptr) {
			throw std::logic_error("context provider not available");
		}
		return static_cast<View&>(*provider);
	}
	void Drop() const {
		provider = &consumer;
	}
	View& GetCurrent() const {
		Drop();
		return Get();
	}
};


} // namespace ViewDesign
