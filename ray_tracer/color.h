#pragma once

#ifndef clamp
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#endif

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color() : r(0), g(0), b(0), a(255) {}
	Color(int r, int g, int b) : r(clamp(r, 0, 255)), g(clamp(g, 0, 255)), b(clamp(b, 0, 255)), a(255) {}
	Color(int r, int g, int b, int a) : r(clamp(r, 0, 255)), g(clamp(g, 0, 255)), b(clamp(b, 0, 255)), a(clamp(a, 0, 255)) {}

	Color operator + (const Color& col) const { return Color(r + col.r, g + col.g, b + col.b, a + col.a); }
	Color operator - (const Color& col) const { return Color(r - col.r, g - col.g, b - col.b, a - col.a); }
};