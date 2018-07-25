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

	RawSize *GetSize() const;

	void Resize(int width, int height);

private:
	void Initialize();
	void SwapChain();
	void ReleaseResources();
	void CreateNativeContext();
};

