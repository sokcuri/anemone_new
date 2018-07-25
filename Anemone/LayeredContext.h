#pragma once
#include "SkCanvas.h"
#include "LayeredBuffer.h"
#include "RawPoint.h"
class LayeredContext : SkCanvas
{
	HWND target;
	LayeredBuffer *buffer;

	RawPoint *emptyPoint = new RawPoint(0, 0);
	RawPoint *targetPosition;

public:
	LayeredContext(HWND hWnd, LayeredBuffer *buffer);
	~LayeredContext();

	void Present();
};

