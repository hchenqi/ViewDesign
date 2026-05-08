#pragma once


namespace ViewDesign {

namespace DirectX {

struct DXGIDevice;
struct DXGIAdapter;
struct DXGIFactory;


DXGIDevice& GetDXGIDevice();
DXGIAdapter& GetDXGIAdapter();
DXGIFactory& GetDXGIFactory();


} // namespace DirectX

} // namespace ViewDesign
