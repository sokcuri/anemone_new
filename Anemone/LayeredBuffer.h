#pragma once
#include "SkBitmap.h"
#include "RawSize.h"
class LayeredBuffer
{
	friend class LayeredContext;

	SkBitmap *bitmap;
	RawSize *size;

	SkColorType colorType;
	SkAlphaType alphaType;

	HDC screenDC;
	HDC memDC;
	HBITMAP native;
	HBITMAP oldBitmap;
	void *scan0;

	BLENDFUNCTION blendFunc;

public:
	LayeredBuffer(int width, int height, SkColorType colorType, SkAlphaType alphaType);
	~LayeredBuffer();

	SkColorType GetColorType();
	SkAlphaType GetAlphaType();

private:
	void Initialize();
	void Resize(int width, int height);
	void SwapChain();
	void ReleaseResources();
	void CreateNativeContext();
};

