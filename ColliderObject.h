#pragma once

#include "Vec3.h"
#include <array>
#include <GL/glut.h>
#include "globals.h"
#include "LinkedVector.h"

class ColliderObject
{
public:
    Vec3 position;
    Vec3 size;
    Vec3 velocity;
    Vec3 colour;

    ColliderObject* pNext = nullptr;

    // if two colliders collide, push them away from each other
    static void resolveCollision(ColliderObject* a, ColliderObject* b) {
        Vec3 normal = { a->position.x - b->position.x, a->position.y - b->position.y, a->position.z - b->position.z };
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

        // Normalize the normal vector
        normal.normalise();

        float relativeVelocityX = a->velocity.x - b->velocity.x;
        float relativeVelocityY = a->velocity.y - b->velocity.y;
        float relativeVelocityZ = a->velocity.z - b->velocity.z;

        // Compute the relative velocity along the normal
        float impulse = relativeVelocityX * normal.x + relativeVelocityY * normal.y + relativeVelocityZ * normal.z;

        // Ignore collision if objects are moving away from each other
        if (impulse > 0) {
            return;
        }

        // Compute the collision impulse scalar
        float e = 0.01f; // Coefficient of restitution (0 = inelastic, 1 = elastic)
        float dampening = 0.9f; // Dampening factor (0.9 = 10% energy reduction)
        float j = -(1.0f + e) * impulse * dampening;

        // Apply the impulse to the colliders' velocities
        a->velocity.x += j * normal.x;
        a->velocity.y += j * normal.y;
        a->velocity.z += j * normal.z;
        b->velocity.x -= j * normal.x;
        b->velocity.y -= j * normal.y;
        b->velocity.z -= j * normal.z;
    }

    // are two colliders colliding?
    static bool checkCollision(const ColliderObject* a, const ColliderObject* b) {
        return (std::abs(a->position.x - b->position.x) * 2 < (a->size.x + b->size.x)) &&
            (std::abs(a->position.y - b->position.y) * 2 < (a->size.y + b->size.y)) &&
            (std::abs(a->position.z - b->position.z) * 2 < (a->size.z + b->size.z));
    }

    static bool TestCollision(ColliderObject* a, ColliderObject* b)
    {
        if (checkCollision(a, b)) {
            resolveCollision(a, b);
            return true;
        }
        return false;
    }

    // draw the physics object
    void draw() {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        GLfloat diffuseMaterial[] = { colour.x, colour.y, colour.z, 1.0f };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
        glScalef(size.x, size.y, size.z);
        glRotatef(-90, 1, 0, 0);
        drawMesh();
        //glutSolidTeapot(1);
        //glutSolidCone(1, 1, 10, 10);
        glPopMatrix();
    }

    virtual void drawMesh() {};

    void update(const float& deltaTime)
    {
        const float floorY = minY;
        // Update velocity due to gravity
        velocity.y += gravity * deltaTime;

        // Update position based on velocity
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;

        // Check for collision with the floor
        if (position.y - size.y / 2.0f < floorY) {
            position.y = floorY + size.y / 2.0f;
            float dampening = 0.7f;
            velocity.y = -velocity.y * dampening;
        }

        // ceiling
        if (position.y + size.y / 2.0f > maxY) {
            velocity.y = -velocity.y;
        }

        // Check for collision with the walls
        if (position.x - size.x / 2.0f < minX || position.x + size.x / 2.0f > maxX) {
            velocity.x = -velocity.x;
        }
        if (position.z - size.z / 2.0f < minZ || position.z + size.z / 2.0f > maxZ) {
            velocity.z = -velocity.z;
        }

        pNext = nullptr;
    }

    void updateCollisions(LinkedVector<ColliderObject*>& colliders) {
        // Check for collisions with other colliders
        for (auto it = colliders.begin(); it != colliders.end(); ++it) {
            ColliderObject* other = *it;
            if (other == nullptr) continue;

            if (this == other) continue;
            if (TestCollision(this, other)) break;
        }
    }

    // a ray which is used to tap (by default, remove) a box - see the 'mouse' function for how this is used.
    bool rayBoxIntersection(const Vec3& rayOrigin, const Vec3& rayDirection)
    {
        float tMin = (position.x - size.x / 2.0f - rayOrigin.x) / rayDirection.x;
        float tMax = (position.x + size.x / 2.0f - rayOrigin.x) / rayDirection.x;

        if (tMin > tMax) std::swap(tMin, tMax);

        float tyMin = (position.y - size.y / 2.0f - rayOrigin.y) / rayDirection.y;
        float tyMax = (position.y + size.y / 2.0f - rayOrigin.y) / rayDirection.y;

        if (tyMin > tyMax) std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax))
            return false;

        if (tyMin > tMin)
            tMin = tyMin;

        if (tyMax < tMax)
            tMax = tyMax;

        float tzMin = (position.z - size.z / 2.0f - rayOrigin.z) / rayDirection.z;
        float tzMax = (position.z + size.z / 2.0f - rayOrigin.z) / rayDirection.z;

        if (tzMin > tzMax) std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax))
            return false;

        return true;
    }

    template<class ColliderType>
    static ColliderObject* createCollider()
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
};

