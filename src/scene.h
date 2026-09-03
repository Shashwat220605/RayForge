#pragma once

#include <limits>
#include <memory>
#include <vector>

#include "hittable.h"
#include "bvh.h"

class Scene : public Hittable {
public:

    std::vector<std::shared_ptr<Hittable>>
        objects;

    mutable std::shared_ptr<BVHNode>
        bvhRoot;

    void add(
        std::shared_ptr<Hittable> object
    ) {

        objects.push_back(
            std::move(object)
        );

        // Scene changed, so the BVH
        // needs to be rebuilt.
        bvhRoot.reset();
    }

    void buildBVH() const {

        std::vector<std::shared_ptr<Hittable>>
            boundedObjects;

        for (const auto& object : objects) {

            if (!object) {
                continue;
            }

            AABB objectBox;

            if (
                object->boundingBox(
                    objectBox
                )
            ) {

                boundedObjects.push_back(
                    object
                );
            }
        }

        if (!boundedObjects.empty()) {

            bvhRoot =
                std::make_shared<BVHNode>(
                    boundedObjects
                );
        }
        else {

            bvhRoot.reset();
        }
    }

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        if (!bvhRoot) {
            buildBVH();
        }

        bool hitAnything =
            false;

        double closestSoFar =
            std::numeric_limits<double>::infinity();

        // Test all finite objects through BVH.
        if (bvhRoot) {

            HitRecord bvhRecord;

            if (
                bvhRoot->hit(
                    ray,
                    bvhRecord
                )
            ) {

                hitAnything = true;

                closestSoFar =
                    bvhRecord.t;

                record =
                    bvhRecord;
            }
        }

        // Infinite objects, such as the ground
        // plane, are tested separately.
        for (const auto& object : objects) {

            if (!object) {
                continue;
            }

            AABB objectBox;

            if (
                object->boundingBox(
                    objectBox
                )
            ) {
                continue;
            }

            HitRecord temporaryRecord;

            if (
                object->hit(
                    ray,
                    temporaryRecord
                )
            ) {

                if (
                    temporaryRecord.t <
                    closestSoFar
                ) {

                    hitAnything = true;

                    closestSoFar =
                        temporaryRecord.t;

                    record =
                        temporaryRecord;
                }
            }
        }

        return hitAnything;
    }

    bool boundingBox(
        AABB& outputBox
    ) const override {

        bool foundBox =
            false;

        for (const auto& object : objects) {

            if (!object) {
                continue;
            }

            AABB objectBox;

            if (
                !object->boundingBox(
                    objectBox
                )
            ) {
                continue;
            }

            if (!foundBox) {

                outputBox =
                    objectBox;

                foundBox = true;
            }
            else {

                outputBox =
                    AABB::surroundingBox(
                        outputBox,
                        objectBox
                    );
            }
        }

        return foundBox;
    }
};