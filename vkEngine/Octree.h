#pragma once
#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <map>
#include <cmath>

#include "OctreeStructs.h"

class Octree
{
public:
	Octree(std::vector<Voxel> voxelArray, int depth);
	Octree() = default;

public:
	std::vector<OctreeNode> octreeArray;

private:
	std::vector<std::vector<OctreeNode>> layerArrays;
	std::vector<std::map<OctreeNodeCoord, OctreeNode>> layerMaps;

private:
	std::map<OctreeNodeCoord, OctreeNode> bottomLayer(
		std::vector<Voxel> voxelArray,
		int layerSize
	);

	std::map<OctreeNodeCoord, OctreeNode> middleLayer(
		std::map<OctreeNodeCoord, OctreeNode> prevLayerMap,
		int layerSize
	);

	std::vector<OctreeNode> layerArray(std::map<OctreeNodeCoord, OctreeNode> layerMap);
};

#endif