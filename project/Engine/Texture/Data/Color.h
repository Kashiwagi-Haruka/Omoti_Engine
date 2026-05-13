#pragma once
#include "Vector4.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace Color {
// 0-255 の RGBA 値を 0.0-1.0 の Vector4 に変換する
Vector4 RGBAToVector4(int r, int g, int b, int a);

// 0xRRGGBBAA 形式のカラーコードを Vector4(RGBA) に変換する
Vector4 ColorCodeToVector4(uint32_t color);

// HSV(0-360, 0-100, 0-100) + Alpha(0-100) を Vector4(RGBA) に変換する
Vector4 HSVIntToVector4(int h, int s, int v, int a = 100);
} // namespace Color