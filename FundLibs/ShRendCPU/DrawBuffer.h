#pragma once

#include <windows.h>
#include <iostream>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct color {
	uchar r, g, b;
	color(uchar r = 0, uchar g = 0, uchar b = 0);
	color(int r, int g, int b);
	color(float r, float g, float b);
}color;

typedef struct DB {
private:
	void DrawCharOnBuffer(size_t index, color col);
	void DrawCharOnBufferHalf(float firstHalfint, size_t index, color col);
	void Clip(int &x, int &y);
	BITMAPINFO buffInfo;
	BYTE* buff;
	uint w, h;
public:
	bool init(uint w, uint h);
	~DB();

	BYTE* GetBuffer();
	uint GetW();
	uint GetH();

	void Fill(int x1, int y1, int x2, int y2, color col = {0, 0, 0});

	void DrawPixel(int x, int y, color col = { 0, 0, 0 });
	void Draw4Pixel(int x, int y, color col = { 0, 0, 0 });
	void DrawPixelHalf(float firstHalfint, int x, int y, color col = { 0, 0, 0 });
	void Draw4PixelHalf(float firstHalfint, int x, int y, color col = { 0, 0, 0 });
	void Draw16PixelHalf(float firstHalfint, int x, int y, color col = { 0, 0, 0 });
	void Draw256PixelHalf(float firstHalfint, int x, int y, color col = { 0, 0, 0 });
	void DrawLine(int x1, int y1, int x2, int y2, color col = {0, 0, 0});

	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color col = {0, 0, 0});
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color col = {0, 0, 0});

	void DrawCircle(int xc, int yc, int r, color col = {0, 0, 0});
	void FillCircle(int xc, int yc, int r, color col = {0, 0, 0});

	void OnDrawContextDrawBuffer(HDC* dc);
}DB;