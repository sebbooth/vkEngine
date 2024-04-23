#pragma once

#ifndef VKCONFIG_H
#define VKCONFIG_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <vector>

struct ComputeUniformBufferObject {
	alignas(16) glm::vec3 camPos = glm::vec3(262.905243, 270.311707, 282.328888);
	alignas(16) glm::vec3 camDir = glm::vec3(-0.519135, -0.634374, -0.572781);
	alignas(16) glm::vec3 camUp = glm::vec3(-0.422386, 0.773021, -0.473317);
	alignas(16) glm::vec3 sunDir = glm::vec3(-1, -2, -3);

	float deltaTime = 1.0f;
	float focalLength = 4.0f;
	float viewportHeight = 1.0f;
	int width = 600;
	int height = 800;
	int octreeSize;
	int octreeMaxDepth;
};

struct VkConfig {
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	bool enableValidationLayers = false;

	const int maxFramesInFlight = 2;
	const uint32_t screenWidth = 800;
	const uint32_t screenHeight = 600;
	int downScaleFactor = 1;
	
	bool newSwapChainNeeded = false;

	// Rasterization pipeline only
	bool msaaEnabled = false;
	bool wireframeEnabled = false;
	bool cullBackFaces = false;
	bool displayNormals = false;
	bool depthStencilEnabled = false;


	bool computeEnabled = false;
	bool guiEnabled = true;
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
	float rotateSpeed = 0.01f;
	float moveSpeed = 0.05f;
	bool cursorActive = true;

	// Octree
	int octreeDepth = 7;
	int octreeWidth = static_cast<int>(pow(2, octreeDepth));
	bool terrain = true;

	// Shader
	bool visualizeOctree = true;
	bool shadows = false;

	// Compute UBO
	ComputeUniformBufferObject ubo{};
};

#endif // !VKCONFIG_H
