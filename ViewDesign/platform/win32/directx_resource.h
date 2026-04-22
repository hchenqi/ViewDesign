#pragma once


namespace ViewDesign {

class Bitmap;

namespace Win32 {


void RegisterBitmap(Bitmap& bitmap);
void UnregisterBitmap(Bitmap& bitmap);

void DirectXRecreateResource();


} // namespace Win32

} // namespace ViewDesign
