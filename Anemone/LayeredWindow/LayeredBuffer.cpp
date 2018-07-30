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

#include "stdafx.h"
#include "LayeredBuffer.h"

LayeredBuffer::LayeredBuffer(int width, int height, SkColorType colorType, SkAlphaType alphaType)
{
	this->size = new RawSize(width, height);
	this->colorType = colorType;
	this->alphaType = alphaType;

	Initialize();
}

LayeredBuffer::~LayeredBuffer()
{
	ReleaseResources();
	delete bitmap;
	delete size;
}

SkColorType LayeredBuffer::GetColorType()
{
	return colorType;
}

SkAlphaType LayeredBuffer::GetAlphaType()
{
	return alphaType;
}

RawSize *LayeredBuffer::GetSize() const
{
	return size;
}

void LayeredBuffer::Initialize()
{
	this->bitmap = new SkBitmap();
	SwapChain();
}

void LayeredBuffer::Resize(int width, int height)
{
	delete this->size;
	this->size = new RawSize(width, height);

	ReleaseResources();
	SwapChain();
}

void LayeredBuffer::SwapChain()
{
	CreateNativeContext();

	auto info = SkImageInfo::Make(size->cx, size->cy, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
	auto result = bitmap->installPixels(info, scan0, info.minRowBytes());
}

void LayeredBuffer::ReleaseResources()
{
	//delete this->bitmap;

	ReleaseDC(nullptr, screenDC);
	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);

	DeleteObject(native);
	DeleteObject(scan0);
	DeleteObject(oldBitmap);
}

void LayeredBuffer::CreateNativeContext()
{
	BITMAPV5HEADER bmh;
	bmh.bV5Size = sizeof(BITMAPV5HEADER);
	bmh.bV5Width = size->cx;
	bmh.bV5Height = -size->cy;
	bmh.bV5Planes = 1;
	bmh.bV5BitCount = 32;
	bmh.bV5Compression = BI_RGB;
	bmh.bV5AlphaMask = 0xFF000000;
	bmh.bV5RedMask = 0x00FF0000;
	bmh.bV5GreenMask = 0x0000FF00;
	bmh.bV5BlueMask = 0x000000FF;

	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255;
	blendFunc.AlphaFormat = AC_SRC_ALPHA;
	
	screenDC = GetDC(nullptr);
	memDC = CreateCompatibleDC(screenDC);

	native = CreateDIBSection(screenDC, reinterpret_cast<BITMAPINFO *>(&bmh), 0, &scan0, nullptr, 0);
	oldBitmap = (HBITMAP)SelectObject(memDC, native);
}
