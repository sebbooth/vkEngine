#pragma once

#ifndef VKCONFIG_H
#define VKCONFIG_H

#include <vector>

struct VkConfig {
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	bool enableValidationLayers = false;

	const int maxFramesInFlight = 2;
	const uint32_t screenWidth = 800;
	const uint32_t screenHeight = 600;
	uint32_t downScaleFactor = 2;

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

	// Clock
	float lastFrameTime = 0.0f;
	double lastTime = 0.0f;

	// Controls
	double cursorX = -999;
	double cursorY = -999;
	double prevCursorX = cursorX;
	double prevCursorY = cursorY;
	float rotateSpeed = 0.01;
	float moveSpeed = 0.05;

	// Octree
	int octreeDepth = 7;
	int octreeWidth = static_cast<int>(pow(2, octreeDepth));
	bool terrain = true;

	// Shader
	bool visualizeOctree = true;
	bool shadows = false;
};

#endif // !VKCONFIG_H
