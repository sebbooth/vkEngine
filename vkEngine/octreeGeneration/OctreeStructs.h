#pragma once
#ifndef OCTREE_STRUCTS_H
#define OCTREE_STRUCTS_H

#include <vector>

struct OctreeNode {
    int p0;
    int p1;
    int p2;
    int p3;
    int p4;
    int p5;
    int p6;
    int p7;
};

struct OctreeChunk {
    int x;
    int y;
    int z;
    std::vector<OctreeNode> tree;
};

struct OctreeSSBO {
    std::vector<OctreeChunk> buffer;
};

struct OctreeNodeCoord {
    int x;
    int y;
    int z;

    bool operator==(const OctreeNodeCoord& other) const {
        if (x != other.x)
            return false;
        if (y != other.y)
            return false;
        if (z < other.z)
            return false;
        return true;
    }

    bool operator<(const OctreeNodeCoord& other) const {
        if (x < other.x)
            return true;
        if (x > other.x)
            return false;
        if (y < other.y)
            return true;
        if (y > other.y)
            return false;
        return z < other.z;
    }
};

struct Voxel {
    int x;
    int y;
    int z;
    int mat;
};

#endif