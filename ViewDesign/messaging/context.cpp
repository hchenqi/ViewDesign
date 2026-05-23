#include "ViewDesign/messaging/context.h"

#include <unordered_set>


namespace ViewDesign {

struct ViewBasePrivateAccess : public ViewBase {
	ref_ptr<ViewBasePrivateAccess> Parent() { return static_cast<ref_ptr<ViewBasePrivateAccess>>(parent); }
};

namespace {

std::unordered_set<ref_ptr<ViewBase>> provider_set;

} // namespace


ContextProvider::ContextProvider(ViewBase& view) : view(view) {
	if (provider_set.contains(&view)) {
		throw std::invalid_argument("view already already registered as a context provider");
	}
	provider_set.emplace(&view);
}

ContextProvider::~ContextProvider() {
	provider_set.erase(&view);
}

ref_ptr<ViewBase> ContextProvider::GetNextProvider(ViewBase& view) {
	auto parent = static_cast<ref_ptr<ViewBasePrivateAccess>>(&view)->Parent();
	for (; parent && !provider_set.contains(parent); parent = parent->Parent()) {}
	return parent;
}


} // namespace ViewDesign
