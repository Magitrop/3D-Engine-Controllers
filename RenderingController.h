#pragma once
#include "pch.h"

class RenderingController final
{
private:
	RenderingController() = delete;
public:
	static void Render(int renderQueueIndex);
};