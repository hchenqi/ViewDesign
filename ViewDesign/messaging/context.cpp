#include "ViewDesign/messaging/context.h"

#include <unordered_set>


namespace ViewDesign {

struct ViewBasePrivateAccess : public ViewBase {
	ref_ptr<ViewBasePrivateAccess> Parent() { return static_cast<ref_ptr<ViewBasePrivateAccess>>(parent); }
};

namespace {

std::unordered_set<ref_ptr<ViewBase>> provider_set;

} // namespace


ContextProvider::ContextProvider(ViewBase& provider) : provider(provider) {
	if (provider_set.contains(&provider)) {
		throw std::invalid_argument("ContextProvider already registered");
	}
	provider_set.emplace(&provider);
}

ContextProvider::~ContextProvider() {
	provider_set.erase(&provider);
}

ref_ptr<ViewBase> Context::GetNextProvider(ViewBase& user) {
	auto parent = static_cast<ref_ptr<ViewBasePrivateAccess>>(&user)->Parent();
	while (parent && !provider_set.contains(parent)) {
		parent = parent->Parent();
	}
	return parent;
}


} // namespace ViewDesign
