#pragma once

#include "../common/core.h"


namespace ViewDesign {


class Bitmap;

void RegisterBitmap(Bitmap& bitmap);
void UnregisterBitmap(Bitmap& bitmap);

void DirectXRecreateResource();


} // namespace ViewDesign
