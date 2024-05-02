#include "BitmaskOctree.h"

BitmaskOctree::BitmaskOctree(int xCoord, int yCoord, int zCoord, unsigned int maxDepth, unsigned int LODLevel)
{
    const unsigned int octreeWidth = static_cast<unsigned int>(pow(2, maxDepth));
    const unsigned int voxelWidth = static_cast<unsigned int>(pow(2, LODLevel));
    const unsigned int octreeWidthInVoxels = octreeWidth / voxelWidth;

    std::cout << "Octree Width: " << octreeWidth << std::endl;
    std::cout << "Voxel Width: " << voxelWidth << std::endl;

    bitMaskArray.resize(sliceIndices[maxDepth - LODLevel + 1], 0);

    Perlin perlinGenerator;
    perlinGenerator.layers = maxDepth - 1;
    perlinGenerator.init_amp = 4;
    perlinGenerator.grid_size = octreeWidth * 8;

    SimplexNoise simplexNoiseGenerator = SimplexNoise(8.0f, 3.0f, 3.0f, 0.1f);

    std::random_device rd;  // Seed for random number engine
    std::mt19937 gen(rd()); // Mersenne Twister random number engine
    std::uniform_int_distribution<> distribution(-int(octreeWidth) / 20, octreeWidth / 20); // Range from 1 to 100

    float perlinValMax = 0;
    float perlinValMin = 100;

    for (int x = 0; x < octreeWidth; x += voxelWidth) {
        for (int z = 0; z < octreeWidth; z += voxelWidth) {
            float perlinVal = perlinGenerator.perlin2D(x + xCoord, z + zCoord);
            int yThreshold = (int)(perlinVal * octreeWidth);

            perlinValMax = std::max(perlinValMax, perlinVal);
            perlinValMin = std::min(perlinValMin, perlinVal);



            for (int y = 0; y < octreeWidth; y += voxelWidth) {
                //float simplex = simplexNoiseGenerator.fractal(5, (float)x / octreeWidth, (float)y / octreeWidth, (float)z / octreeWidth);
                

                if ( y + yCoord < yThreshold) {
                    unsigned int voxelIndex = 0b000;
                    unsigned int curDepth = 0;
                    int curVoxelSize = octreeWidth;
                    while (curDepth++ < (maxDepth - LODLevel)) {
                        curVoxelSize /= 2;
                        
                        if (static_cast<unsigned int>(floor(x / curVoxelSize)) % 2 == 1) {
                            voxelIndex = voxelIndex + 0b100;
                        }
                        if (static_cast<unsigned int>(floor(y / curVoxelSize)) % 2 == 1) {
                            voxelIndex = voxelIndex + 0b010;
                        }
                        if (static_cast<unsigned int>(floor(z / curVoxelSize)) % 2 == 1) {
                            voxelIndex = voxelIndex + 0b001;
                        }
                        voxelIndex = voxelIndex << 3;

                    }                    
                    voxelIndex = voxelIndex >> 3;
                    //std::cout << "voxelIndex = " << std::bitset<24>(voxelIndex) << std::endl;

                    unsigned int finalIndex = sliceIndices[maxDepth - LODLevel] + voxelIndex;

                    //std::cout << "X: " << x << " Y: " << y << " Z: " << z << " index: " << voxelIndex << std::endl;
                    unsigned int mat = 1; // grass
                    int randomDist = distribution(gen);

                    if (y + yCoord < yThreshold - 6) {
                        mat = 2;// stone
                    }
                    if (y + yCoord > randomDist + (octreeWidth * 0.55)) {
                        mat = 2; // stone
                    }
                    if (y + yCoord > randomDist + (octreeWidth * 0.70)) {
                        mat = 3; //snow
                    }

                    bitMaskArray[finalIndex] = mat << 8;
                    //std::cout << "Depth: " << maxDepth << " voxel = " << std::bitset<24>(bitMaskArray[finalIndex]) << std::endl;
                }
            }
        }
    }

    std::cout << perlinValMax << std::endl;
    std::cout << perlinValMin << std::endl;

    int curDepth = maxDepth - LODLevel - 1;

    while (curDepth >= 0) {
        for (int i = sliceSizes[curDepth] - 1; i >= 0 ; i--) {
            unsigned int curNode = 0b00;
            unsigned int childrenIndex = (i << 3) + sliceIndices[curDepth + 1];
            std::unordered_map<int, int> mats;

            for (int j = 7; j >= 0; j--) {
                int child = bitMaskArray[childrenIndex + 7 - j];
                int mat = child >> 8;
                if ((child & 255) == 0) {
                    mats[mat]++;
                }
                if (child > 0) {
                    curNode += 0b1;
                } 
                curNode = curNode << 1;
            }
            curNode = curNode >> 1;

            int max_mat = -1;
            int max_value = -1;
            for (const auto& pair : mats) {
                if (pair.second > max_value) {
                    max_value = pair.second;
                    max_mat = pair.first;
                }
            }

            

            if (max_value == 8) {
                curNode = (max_mat << 8);
            }

            
            bitMaskArray[i + sliceIndices[curDepth]] = curNode;
            /*
            std::cout 
                << " Cur Index: " << (i + sliceIndices[curDepth])
                << " Child index: " << ((i * 8) + sliceIndices[curDepth + 1])
                << " ChildMask: " << std::bitset<8>(curNode&0b11111111) 
                << " Material = " << std::bitset<4>((curNode&0b111111111111100000000) >> 8) 
                << std::endl;
            */
        }
        curDepth--;
    }
    

    std::cout << "Donezo size: " << sizeof(unsigned int) * bitMaskArray.size() << "\n";

}
