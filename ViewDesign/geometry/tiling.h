#pragma once

#include "ViewDesign/common/tile_range.h"
#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/geometry/sizeu.h"


namespace ViewDesign {


inline Vector GetTileOffset(Size tile_size, TileIndex tile) {
	return Vector(tile.x * tile_size.width, tile.y * tile_size.height);
}

inline Rect GetTileRegion(Size tile_size, TileIndex tile) {
	return Rect(point_zero + GetTileOffset(tile_size, tile), tile_size);
}

inline TileRange GetOverlappingTileRange(Size tile_size, Rect region) {
	if (tile_size.IsEmpty()) { throw std::invalid_argument("empty tile size"); }
	return {
		{ static_cast<int>(floorf(region.left() / tile_size.width)), static_cast<int>(floorf(region.top() / tile_size.height)) },
		{ static_cast<int>(ceilf(region.right() / tile_size.width)), static_cast<int>(ceilf(region.bottom() / tile_size.height)) }
	};
}


struct TilingFuncDefault {
	SizeU operator()(SizeU tile_size_current, Size size_new) {			
		SizeU tile_size = size_u_empty;

		if (size_new.width <= 64.0f) { tile_size.width = 64; }
		else if (size_new.width <= 128.0f) { tile_size.width = 128; }
		else if (size_new.width <= 256.0f) { tile_size.width = 256; }
		else { tile_size.width = 512; }

		if (size_new.height <= 64.0f) { tile_size.height = 64; }
		else if (size_new.height <= 128.0f) { tile_size.height = 128; }
		else if (size_new.height <= 256.0f) { tile_size.height = 256; }
		else { tile_size.height = 512; }

		if (tile_size.width < tile_size_current.width) { tile_size.width = tile_size_current.width; }
		if (tile_size.height < tile_size_current.height) { tile_size.height = tile_size_current.height; }

		return tile_size;
	}
};


} // namespace ViewDesign
