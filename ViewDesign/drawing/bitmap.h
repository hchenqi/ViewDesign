#pragma once

#include "../common/uncopyable.h"


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
