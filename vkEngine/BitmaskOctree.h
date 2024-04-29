#pragma once
#ifndef BITMASKOCTREE_H
#define BITMASKOCTREE_H

#include <vector>
#include <array>
#include <map>
#include <cmath>
#include <random>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <algorithm>

#include "OctreeStructs.h"
#include "SimplexNoise.h"
#include "Perlin.h"

struct Node {
	bool p0;
	bool p1;
	bool p2;
	bool p3;
	bool p4;
	bool p5;
	bool p6;
	bool p7;
	int mat;
};

/*
OCTREE with bitmask?

indices

size: 8^0 = 1        ->       0 : 1
size: 8^1 = 8        ->       1 : 9
size: 8^2 = 64       ->       9 : 73
size: 8^3 = 512      ->      73 : 585
size: 8^4 = 4680     ->     585 : 5265
size: 8^5 = 32768    ->    5265 : 38033
size: 8^6 = 262144   ->   38033 : 300177
size: 8^7 = 2097152  ->  300177 : 2397329


19.2 MB LOD 7   * 16 (4x4) = 307.2
2.40 MB LOD 6   * 48 (8x8) = 115.2
0.30 MB LOD 5   * 192 (16x16) = 57.6
0.04 MB LOD 4   * 36 (32x32) = 1.44
0.005 MB LOD 3  * 768 (64x64) = 3.84
0.0005 MB LOD 2  * 12,288 (128x128) = 6.144
... LOD 1    16  *    49,152    (256x256) = 3.538944
0.000008 LOD 0   18  * 196,608  (512x512) 1.57

bitmask: 3 bits (0->7)

array of bitmasks:
[
   0: 001 -> 1 =   1 + 1 = 2
   2: 010 -> 2 =   9 + 1*8   + 2 = 19
  19: 100 -> 4 =  72 + 1*64  + 2*8  + 4 = 156
 156: 101 -> 5 = 585 + 1*512 + 2*64 + 4*8 + 5 = 1262
1262: ...

   0: 001 -> 1 =          1 + 001 = 2
   2: 010 -> 2 =       1001 + 001 010 = 19
  19: 100 -> 4 =    1001001 + 001 010 100 = 156
 156: 101 -> 5 = 1001001001 + 001 010 100 101 = 1262
1262: ...     1010010010001
		   1001010010010001
		1001001010010010001
	 1001001001010010010001
  1001001001001010010010001


]

unsigned int  sliceSizes[8] = { 1,8,64,512,4680,32768,262144,2097152 };
unsigned int sliceIndices[8];

sliceIndices[0] = 0;
for (int i = 1; i < 8; i++) {
	sliceIndices[i] = sliceSizes[i - 1] + sliceIndices[i - 1];
}



*/class BitmaskOctree
{

public:
	BitmaskOctree(
		int xCoord, 
		int yCoord, 
		int zCoord, 
		unsigned int maxDepth, 
		unsigned int LODLevel
	);

	BitmaskOctree() = default;

public:
	std::vector<unsigned int> bitMaskArray;
	std::vector<OctreeNode> octreeArray;

private:
	unsigned int   voxelSizes[8] = { 128, 64, 32,  16,    8,     4,      2,       1 };
	unsigned int   sliceSizes[8] = {   1,  8, 64, 512, 4096, 32768, 262144, 2097152 };
	unsigned int sliceIndices[9] = {   0,  1,  9,  73,  585,  5265,  38033,  300177, 2397329 };
	

	SimplexNoise simplexNoiseGenerator = SimplexNoise(8.0f, 3.0f, 3.0f, 0.1f);

	std::vector<std::vector<OctreeNode>> layerArrays;
	std::vector<std::map<OctreeNodeCoord, OctreeNode>> layerMaps;
};

#endif 