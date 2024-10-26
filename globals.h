#pragma once

constexpr float minX = -10.0f;
constexpr float maxX = 30.0f;
constexpr float minZ = -30.0f;
constexpr float maxZ = 30.0f;
constexpr float minY = 0.0f;
constexpr float maxY = 50.0f;

// this is the number of falling physical items. 
constexpr unsigned int boxCount = 1000;
constexpr unsigned int sphereCount = 1000;
constexpr unsigned int octreeDepth = 4;

// these is where the camera is, where it is looking and the bounds of the continaing box. You shouldn't need to alter these
constexpr int LOOKAT_X = 10;
constexpr int LOOKAT_Y = 10;
constexpr int LOOKAT_Z = 50;

constexpr int LOOKDIR_X = 10;
constexpr int LOOKDIR_Y = 0;
constexpr int LOOKDIR_Z = 0;

// gravity - change it and see what happens (usually negative!)
constexpr float gravity = -19.81f;