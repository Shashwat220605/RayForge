#pragma once

#include <limits>
#include <memory>
#include <vector>

#include "hittable.h"
#include "triangle.h"
#include "bvh.h"

class Mesh : public Hittable {
public:

    // Keep triangles as values so the existing
    // OBJ loader and addTriangle() interface
    // continue to work.
    std::vector<Triangle> triangles;

    // Internal acceleration structure.
    // Built lazily the first time the mesh is rendered.
    mutable std::shared_ptr<BVHNode> bvhRoot;

    Mesh()
        : bvhRoot(nullptr) {
    }

    // ============================================================
    // ADD TRIANGLE
    // ============================================================

    void addTriangle(
        const Vec3& v0,
        const Vec3& v1,
        const Vec3& v2,
        std::shared_ptr<Material> material
    ) {

        triangles.emplace_back(
            v0,
            v1,
            v2,
            material
        );

        // Mesh changed, so the old BVH is no longer valid.
        bvhRoot.reset();
    }

    // ============================================================
    // BUILD TRIANGLE BVH
    // ============================================================

    void buildBVH() const {

        std::vector<
            std::shared_ptr<Hittable>
        > triangleObjects;

        triangleObjects.reserve(
            triangles.size()
        );

        for (
            const Triangle& triangle :
            triangles
        ) {

            // Create a shared Triangle object for
            // the acceleration structure.
            triangleObjects.push_back(
                std::make_shared<Triangle>(
                    triangle
                )
            );
        }

        if (
            triangleObjects.empty()
        ) {

            bvhRoot.reset();

            return;
        }

        bvhRoot =
            std::make_shared<BVHNode>(
                triangleObjects
            );
    }

    // ============================================================
    // RAY / MESH INTERSECTION
    // ============================================================

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        // --------------------------------------------------------
        // Empty mesh
        // --------------------------------------------------------

        if (triangles.empty()) {
            return false;
        }

        // --------------------------------------------------------
        // Lazily build BVH
        // --------------------------------------------------------

        if (!bvhRoot) {
            buildBVH();
        }

        // --------------------------------------------------------
        // BVH traversal
        // --------------------------------------------------------

        if (bvhRoot) {

            return bvhRoot->hit(
                ray,
                record
            );
        }

        return false;
    }

    // ============================================================
    // MESH BOUNDING BOX
    // ============================================================

    bool boundingBox(
        AABB& outputBox
    ) const override {

        if (triangles.empty()) {
            return false;
        }

        AABB meshBox;

        bool firstBox = true;

        for (
            const Triangle& triangle :
            triangles
        ) {

            AABB triangleBox;

            if (
                !triangle.boundingBox(
                    triangleBox
                )
            ) {
                continue;
            }

            if (firstBox) {

                meshBox =
                    triangleBox;

                firstBox = false;
            }
            else {

                meshBox =
                    AABB::surroundingBox(
                        meshBox,
                        triangleBox
                    );
            }
        }

        if (firstBox) {
            return false;
        }

        outputBox =
            meshBox;

        return true;
    }
};