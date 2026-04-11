#pragma once

#include "../view/ViewBase.h"


namespace ViewDesign {


class ContextProvider : Uncopyable {
private:
	ViewBase& provider;
public:
	ContextProvider(ViewBase& provider);
	~ContextProvider();
};


class Context : Uncopyable {
private:
	ViewBase& user;
	ref_ptr<ViewBase> provider;
private:
	static ref_ptr<ViewBase> GetNextProvider(ViewBase& user);
public:
	Context(ViewBase& user) : user(user), provider(nullptr) {}
public:
	template<class View> requires std::is_base_of_v<ViewBase, View>
	View& Get() {
		if (!provider || !dynamic_cast<ref_ptr<View>>(provider)) {
			ref_ptr<ViewBase> next = &user;
			do {
				next = GetNextProvider(*next);
				if (next == nullptr) {
					throw std::invalid_argument("context provider not found");
				}
				provider = dynamic_cast<ref_ptr<View>>(next);
			} while (!provider);
		}
		return static_cast<View&>(*provider);
	}
};


} // namespace ViewDesign
