#pragma once


namespace ViewDesign {

struct DXGIDevice;
struct DXGIAdapter;
struct DXGIFactory;


DXGIDevice& GetDXGIDevice();
DXGIAdapter& GetDXGIAdapter();
DXGIFactory& GetDXGIFactory();


} // namespace ViewDesign
