#pragma once
#include <memory>
#include <unordered_map>
#include <tiny_obj_loader.h>
#include <stdexcept>

#include "VkStructs.h"

class Model
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const std::string MODEL_PATH = "assets/models/viking_room/viking_room.obj";

	Model();

};

