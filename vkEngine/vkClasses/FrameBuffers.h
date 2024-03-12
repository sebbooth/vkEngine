#pragma once
#include "DepthResources.h"

class FrameBuffers
{
public:
	std::shared_ptr<DepthResources> depthResourcesObj;

	FrameBuffers(std::shared_ptr<DepthResources> depthResourcesObj);
	void create();
};

