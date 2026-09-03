#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

#include "aabb.h"
#include "hittable.h"

class BVHNode : public Hittable {
public:

    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;

    AABB box;

    BVHNode()
        : left(nullptr),
          right(nullptr),
          box() {
    }

    explicit BVHNode(
        const std::vector<std::shared_ptr<Hittable>>& objects
    ) {

        std::vector<std::shared_ptr<Hittable>>
            boundedObjects;

        for (const auto& object : objects) {

            if (!object) {
                continue;
            }

            AABB objectBox;

            if (object->boundingBox(objectBox)) {
                boundedObjects.push_back(object);
            }
        }

        build(
            boundedObjects,
            0,
            boundedObjects.size()
        );
    }

    BVHNode(
        std::vector<std::shared_ptr<Hittable>>& objects,
        std::size_t start,
        std::size_t end
    ) {

        build(
            objects,
            start,
            end
        );
    }

    void build(
        std::vector<std::shared_ptr<Hittable>>& objects,
        std::size_t start,
        std::size_t end
    ) {

        std::size_t objectCount =
            end - start;

        if (objectCount == 0) {

            left = nullptr;
            right = nullptr;
            box = AABB();

            return;
        }

        if (objectCount == 1) {

            left =
                objects[start];

            right = nullptr;

            left->boundingBox(box);

            return;
        }

        AABB rangeBox;

        bool firstBox = true;

        for (
            std::size_t i = start;
            i < end;
            i++
        ) {

            AABB objectBox;

            if (
                !objects[i]->boundingBox(
                    objectBox
                )
            ) {
                continue;
            }

            if (firstBox) {

                rangeBox =
                    objectBox;

                firstBox = false;
            }
            else {

                rangeBox =
                    AABB::surroundingBox(
                        rangeBox,
                        objectBox
                    );
            }
        }

        Vec3 extent =
            rangeBox.maximum -
            rangeBox.minimum;

        int axis = 0;

        if (
            extent.y > extent.x &&
            extent.y >= extent.z
        ) {
            axis = 1;
        }
        else if (
            extent.z > extent.x &&
            extent.z >= extent.y
        ) {
            axis = 2;
        }

        std::sort(
            objects.begin()
                + static_cast<std::ptrdiff_t>(start),

            objects.begin()
                + static_cast<std::ptrdiff_t>(end),

            [axis](
                const std::shared_ptr<Hittable>& a,
                const std::shared_ptr<Hittable>& b
            ) {

                AABB boxA;
                AABB boxB;

                a->boundingBox(boxA);
                b->boundingBox(boxB);

                Vec3 centerA =
                    boxA.centroid();

                Vec3 centerB =
                    boxB.centroid();

                if (axis == 0) {
                    return centerA.x < centerB.x;
                }

                if (axis == 1) {
                    return centerA.y < centerB.y;
                }

                return centerA.z < centerB.z;
            }
        );

        std::size_t middle =
            start + objectCount / 2;

        left =
            std::make_shared<BVHNode>(
                objects,
                start,
                middle
            );

        right =
            std::make_shared<BVHNode>(
                objects,
                middle,
                end
            );

        AABB leftBox;
        AABB rightBox;

        left->boundingBox(leftBox);
        right->boundingBox(rightBox);

        box =
            AABB::surroundingBox(
                leftBox,
                rightBox
            );
    }

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        if (!box.hit(ray)) {
            return false;
        }

        bool hitAnything = false;

        double closestSoFar =
            1e30;

        HitRecord leftRecord;

        if (
            left &&
            left->hit(
                ray,
                leftRecord
            )
        ) {

            hitAnything = true;

            closestSoFar =
                leftRecord.t;

            record =
                leftRecord;
        }

        HitRecord rightRecord;

        if (
            right &&
            right->hit(
                ray,
                rightRecord
            )
        ) {

            if (
                !hitAnything ||
                rightRecord.t < closestSoFar
            ) {

                hitAnything = true;

                closestSoFar =
                    rightRecord.t;

                record =
                    rightRecord;
            }
        }

        return hitAnything;
    }

    bool boundingBox(
        AABB& outputBox
    ) const override {

        outputBox =
            box;

        return
            left != nullptr ||
            right != nullptr;
    }
};