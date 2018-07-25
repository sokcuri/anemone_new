#include "stdafx.h"
#include "LayeredContext.h"


LayeredContext::LayeredContext(HWND target, LayeredBuffer *buffer) : SkCanvas(*buffer->bitmap), emptyPoint(0, 0), targetPosition(0, 0)
{
	this->target = target;
	this->buffer = buffer;
}

LayeredContext::~LayeredContext()
{
}

void LayeredContext::Present()
{
	auto screenSize = buffer->size;

	UpdateLayeredWindow(target, buffer->screenDC, &targetPosition, screenSize, buffer->memDC, &emptyPoint, 0, &buffer->blendFunc, ULW_ALPHA);
}

void LayeredContext::SetPosition(int x, int y)
{
	targetPosition.x = x;
	targetPosition.y = y;
}
