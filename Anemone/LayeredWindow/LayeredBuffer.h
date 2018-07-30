/**
* This file is part of Anemone.
*
* Anemone is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* The Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Anemone is distributed in the hope that it will be useful,
*
* But WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Anemone.
*
*   If not, see <http://www.gnu.org/licenses/>.
*
**/

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

