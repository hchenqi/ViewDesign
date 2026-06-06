#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <list>


namespace ViewDesign {


class Trackable : Uncopyable {
public:
	class ConstReference : Uncopyable {
	private:
		friend class Trackable;
	public:
		ConstReference(const Trackable& trackable) : trackable(&trackable), index(trackable.AddReference(*this)) {}
		~ConstReference() { if (trackable != nullptr) { trackable->RemoveReference(index); } }
	private:
		ref_ptr<const Trackable> trackable;
		std::list<ref_ptr<ConstReference>>::iterator index;
	public:
		ref_ptr<const Trackable> GetTrackable() const { return trackable; }
	};

	class Reference : private ConstReference {
	public:
		Reference(Trackable& trackable) : ConstReference(trackable) {}
	public:
		ref_ptr<Trackable> GetTrackable() const { return const_cast<ref_ptr<Trackable>>(ConstReference::GetTrackable()); }
	};

protected:
	~Trackable() {
		for (auto reference : reference_list) {
			reference->trackable = nullptr;
		}
	}

private:
	mutable std::list<ref_ptr<ConstReference>> reference_list;
private:
	std::list<ref_ptr<ConstReference>>::iterator AddReference(ConstReference& reference) const {
		return reference_list.emplace(reference_list.end(), &reference);
	}
	void RemoveReference(std::list<ref_ptr<ConstReference>>::iterator index) const {
		reference_list.erase(index);
	}
};


} // namespace ViewDesign
