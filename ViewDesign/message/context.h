#pragma once

#include "../window/WndObject.h"


namespace ViewDesign {


class ContextProvider : Uncopyable {
private:
	WndObject& provider;
public:
	ContextProvider(WndObject& provider);
	~ContextProvider();
};


class Context : Uncopyable {
private:
	WndObject& user;
	ref_ptr<WndObject> provider;
private:
	static ref_ptr<WndObject> GetNextProvider(WndObject& user);
public:
	Context(WndObject& user) : user(user), provider(nullptr) {}
public:
	template<class Wnd> requires std::is_base_of_v<WndObject, Wnd>
	Wnd& Get() {
		if (!provider || !dynamic_cast<ref_ptr<Wnd>>(provider)) {
			ref_ptr<WndObject> next = &user;
			do {
				next = GetNextProvider(*next);
				if (next == nullptr) {
					throw std::invalid_argument("context provider not found");
				}
				provider = dynamic_cast<ref_ptr<Wnd>>(next);
			} while (!provider);
		}
		return static_cast<Wnd&>(*provider);
	}
};


} // namespace ViewDesign
