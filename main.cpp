#include <GL/freeglut.h>
#include <chrono>
#include <iostream>

#include "globals.h"
#include "Vec3.h"
#include "ColliderObject.h"
#include "Box.h"
#include "Sphere.h"

#include "MemoryOperators.h"
#include "MemoryManager.h"

#include "MemoryPoolManager.h"

#include "Timer.h"
#include "TimeLogger.h"
#include "LinkedVector.h"
#include "Octree.h"

using namespace std::chrono;
using ColliderObjs = LinkedVector<ColliderObject*>;

ColliderObjs* sphereColliders = nullptr;
ColliderObjs* boxColliders = nullptr;

unsigned int boxCount = 3000;
unsigned int sphereCount = 3000;
size_t threadCount = 4;
unsigned int octreeDepth = 4;

Octree* octree = nullptr;

// used in the 'mouse' tap function to convert a screen point to a point in the world
Vec3 screenToWorld(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return Vec3((float)posX, (float)posY, (float)posZ);
}

// update the physics: gravity, collision test, collision resolution
void updatePhysics(const float deltaTime) {
    octree->ClearLists();

    ColliderObjs& colliders = *boxColliders;
    for (ColliderObject* box : colliders) { 
        if (box == nullptr) continue;

        box->update(deltaTime);
        octree->Insert(box);
    }
    octree->TestCollisions();
}

// draw the sides of the containing area
void drawQuad(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& v4) {
    glBegin(GL_QUADS);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    glVertex3f(v4.x, v4.y, v4.z);
    glEnd();
}

// draw the entire scene
void drawScene() {
    // Draw the side wall
    GLfloat diffuseMaterial[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);

    // Draw the left side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 leftSideWallV1(minX, minY, maxZ);
    Vec3 leftSideWallV2(minX, maxY, maxZ);
    Vec3 leftSideWallV3(minX, maxY, minZ);
    Vec3 leftSideWallV4(minX, minY, minZ);
    drawQuad(leftSideWallV1, leftSideWallV2, leftSideWallV3, leftSideWallV4);

    // Draw the right side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 rightSideWallV1(maxX, minY, maxZ);
    Vec3 rightSideWallV2(maxX, maxY, maxZ);
    Vec3 rightSideWallV3(maxX, maxY, minZ);
    Vec3 rightSideWallV4(maxX, minY, minZ);
    drawQuad(rightSideWallV1, rightSideWallV2, rightSideWallV3, rightSideWallV4);

    // Draw the back wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 backWallV1(minX, minY, minZ);
    Vec3 backWallV2(minX, maxY, minZ);
    Vec3 backWallV3(maxX, maxY, minZ);
    Vec3 backWallV4(maxX, minY, minZ);
    drawQuad(backWallV1, backWallV2, backWallV3, backWallV4);

    ColliderObjs& colliders = *boxColliders;
    for (ColliderObject* box : colliders) {
        if (box == nullptr) continue;
        box->draw();
    }
}

// called by GLUT - displays the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(LOOKAT_X, LOOKAT_Y, LOOKAT_Z, LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z, 0, 1, 0);

    drawScene();

    glutSwapBuffers();
}

// called by GLUT when the cpu is idle - has a timer function you can use for FPS, and updates the physics
// see https://www.opengl.org/resources/libraries/glut/spec3/node63.html#:~:text=glutIdleFunc
// NOTE this may be capped at 60 fps as we are using glutPostRedisplay(). If you want it to go higher than this, maybe a thread will help here. 
void idle() {
    static auto last = steady_clock::now();
    const duration<float> frameTime = steady_clock::now() - last;
    float deltaTime = frameTime.count();

    TimeLogger::Update(deltaTime);
    last = steady_clock::now();

    updatePhysics(deltaTime);

    // tell glut to draw - note this will cap this function at 60 fps
    glutPostRedisplay();
}

// called the mouse button is tapped
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Get the camera position and direction
        Vec3 cameraPosition(LOOKAT_X, LOOKAT_Y, LOOKAT_Z); // Replace with your actual camera position
        Vec3 cameraDirection(LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z); // Replace with your actual camera direction

        // Get the world coordinates of the clicked point
        Vec3 clickedWorldPos = screenToWorld(x, y);

        // Calculate the ray direction from the camera position to the clicked point
        Vec3 rayDirection = clickedWorldPos - cameraPosition;
        rayDirection.normalise();

        // Perform a ray-box intersection test and remove the clicked box
        float minIntersectionDistance = std::numeric_limits<float>::max();

        ColliderObject* clickedBox = nullptr;
        ColliderObjs& colliders = *boxColliders;
        for (ColliderObject* box : colliders) {

            if (box->rayBoxIntersection(cameraPosition, rayDirection)) {
                // Calculate the distance between the camera and the intersected box
                Vec3 diff = box->position - cameraPosition;
                float distance = diff.length();

                // Update the clicked box index if this box is closer to the camera
                if (distance < minIntersectionDistance) {
                    minIntersectionDistance = distance;
                    clickedBox = box;
                }
            }
        }

        if (clickedBox != nullptr)
        {
            auto end = colliders.end();
            auto it = std::find(colliders.begin(), end, clickedBox);
            if (it != end)
            {
                std::vector<ColliderObject*>& owningVector = it.linkedVec->vector;
                size_t offset = (it.getPtr() - owningVector.data());
                delete clickedBox;
                owningVector.erase(owningVector.begin() + offset);
            }
        }
    }
}

void cleanup()
{
    if (sphereColliders != nullptr && boxColliders != nullptr)
    {
        ColliderObjs& colliders = *boxColliders;
        for (auto it = colliders.begin(); it != colliders.end(); ++it) {
            delete* it;
            *it = nullptr;
        }
        colliders.clear();

        delete sphereColliders;
        sphereColliders = nullptr;
        delete boxColliders;
        boxColliders = nullptr;
    }

    if (octree != nullptr)
    {
        delete octree;
        octree = nullptr;
    }

    TimeLogger::Destroy();

#ifdef _DEBUG
    MemoryManager::Cleanup();
#endif
    MemoryPoolManager::Cleanup();
}

// called when the keyboard is used
void keyboard(unsigned char key, int x, int y) {
    const float impulseMagnitude = 20.0f; // Upward impulse magnitude

    static int* intPtr = nullptr;

    switch (key)
    {
    case ' ': // make colliders jump
    {
        ColliderObjs& colliders = *boxColliders;
        for (ColliderObject* box : colliders) {
            box->velocity.y += impulseMagnitude;
        }
    }
        break;
#ifdef _DEBUG
    case 'm': // display memory allocation info
        MemoryManager::OutputAllocations();
        MemoryPoolManager::PrintPoolDebugInfo();
        break;
    case 'w':
        std::cout << "\nWalking the heap:" << std::endl;
        MemoryManager::WalkHeap();
        break;
#endif
    case 'q': // quits glut main loop (freeglut)
        glutLeaveMainLoop();
        break;
    case 't': // allocates memory with global new
        if (!intPtr) {
            intPtr = new int[10];
            std::cout << "Created array of 10 ints on heap" << std::endl;
        }
        break;
    case 'u': // deallocates memory with global delete
        delete[] intPtr;
        intPtr = nullptr;
        std::cout << "Called delete on ptr to 10 int array" << std::endl;
        break;
    case 'f':
        if (intPtr != nullptr)
        {
            void* toCopy = std::malloc(11 * 4);
            std::memcpy(intPtr, toCopy, 11 * 4);
            std::cout << "Memory corrupted" << std::endl;
            std::free(toCopy);
        }
        else
        {
            std::cout << "Press t first to allocate memory to be corrupted!" << std::endl;
        }
        break;
    case 'h':
        if (intPtr != nullptr)
        {
            void* toCopy = std::malloc(11 * 4);
            std::memcpy(intPtr-5, toCopy, 11 * 4);
            std::cout << "Memory corrupted" << std::endl;
            std::free(toCopy);
        }
        else
        {
            std::cout << "Press t first to allocate memory to be corrupted!" << std::endl;
        }
        break;
    case 'r':
    {
        std::vector<ColliderObject*>& boxes = boxColliders->vector;
        delete boxes.back();
        boxes.pop_back();
        std::cout << "Removed Box" << std::endl;
    }
    if (boxCount != 0) --boxCount;
        break;
    case 'a':
    {
        ColliderObject* box = ColliderObject::createCollider<Box>();
        boxColliders->vector.emplace_back(box);
        std::cout << "Added Box" << std::endl;
    }
    ++boxCount;
        break;
    case 'R':
    {
        std::vector<ColliderObject*>& spheres = sphereColliders->vector;
        delete spheres.back();
        spheres.pop_back();
        std::cout << "Removed Sphere" << std::endl;
    }
    if (sphereCount != 0) --sphereCount;
        break;
    case 'A':
    {
        ColliderObject* sphere = ColliderObject::createCollider<Sphere>();
        sphereColliders->vector.emplace_back(sphere);
        std::cout << "Added Sphere" << std::endl;
    }
    ++sphereCount;
        break;
    }
}

void initGlut(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    int handle = glutCreateWindow("Simple Physics Simulation");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
}

void initOpenGl()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void initScene(int boxCount, int sphereCount)
{
    sphereColliders = new ColliderObjs(sphereCount);
    boxColliders = new ColliderObjs(sphereColliders, boxCount);

    octree = new Octree(
        Vec3((maxX - minX) / 2.0f, (maxY - minY) / 2.0f, (maxZ - minZ) / 2.0f),
        Vec3(maxX - minX, maxZ - minZ, maxZ - minZ),
        octreeDepth
    );

    for (int i = 0; i < boxCount; ++i) {
        (*boxColliders)[i] = ColliderObject::createCollider<Box>();
    }

    for (int i = 0; i < sphereCount; ++i) {
        (*sphereColliders)[i] = ColliderObject::createCollider<Sphere>();
    }
}

int getConstants()
{
    std::cout << "Number of spheres: ";
    std::cin >> sphereCount;
    std::cout << "Number of cubes: ";
    std::cin >> boxCount;
    std::cout << "Octree depth: ";
    std::cin >> octreeDepth;
    if (octreeDepth >= maxOctantDepth)
    {
        return 1;
    }
    std::cout << "Thread count: ";
    std::cin >> threadCount;
    if (threadCount == 0)
    {
        return 2;
    }
    MemoryPoolManager::Init();
    return 0;
}

// the main function. 
int main(int argc, char** argv) {
    int ret = getConstants();
    switch(ret)
    {
    case 1:
        std::cout << "\nOctree depth too large, exiting!" << std::endl;
        return 0;
    case 2:
        std::cout << "\Thread count must not be 0, exiting!" << std::endl;
        return 0;
    }

    srand(static_cast<unsigned>(time(0))); // Seed random number generator
    initGlut(argc, argv);
    initOpenGl();
    
    TimeLogger::Init();

    {
        Timer<std::chrono::steady_clock, std::milli> timer{};
        initScene(boxCount, sphereCount);
        TimeLogger::LogInit(timer.Elapsed());
    }

    // it will stick here until the program ends. 
    glutMainLoop();

    // cleans up resources in the correct order
    cleanup();
    return 0;
}