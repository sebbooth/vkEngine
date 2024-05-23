layout(binding = 0) uniform ParameterUBO {
    vec3 camPos;
    vec3 camDir;
    vec3 camUp;
    vec3 sunDir;
    vec3 fogColor;

    float fogMinDistance;
    float fogMaxDistance;
    float depthPassCorrection;

    float deltaTime;
    float focalLength;
    float viewportHeight;
    float rayEpsilon;

    int screenWidth;
    int screenHeight;

    int octreeSize;
    int octreeMaxDepth;

	int minX;
	int maxX;
	int minY;
	int maxY;
	int minZ;
	int maxZ;

    int numChunks;

    uint showDepth;
	uint showNormals;
    uint findChunkMode;
    uint useInitialDepthPass;
} ubo;

layout(std430, binding = 1) readonly buffer OctreeSSBO {
    uint octreeArray[];
} octree;


layout(std430, binding = 2) readonly buffer ChunkHashTableSSBO {
    int chunkHashTable[];
} chunkHashTable;

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

// independant constants (no dependence on UBO)
const float INF = 1.0 / 0.0;
const int sliceIndices[9] = { 0,  1,  9,  73,  585,  5265,  38033,  300177, 2397329 };
const float ambient = 0.4;
const vec3 normals[6] = {
    vec3( 1,  0,  0),
    vec3( 1,  0,  0),
    vec3( 0,  1,  0),
    vec3( 0,  1,  0),
    vec3( 0,  0,  1),
    vec3( 0,  0,  1)
};
const vec3 materials[3] = {
    vec3(0, 1, 0),
    vec3(0.4, 0.4, 0.4),
    vec3(1, 1, 1)
};


float aspectRatio;
float viewportWidth;
float viewportV;
float viewportU;
vec3 camRight;
vec3 viewportCenter;
vec3 botLeft;
float EPSILON;
int CHUNK_SIZE;
vec3 sunDir;
int screenX;
int screenY;
ivec2 texPos;
vec3 pixelCenter;
vec3 rayDir;
vec3 curPos;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
        vec2(12.9898,78.233)))*
        43758.5453123);
}

bool pointInIVoxel(vec3 point, ivec3 iVoxel, int iVoxelSize) {
    vec3 fIVoxel = vec3(iVoxel);
    return (point.x >= fIVoxel.x && point.x <= (fIVoxel.x + iVoxelSize) &&
            point.y >= fIVoxel.y && point.y <= (fIVoxel.y + iVoxelSize) &&
            point.z >= fIVoxel.z && point.z <= (fIVoxel.z + iVoxelSize));
}

ivec3 roundVoxel(vec3 origVoxel, int voxelLength) {
    ivec3 scaledVoxel;
    scaledVoxel.x = voxelLength * int(floor(origVoxel.x / float(voxelLength)));
    scaledVoxel.y = voxelLength * int(floor(origVoxel.y / float(voxelLength)));
    scaledVoxel.z = voxelLength * int(floor(origVoxel.z / float(voxelLength)));
    return scaledVoxel;
}

// returns vec ( deltaX, deltaY, deltaZ)
vec3 findDeltas(vec3 position, vec3 rayDir, int voxelLength, vec3 prevDeltas) {
    float nextIntersect;
    vec3 deltas = prevDeltas;

    if (deltas.x < 0) {
        if (rayDir.x > 0) {
            nextIntersect = voxelLength * (floor(position.x / voxelLength) + 1);
            deltas.x = (nextIntersect - position.x) / rayDir.x;
        }
        else if (rayDir.x < 0) {
            nextIntersect = voxelLength * (floor(position.x / voxelLength));
            deltas.x = (nextIntersect - position.x) / rayDir.x;
        }
    }
    
    if (deltas.y < 0) {
        if (rayDir.y > 0) {
            nextIntersect = voxelLength * (floor(position.y / voxelLength) + 1);
            deltas.y = (nextIntersect - position.y) / rayDir.y;
        }
        else if (rayDir.y < 0) {
            nextIntersect = voxelLength * (floor(position.y / voxelLength));
            deltas.y = (nextIntersect - position.y) / rayDir.y;
        }
    }

    if (deltas.z < 0) {
        if (rayDir.z > 0) {
            nextIntersect = voxelLength * (floor(position.z / voxelLength) + 1);
            deltas.z = (nextIntersect - position.z) / rayDir.z;
        }
        else if (rayDir.z < 0){
            nextIntersect = voxelLength * (floor(position.z / voxelLength));
            deltas.z = (nextIntersect - position.z) / rayDir.z;
        }
    }

    return deltas;
}

// returns vec4 ( vec3(intersectionPoint), intersection type )
vec4 findNextIntersect(vec3 position, vec3 rayDir, int voxelLength) {
    float nextIntersect;
    int intersectionType;

    float curDelta;
    float minDelta = INF;

    if (rayDir.x > 0) {
        nextIntersect = voxelLength * (floor(position.x / voxelLength) + 1);
        minDelta = (nextIntersect - position.x) / rayDir.x;
        intersectionType = 0;
    }

    else if (rayDir.x < 0) {
        nextIntersect = voxelLength * (floor(position.x / voxelLength));
        minDelta = (nextIntersect - position.x) / rayDir.x;
        intersectionType = 1;
    }


    if (rayDir.y > 0) {
        nextIntersect = voxelLength * (floor(position.y / voxelLength) + 1);
        curDelta = (nextIntersect - position.y) / rayDir.y;
        if (curDelta < minDelta) {
            minDelta = curDelta;
            intersectionType = 2;
        }
    }
    else if (rayDir.y < 0) {
        nextIntersect = voxelLength * (floor(position.y / voxelLength));
        curDelta = (nextIntersect - position.y) / rayDir.y;
        if (curDelta < minDelta) {
            minDelta = curDelta;
            intersectionType = 3;
        }
    }

    if (rayDir.z > 0) {
        nextIntersect = voxelLength * (floor(position.z / voxelLength) + 1);
        curDelta = (nextIntersect - position.z) / rayDir.z;
        if (curDelta < minDelta) {
            minDelta = curDelta;
            intersectionType = 4;
        }
    }
    else if (rayDir.z < 0){
        nextIntersect = voxelLength * (floor(position.z / voxelLength));
        curDelta = (nextIntersect - position.z) / rayDir.z;
        if (curDelta < minDelta) {
            minDelta = curDelta;
            intersectionType = 5;
        }
    }

    return vec4(vec3(position + (minDelta * rayDir)), intersectionType);
}

// returns ivec2(material index, intersection type, depth, randFloat)
vec4 traverseOctree(vec3 curPos, vec3 rayDir, int intersectionType, ivec3 octreePosition, int chunkIndex) {
    ivec3 parentVoxels[8];
    parentVoxels[0] = octreePosition;
    
    // descend to leaf node
    int nodeIndex = 0;
    int curDepth = 0;
    int nodeSize = CHUNK_SIZE;
    uint curNode;

    int maxLoops = 1000;
    while (maxLoops-- > 0) {
        // check if position is in parent voxel
        if (!pointInIVoxel(curPos, octreePosition, CHUNK_SIZE)) {
            return vec4(-1,-1,-1, 1);
        }

   
        curNode = octree.octreeArray[chunkIndex + nodeIndex + sliceIndices[curDepth]];
        

        if ((curNode & uint(255)) == 0) {
            if ((curNode >> 8) == 0) {
                return vec4(-1,-1,-1, 1);
            }
            ivec3 curVoxel = roundVoxel(curPos, 1);
            float randFloat = 0.33 * (random(vec2(curVoxel.xy)) + random(vec2(curVoxel.yz)) + random(vec2(curVoxel.xz)));

            return vec4(
                        (curNode >> 8), 
                        intersectionType, 
                        dot((ubo.camPos-curPos), (ubo.camPos-curPos)),
                        randFloat
            );

        }

        // else check deeper
        curDepth += 1;
        nodeSize = CHUNK_SIZE >> curDepth;
        ivec3 curVoxel = roundVoxel(curPos, nodeSize);
        ivec3 parentVoxel = parentVoxels[curDepth - 1];
        //parentVoxel = roundVoxel(curPos, nodeSize * 2);
        
        nodeIndex = nodeIndex << 3;

        if (curVoxel.x != parentVoxel.x) {
            nodeIndex += 4;
        }
            
        if (curVoxel.y != parentVoxel.y) {
            nodeIndex += 2;
        }

        if (curVoxel.z != parentVoxel.z) {
            nodeIndex += 1;
        }

        parentVoxels[curDepth] = curVoxel;
          
        if ((curNode & (128 >> (nodeIndex & 7))) == 0) {
            // traverse
            vec4 nextIntersect = findNextIntersect(curPos, rayDir, nodeSize);
            intersectionType = int(nextIntersect.a);
            curPos = nextIntersect.xyz;
            if (intersectionType < 2) {
                if (intersectionType < 1) {
                    curPos.x += EPSILON;
                }
                else {
                    curPos.x -= EPSILON;
                }
            }
            else if (intersectionType < 4) {
                if (intersectionType < 3) {
                    curPos.y += EPSILON;
                }
                else {
                    curPos.y -= EPSILON;
                }
            }
            else {
                if (intersectionType < 5) {
                    curPos.z += EPSILON;
                }
                else {
                    curPos.z -= EPSILON;
                }
            }

            while (curDepth > 0) {
                nodeSize = CHUNK_SIZE >> curDepth;
                if (!(pointInIVoxel(curPos, parentVoxels[curDepth], nodeSize))) {
                    nodeIndex = nodeIndex >> 3;
                    curDepth -= 1;
                }
                else {
                    break;
                }
            }
        }
    }
    return vec4(-1,-2,-1, 1);
}

bool rayHasLeftBounds(vec3 rayDir, vec3 rayPosition) {
    return (
        ( (rayDir.x > 0) && rayPosition.x > ubo.maxX) ||
        (!(rayDir.x > 0) && rayPosition.x < ubo.minX) ||
        ( (rayDir.y > 0) && rayPosition.y > ubo.maxY) ||
        (!(rayDir.y > 0) && rayPosition.y < ubo.minY) ||
        ( (rayDir.z > 0) && rayPosition.z > ubo.maxZ) ||
        (!(rayDir.z > 0) && rayPosition.z < ubo.minZ)
    );
}

bool pointInBounds(vec3 point) {
    return (
        point.x <= ubo.maxX &&
        point.x >= ubo.minX &&
        point.y <= ubo.maxY &&
        point.y >= ubo.minY &&
        point.z <= ubo.maxZ &&
        point.z >= ubo.minZ
    );
}

vec4 findChunk(vec3 rayOrigin, vec3 rayDir, int maxChecks) {
    
    if (rayHasLeftBounds(rayDir, rayOrigin)){
            return vec4(0,-1,0, 1);
    } 
    
    ivec3 curVoxel = roundVoxel(rayOrigin, CHUNK_SIZE);

    if (pointInBounds(rayOrigin)) {
        int chunkHash = int(mod(curVoxel.x >> 7, 128)) * (16 * 128) + int(mod(curVoxel.y >> 7, 16)) * (128) + int(mod(curVoxel.z >> 7, 128));
        int chunkIndex = chunkHashTable.chunkHashTable[chunkHash];
        debugPrintfEXT("ChunkHash is %i", chunkHash);
        debugPrintfEXT("ChunkIndex is %i", chunkIndex);
        debugPrintfEXT("x is %i", curVoxel.x);
        debugPrintfEXT("y is %i", curVoxel.y);
        debugPrintfEXT("z is %i", curVoxel.z);

        if (chunkIndex > -1) {
            vec4 traversal = traverseOctree(rayOrigin, rayDir, 0, curVoxel, chunkIndex);
            if (traversal.y > -1) {
                return traversal;
            }        
        }
    }

    float xDelta = abs(CHUNK_SIZE / rayDir.x);
    float yDelta = abs(CHUNK_SIZE / rayDir.y);
    float zDelta = abs(CHUNK_SIZE / rayDir.z);

    float stepSize;

    int intersectionType;
    vec3 prevEpsilon = vec3(0,0,0);
    vec3 curEpsilon = vec3(0,0,0);
    vec3 curPos = rayOrigin;
    vec3 curDeltas = findDeltas(curPos, rayDir, CHUNK_SIZE, vec3(-1,-1,-1));

    float curXDelta = curDeltas.x;
    float curYDelta = curDeltas.y;
    float curZDelta = curDeltas.z;

    while (maxChecks > 0) {
        maxChecks -= 1;

        if (curXDelta < curYDelta) {
            if (curXDelta < curZDelta) {
                stepSize =  curXDelta;
                curXDelta = xDelta;
                curYDelta -= stepSize;
                curZDelta -= stepSize;
                if ((rayDir.x > 0)) {
                    intersectionType = 0;
                    curEpsilon = vec3(EPSILON, 0, 0);
                    curVoxel.x += CHUNK_SIZE;
                }
                else {
                    intersectionType = 1;
                    curEpsilon = vec3(-EPSILON, 0, 0);
                    curVoxel.x -= CHUNK_SIZE;
                }
            }
            else {
                stepSize =  curZDelta;
                curXDelta -= stepSize;
                curYDelta -= stepSize;
                curZDelta = zDelta;
                if ((rayDir.z > 0)) {
                    intersectionType = 4;
                    curEpsilon = vec3(0, 0, EPSILON);
                    curVoxel.z += CHUNK_SIZE;
                }
                else {
                    intersectionType = 5;
                    curEpsilon = vec3(0, 0, -EPSILON);
                    curVoxel.z -= CHUNK_SIZE;
                }
            }
        }
        else if (curYDelta < curZDelta) {
            stepSize = curYDelta;
            curXDelta -= stepSize;
            curYDelta = yDelta;
            curZDelta -= stepSize;
            if ((rayDir.y > 0)) {
                intersectionType = 2;
                curEpsilon = vec3(0, EPSILON, 0);
                curVoxel.y += CHUNK_SIZE;
            }
            else {
                intersectionType = 3;
                curEpsilon = vec3(0, -EPSILON, 0);
                curVoxel.y -= CHUNK_SIZE;
            }
        }
        else {
            stepSize =  curZDelta;
            curXDelta -= stepSize;
            curYDelta -= stepSize;
            curZDelta = zDelta;
            if ((rayDir.z > 0)) {
                intersectionType = 4;
                curEpsilon = vec3(0, 0, EPSILON);
                curVoxel.z += CHUNK_SIZE;
            }
            else {
                intersectionType = 5;
                curEpsilon = vec3(0, 0, -EPSILON);
                curVoxel.z -= CHUNK_SIZE;
            }
        }

        curPos += (stepSize * rayDir);
        curPos += curEpsilon;

        if (pointInBounds(curPos)) {
            int chunkHash = int(mod(curVoxel.x >> 7, 128)) * (16 * 128) + int(mod(curVoxel.y >> 7, 16)) * (128) + int(mod(curVoxel.z >> 7, 128));
            int chunkIndex = chunkHashTable.chunkHashTable[chunkHash];
            debugPrintfEXT("ChunkHash is %i", chunkHash);
            debugPrintfEXT("ChunkIndex is %i", chunkIndex);
            debugPrintfEXT("x is %i", curVoxel.x);
            debugPrintfEXT("y is %i", curVoxel.y);
            debugPrintfEXT("z is %i", curVoxel.z);

            if (chunkIndex > -1) {
                vec4 traversal = traverseOctree(curPos, rayDir, intersectionType, curVoxel, chunkIndex);
                if (traversal.y > -1) {
                    return traversal;
                }        
            }
        }
        else if (rayHasLeftBounds(rayDir, curPos)){
            return vec4(0,-1,0, 1);
        }
        curPos -= curEpsilon;
    }
    return vec4(0,-1, 0, 1);
}

vec3 outputColor(vec4 intersection, float lightings[6]) {
    vec3 color;
    float fogFactor;

    if (intersection.y >= 0) {  // collision
        fogFactor = min(1,(sqrt(intersection.z) / ubo.fogMaxDistance));
        if (ubo.showDepth == 1) {
            color = vec3(1,1,1) * fogFactor;
            
        }
        else if (ubo.showNormals == 1){
            color = normals[int(intersection.y)];
        } 
        else {
            color = materials[int(intersection.x) - 1] * lightings[int(intersection.y)];
            color -= intersection.a * vec3(0.2, 0.2, 0.2);
            color *= (1 - fogFactor);
            color += ubo.fogColor * fogFactor;
        }
    }
    else {  // miss
        if (ubo.showDepth == 1) {
            color = vec3(1,1,1);
        }
        else {
            if (dot(rayDir, -sunDir) > 0.999) {
                color = vec3(1,1,1);
            }
            else {
                color = vec3(0.6,0.6,0.6) * dot(rayDir, -sunDir) + vec3(0,0,0.3);                
            }
            color = ubo.fogColor;

        }
    }
    
    return color;
}

vec3 depthColor(vec4 intersection) {
    vec3 depthColor;

    if (intersection.y >= 0) {
        float fogFactor = min(1,(sqrt(intersection.z) / ubo.fogMaxDistance));
        depthColor = vec3(1,1,1) * fogFactor;
    }
    else {
        depthColor = vec3(1,1,1);
    }
    return depthColor; 
}