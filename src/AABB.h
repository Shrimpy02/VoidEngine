#pragma once
#include <glm/vec3.hpp>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <Collision.h>
#include <Actor.h>
#include <iostream>
#include "BoundingSphere.h"
#include <Mesh.h>

struct AABB
{
private:
    // ----------- Variables ---------------

     // ----------- Functions ---------------

public:
    // ----------- Variables ---------------
    glm::vec3 center{ 0.0f,0.0f,0.0f };
    glm::vec3 extent{ 0.5f,0.5f,0.5f };

    // ----------- Functions ---------------
    AABB(const glm::vec3& center, const glm::vec3& extent) : center(center), extent(extent) {}

    bool IsIntersecting(const AABB& other, glm::vec3* mtv) const
    {
        // calculates the difference from one center to the other
        // and their extent sum
        glm::vec3 diff = other.center - this->center;
        glm::vec3 sumExtent = this->extent + other.extent;

        // Check each axis. 0 = x, 1 = y, 2 = z. 
        for(int i = 0; i < 3; i++)
        {
            // if the difference in length is larger then the sum extent
            // in each axis there is no intersection. 
	        if(abs(diff[i]) >= sumExtent[i])
	        {
                return false; // no intersection
	        }
        }

        // If there is an intersection (ie function has not returned yet)
        // and the caller wants to calculate the minimum translation vector
        if(mtv)
        {
            // Gets collision values from the check earlier
            glm::vec3 mtvValues;
            for (int i = 0; i < 3; i++)
            {
                mtvValues[i] = sumExtent[i] - abs(diff[i]);
            }

            // Find the minimum non-zero translation vector
            // by iterating through each axis and selecting the smallest non zero value.
            float minMTV = FLT_MAX;
            int minAxis = -1;

            // axis
            for(int i = 0; i < 3; i++)
            {
                // if an axis value is less than minMTV and greater than 0
	            if(mtvValues[i] < minMTV && mtvValues[i] > 0)
	            {
                    // set new minMTV and its axis
                    minMTV = mtvValues[i];
                    minAxis = i;
	            }
            }

            // calculates the direction of the mtv based on the axis with the smallest mtv 
            if(minAxis != -1)
            {
                // Calc direction
                glm::vec3 direction = glm::vec3(0);
                direction[minAxis] = diff[minAxis] > 0.f ? 1.f : -1.f;
                // Assigns the calculated mtv ti the mtv pointer.
                *mtv = direction * minMTV;
            }
        }

        return true; // intersection exists
    }
  
    void Expand(const glm::vec3& p)
    {
        auto relativeP = p - center;
        extent = glm::max(abs(relativeP), extent);
    }
 
};


class AABBActor : public Actor, public IBounded, public IRender
{
public:
    // ---------- Global Variables --------------

    CollisionProperties mCollisionProperties{ CollisionType::STATIC, CollisionResponse::BLOCK };

private:
    // ---------- Local Variables --------------

    glm::vec3 mCenter;
    glm::vec3 mMaxExtent;
	glm::vec3 mMinExtent;
    class Mesh* mCollisionMesh;
    bool mDrawCollisionMesh = true;

public:
    // ---------- Global functions --------------
    AABBActor(const std::string& name, class Mesh* _collisionMesh, glm::vec3& _minExtent, glm::vec3& _maxExtent)
        :Actor(name), mCollisionMesh(_collisionMesh), mMinExtent(_minExtent), mMaxExtent(_maxExtent)  {}


    void Draw(const Shader* _shader) const override;

    AABB GetAABB() const override;

    CollisionProperties GetCollisionProperties() const;


private:
    // ---------- Local functions --------------
  
public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};