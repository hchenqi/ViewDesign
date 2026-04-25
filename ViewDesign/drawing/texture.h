#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <stdexcept>


namespace ViewDesign {

struct TextureResource;


class Texture : Uncopyable {
private:
	mutable owner_ptr<TextureResource> resource;
public:
	Texture();
	~Texture();
public:
	bool IsEmpty() const { return resource == nullptr; }
	ref_ptr<TextureResource> GetResource() const { if (IsEmpty()) { throw std::invalid_argument("invalid texture resource"); } return resource; }
public:
	void Set(owner_ptr<TextureResource> resource) { Destroy(); this->resource = resource; }
	void Destroy();
};


} // namespace ViewDesign
