#pragma once

#ifndef VKCONFIG_H
#define VKCONFIG_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <vector>
#include <array>

struct ComputeUniformBufferObject {
	alignas(16) glm::vec3 camPos = glm::vec3(2048, 256, 2048);
	alignas(16) glm::vec3 camDir = glm::vec3(0, -1, 0);
	alignas(16) glm::vec3 camUp = glm::vec3(0, 0, 1);
	alignas(16) glm::vec3 sunDir = glm::vec3(-1, -2, -3);
	alignas(16) glm::vec3 fogColor = glm::vec3(0, 0, 0);

	float fogMinDistance = 1.0f;
	float fogMaxDistance = 10000.0f;

	float depthPassCorrection = 10.0f;

	float deltaTime = 1.0f;
	float focalLength = 4.0f;
	float viewportHeight = 1.0f;
	float rayEpsilon = 0.01f;

	int width = 600;
	int height = 800;
	int octreeSize;
	int octreeMaxDepth;

	int minX;
	int maxX;
	int minY;
	int maxY;
	int minZ;
	int maxZ;

	int numChunks = 4;

	unsigned int showDepth = 0;
	unsigned int showNormals = 0;
	unsigned int findChunkMode = 1;
	unsigned int useInitialDepthPass = 1;
};

struct VkConfig {
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	bool enableValidationLayers = false;

	const int maxFramesInFlight = 3;
	const uint32_t screenWidth = 800;
	const uint32_t screenHeight = 600;
	int downScaleFactor = 2;
	
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
	bool visualizeOctree = false;
	bool shadows = false;
	bool showDepth = 0;
	bool showNormals = 0;
	bool useInitialDepthPass = 1;
	// Compute UBO
	ComputeUniformBufferObject ubo{};

	glm::ivec3 curChunk;
	glm::ivec3 prevChunk;
};

#endif // !VKCONFIG_H
