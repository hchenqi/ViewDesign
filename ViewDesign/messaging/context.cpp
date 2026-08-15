#include "ViewDesign/messaging/context.h"

#include <unordered_set>


namespace ViewDesign {

namespace {

std::unordered_set<ref_ptr<ViewBase>> provider_set;

inline ref_ptr<ViewBase> CheckFindProvider(ViewBase& view) {
	return provider_set.contains(&view) ? &view : view.HasParent() ? CheckFindProvider(view.GetParent()) : nullptr;
}

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
	return view.HasParent() ? CheckFindProvider(view.GetParent()) : nullptr;
}


} // namespace ViewDesign
