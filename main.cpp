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
#include "LinkedVector.h"

using namespace std::chrono;

// this is the number of falling physical items. 
constexpr unsigned int boxCount = 50;
constexpr unsigned int sphereCount = 50;

// these is where the camera is, where it is looking and the bounds of the continaing box. You shouldn't need to alter these
constexpr int LOOKAT_X = 10;
constexpr int LOOKAT_Y = 10;
constexpr int LOOKAT_Z = 50;

constexpr int LOOKDIR_X = 10;
constexpr int LOOKDIR_Y = 0;
constexpr int LOOKDIR_Z = 0;

using ColliderObjs = LinkedVector<ColliderObject*>;

ColliderObjs* sphereColliders = new ColliderObjs(sphereCount);
ColliderObjs* boxColliders = new ColliderObjs(sphereColliders, boxCount);

// Fine as box colliders only deleted at end of program
ColliderObjs& colliders = *boxColliders;

template <class ColliderType>
ColliderObject* initColliderObject()
{
    ColliderObject* obj = new ColliderType();

    // Assign random x, y, and z positions within specified ranges
    obj->position.x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));
    obj->position.y = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
    obj->position.z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));

    obj->size = { 1.0f, 1.0f, 1.0f };

    // Assign random x-velocity between -1.0f and 1.0f
    float randomXVelocity = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
    obj->velocity = { randomXVelocity, 0.0f, 0.0f };

    // Assign a random color to the box
    obj->colour.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    obj->colour.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    obj->colour.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    return obj;
}

void initScene(int boxCount, int sphereCount) {
    for (int i = 0; i < boxCount; ++i) {
        (*boxColliders)[i] = initColliderObject<Box>();
    }

    for (int i = 0; i < sphereCount; ++i) {
        (*sphereColliders)[i] = initColliderObject<Sphere>();
    }
}

// a ray which is used to tap (by default, remove) a box - see the 'mouse' function for how this is used.
bool rayBoxIntersection(const Vec3& rayOrigin, const Vec3& rayDirection, const ColliderObject* box) {
    float tMin = (box->position.x - box->size.x / 2.0f - rayOrigin.x) / rayDirection.x;
    float tMax = (box->position.x + box->size.x / 2.0f - rayOrigin.x) / rayDirection.x;

    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (box->position.y - box->size.y / 2.0f - rayOrigin.y) / rayDirection.y;
    float tyMax = (box->position.y + box->size.y / 2.0f - rayOrigin.y) / rayDirection.y;

    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax))
        return false;

    if (tyMin > tMin)
        tMin = tyMin;

    if (tyMax < tMax)
        tMax = tyMax;

    float tzMin = (box->position.z - box->size.z / 2.0f - rayOrigin.z) / rayDirection.z;
    float tzMax = (box->position.z + box->size.z / 2.0f - rayOrigin.z) / rayDirection.z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax))
        return false;

    return true;
}

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
    
    // todo for the assessment - use a thread for each sub region
    // for example, assuming we have two regions:
    // from 'colliders' create two separate lists
    // empty each list (from previous frame) and work out which collidable object is in which region, 
    //  and add the pointer to that region's list.
    // Then, run two threads with the code below (changing 'colliders' to be the region's list)
    for (ColliderObject* box : colliders) { 
        if (box == nullptr) continue;

        box->update(colliders, deltaTime);
    }
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
    auto old = last;
    last = steady_clock::now();
    const duration<float> frameTime = last - old;
    float deltaTime = frameTime.count();

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

            if (rayBoxIntersection(cameraPosition, rayDirection, box)) {
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
        for (ColliderObject* box : colliders) {
            box->velocity.y += impulseMagnitude;
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
        break;
    case 'a':
    {
        ColliderObject* box = initColliderObject<Box>();
        boxColliders->vector.emplace_back(box);
        std::cout << "Added Box" << std::endl;
    }
        break;
    case 'R':
    {
        std::vector<ColliderObject*>& spheres = sphereColliders->vector;
        delete spheres.back();
        spheres.pop_back();
        std::cout << "Removed Sphere" << std::endl;
    }
        break;
    case 'A':
    {
        ColliderObject* sphere = initColliderObject<Sphere>();
        sphereColliders->vector.emplace_back(sphere);
        std::cout << "Added Sphere" << std::endl;
    }
        break;
    }
}

// the main function. 
int main(int argc, char** argv) {

    srand(static_cast<unsigned>(time(0))); // Seed random number generator
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    int handle = glutCreateWindow("Simple Physics Simulation");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    Timer<std::chrono::steady_clock, std::milli> timer{};
    initScene(boxCount, sphereCount);
    std::cout << "Init took: " << timer.Elapsed() << "ms\n" << std::endl;

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // it will stick here until the program ends. 
    glutMainLoop();

    cleanup();

    return 0;
}
