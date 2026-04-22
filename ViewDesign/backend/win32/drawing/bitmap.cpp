#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/win32/directx_resource.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


Bitmap::Bitmap() : bitmap(nullptr) { RegisterBitmap(*this); }

Bitmap::~Bitmap() { Destroy(); UnregisterBitmap(*this); }

void Bitmap::Destroy() { SafeRelease(&bitmap); }


} // namespace ViewDesign
