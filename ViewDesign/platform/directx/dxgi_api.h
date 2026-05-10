#pragma once

#include <dxgi1_4.h>


namespace ViewDesign {

namespace DirectX {

using DXGIDevice = IDXGIDevice;
using DXGIAdapter = IDXGIAdapter;
using DXGIFactory = IDXGIFactory2;
using DXGISwapChain = IDXGISwapChain1;


DXGIDevice& GetDXGIDevice();
DXGIAdapter& GetDXGIAdapter();
DXGIFactory& GetDXGIFactory();


} // namespace DirectX

} // namespace ViewDesign
