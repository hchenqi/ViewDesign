#include "bitmap.h"

#include "../system/directx_resource.h"
#include "../system/directx_helper.h"


namespace ViewDesign {


Bitmap::Bitmap() : bitmap(nullptr) { RegisterBitmap(*this); }

Bitmap::~Bitmap() { Destroy(); UnregisterBitmap(*this); }

void Bitmap::Destroy() { SafeRelease(&bitmap); }


} // namespace ViewDesign
