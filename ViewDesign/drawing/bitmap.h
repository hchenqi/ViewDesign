#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"

#include <stdexcept>


namespace ViewDesign {

struct BitmapResource;


class Bitmap : Uncopyable {
private:
	mutable owner_ptr<BitmapResource> bitmap;
public:
	Bitmap();
	~Bitmap();
	bool IsEmpty() const { return bitmap == nullptr; }
	void Set(owner_ptr<BitmapResource> bitmap) { Destroy(); this->bitmap = bitmap; }
	void Destroy();
	ref_ptr<BitmapResource> Get() const { if (IsEmpty()) { throw std::invalid_argument("invalid bitmap resource"); } return bitmap; }
};


} // namespace ViewDesign
