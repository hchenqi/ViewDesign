#pragma once

#include <stdexcept>
#include <utility>

#include <wrl/client.h>


namespace ViewDesign {

namespace DirectX {


template<class T>
class ComPtr : public Microsoft::WRL::ComPtr<T> {
public:
	operator T& () {
		if (Microsoft::WRL::ComPtr<T>::Get() == nullptr) {
			throw std::logic_error("DirectX: empty ComPtr");
		}
		return *Microsoft::WRL::ComPtr<T>::Get();
	}

public:
	ComPtr& Swap(owner_ptr<T>& ptr) {
		ptr = std::exchange(Microsoft::WRL::ComPtr<T>::ptr_, ptr);
		return *this;
	}
};


constexpr struct HResultWrapper {
	const HResultWrapper& operator<<(HRESULT hr) const {
		if (!SUCCEEDED(hr)) {
			throw std::runtime_error("DirectX: operation failed");
		}
		return *this;
	}
} hr;


} // namespace DirectX

} // namespace ViewDesign
