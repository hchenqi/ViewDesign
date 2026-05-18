#pragma once

#include <vector>


namespace ViewDesign {

namespace Vulkan {


template<class T>
class Registry {
private:
	std::vector<T> list;
public:
	size_t PutItem(T item) {
		size_t index = list.size();
		list.emplace_back(std::move(item));
		return index;
	}
	T& GetItem(size_t index) {
		return list[index];
	}
public:
	static Registry& Get() {
		static Registry registry;
		return registry;
	}
};


} // namespace Vulkan

} // namespace ViewDesign
