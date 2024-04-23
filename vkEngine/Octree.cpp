#include "Octree.h"

Octree::Octree(std::vector<Voxel> voxelArray, int depth)
{
    bitMaskArray.resize(19174545);

    int layerSize = 2;

    for (int layer = 0; layer < depth; layer++) {
        if (layer == 0) layerMaps.push_back(bottomLayer(voxelArray, layerSize));
        else layerMaps.push_back(middleLayer(layerMaps[layer - 1], layerSize));

        layerSize *= 2;
    }
    layerArrays.push_back(layerArray(layerMaps[depth - 1]));


    int pointerOffset = 0;
    for (int layer = depth - 1; layer >= 0; layer--) {
        pointerOffset += (int)layerArrays[layer].size();

        for (auto const& val : layerArrays[layer]) {
            OctreeNode curNode = val;
            if (curNode.p0 >= 0) curNode.p0 += pointerOffset;
            if (curNode.p1 >= 0) curNode.p1 += pointerOffset;
            if (curNode.p2 >= 0) curNode.p2 += pointerOffset;
            if (curNode.p3 >= 0) curNode.p3 += pointerOffset;
            if (curNode.p4 >= 0) curNode.p4 += pointerOffset;
            if (curNode.p5 >= 0) curNode.p5 += pointerOffset;
            if (curNode.p6 >= 0) curNode.p6 += pointerOffset;
            if (curNode.p7 >= 0) curNode.p7 += pointerOffset;

            octreeArray.push_back(curNode);
        }
    }
}

std::map<OctreeNodeCoord, OctreeNode> Octree::bottomLayer(std::vector<Voxel> voxelArray, int layerSize)
{
    std::map<OctreeNodeCoord, OctreeNode> layerMap;

    for (size_t i = 0; i < voxelArray.size(); i++) {
        Voxel curVoxel = voxelArray[i];
        OctreeNodeCoord voxelNodeCoord{};
        voxelNodeCoord.x = layerSize * (int)std::floor(curVoxel.x / layerSize);
        voxelNodeCoord.y = layerSize * (int)std::floor(curVoxel.y / layerSize);
        voxelNodeCoord.z = layerSize * (int)std::floor(curVoxel.z / layerSize);

        OctreeNode newNode{};

        if (layerMap.find(voxelNodeCoord) == layerMap.end()) {
            newNode = {
                -1,-1,-1,-1,-1,-1,-1,-1
            };
        }
        else {
            newNode = layerMap[voxelNodeCoord];
        }

        int x = 0;
        int y = 0;
        int z = 0;
        if (curVoxel.x % layerSize != 0) x = 1;
        if (curVoxel.y % layerSize != 0) y = 1;
        if (curVoxel.z % layerSize != 0) z = 1;

        if (x == 0) {
            if (y == 0) {
                if (z == 0) newNode.p0 = curVoxel.mat;     // 000
                else newNode.p1 = curVoxel.mat;            // 001
            }
            else {
                if (z == 0) newNode.p2 = curVoxel.mat;     // 010
                else newNode.p3 = curVoxel.mat;            // 011
            }
        }
        else {
            if (y == 0) {
                if (z == 0) newNode.p4 = curVoxel.mat;     // 100
                else newNode.p5 = curVoxel.mat;            // 101
            }
            else {
                if (z == 0) newNode.p6 = curVoxel.mat;     // 110
                else newNode.p7 = curVoxel.mat;            // 111
            }
        }
        layerMap[voxelNodeCoord] = newNode;
    }
    return layerMap;
}

std::map<OctreeNodeCoord, OctreeNode> Octree::middleLayer(std::map<OctreeNodeCoord, OctreeNode> prevLayerMap, int layerSize)
{
    std::map<OctreeNodeCoord, OctreeNode> layerMap;
    std::vector<OctreeNode> prevLayerArray;

    int i = 0;
    for (auto const& [key, val] : prevLayerMap)
    {
        OctreeNodeCoord curCoord = key;
        OctreeNode curNode = val;

        if (val.p0 == val.p1 &&
            val.p1 == val.p2 &&
            val.p2 == val.p3 &&
            val.p3 == val.p4 &&
            val.p4 == val.p5 &&
            val.p5 == val.p6 &&
            val.p5 == val.p7) {
            i = val.p0;
        }
        else {
            prevLayerArray.push_back(val);
            i = (int)prevLayerArray.size() - 1;
        }

        OctreeNodeCoord voxelNodeCoord{};
        voxelNodeCoord.x = layerSize * (int)std::floor(curCoord.x / layerSize);
        voxelNodeCoord.y = layerSize * (int)std::floor(curCoord.y / layerSize);
        voxelNodeCoord.z = layerSize * (int)std::floor(curCoord.z / layerSize);

        OctreeNode newNode{};

        if (layerMap.find(voxelNodeCoord) == layerMap.end()) {
            newNode = {
                -1,-1,-1,-1,-1,-1,-1,-1
            };
        }
        else {
            newNode = layerMap[voxelNodeCoord];
        }

        int x = 0;
        int y = 0;
        int z = 0;
        if (curCoord.x % layerSize != 0) x = 1;
        if (curCoord.y % layerSize != 0) y = 1;
        if (curCoord.z % layerSize != 0) z = 1;

        if (x == 0) {
            if (y == 0) {
                if (z == 0) newNode.p0 = i;
                else newNode.p1 = i;
            }
            else {
                if (z == 0) newNode.p2 = i;
                else newNode.p3 = i;
            }
        }
        else {
            if (y == 0) {
                if (z == 0) newNode.p4 = i;
                else newNode.p5 = i;
            }
            else {
                if (z == 0) newNode.p6 = i;
                else newNode.p7 = i;
            }
        }
        layerMap[voxelNodeCoord] = newNode;
        i++;
    }
    layerArrays.push_back(prevLayerArray);
    return layerMap;
}

std::vector<OctreeNode> Octree::layerArray(std::map<OctreeNodeCoord, OctreeNode> layerMap)
{
    std::vector<OctreeNode> layerArray;
    for (auto const& [key, val] : layerMap)
    {
        layerArray.push_back(val);
    }
    return layerArray;
}
