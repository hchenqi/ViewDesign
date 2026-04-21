#pragma once


namespace ViewDesign {

class Bitmap;


void RegisterBitmap(Bitmap& bitmap);
void UnregisterBitmap(Bitmap& bitmap);

void DirectXRecreateResource();


} // namespace ViewDesign
