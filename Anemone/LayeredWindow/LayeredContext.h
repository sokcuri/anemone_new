#pragma once
#include "SkCanvas.h"
#include "LayeredBuffer.h"
#include "RawPoint.h"
class LayeredContext : public SkCanvas
{
	RawPoint emptyPoint;
	RawPoint targetPosition;
public:
	HWND target;
	LayeredBuffer *buffer;

	LayeredContext(HWND hWnd, LayeredBuffer *buffer);
	~LayeredContext();

	void Present();
	void SetPosition(int x, int y);
};

