#pragma once

#ifndef RENDERINGSETTING_H
#define RENDERINGSETTING_H

#include <vector>

struct RenderingSettings {
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	bool enableValidationLayers = false;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	// Rasterization pipeline only
	bool msaaEnabled = false;
	bool wireframeEnabled = false;
	bool cullBackFaces = false;
	bool displayNormals = false;
	bool depthStencilEnabled = false;


	bool computeEnabled = false;
	bool guiEnabled = false;
	bool uboEnabled = true;
	bool samplerEnabled = true;
	bool simpleShader = true;
};

#endif // !RENDERINGSETTING_H
