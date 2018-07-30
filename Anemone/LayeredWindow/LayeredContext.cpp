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
