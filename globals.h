#pragma once

constexpr float minX = -10.0f;
constexpr float maxX = 30.0f;
constexpr float minZ = -30.0f;
constexpr float maxZ = 30.0f;
constexpr float minY = 0.0f;
constexpr float maxY = 50.0f;

// this is the number of falling physical items. 
extern unsigned int boxCount;
extern unsigned int sphereCount;
extern size_t threadCount;
extern unsigned int octreeDepth;

constexpr unsigned int maxOctantDepth = 10;

constexpr size_t chunkSize = 100;
constexpr size_t chunkCount = 10;


// these is where the camera is, where it is looking and the bounds of the continaing box. You shouldn't need to alter these
constexpr int LOOKAT_X = 10;
constexpr int LOOKAT_Y = 10;
constexpr int LOOKAT_Z = 50;

constexpr int LOOKDIR_X = 10;
constexpr int LOOKDIR_Y = 0;
constexpr int LOOKDIR_Z = 0;

// gravity - change it and see what happens (usually negative!)
constexpr float gravity = -19.81f;