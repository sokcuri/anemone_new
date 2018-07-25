#include "stdafx.h"
#include "LayeredContext.h"


LayeredContext::LayeredContext(HWND target, LayeredBuffer *buffer) : SkCanvas(*buffer->bitmap)
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

	UpdateLayeredWindow(target, buffer->screenDC, targetPosition, screenSize, buffer->memDC, emptyPoint, 0, &buffer->blendFunc, ULW_ALPHA);
}
