#pragma once

#include <cmath>
#include <memory>

#include "hittable.h"
#include "material.h"

class Sphere : public Hittable {
public:

    Vec3 center;
    double radius;

    std::shared_ptr<Material> material;

    Sphere(
        const Vec3& center,
        double radius,
        std::shared_ptr<Material> material
    )
        : center(center),
          radius(radius),
          material(material) {
    }

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        Vec3 oc =
            ray.origin - center;

        double a =
            Vec3::dot(
                ray.direction,
                ray.direction
            );

        double b =
            2.0 *
            Vec3::dot(
                oc,
                ray.direction
            );

        double c =
            Vec3::dot(
                oc,
                oc
            ) - radius * radius;

        double discriminant =
            b * b - 4.0 * a * c;

        if (discriminant < 0.0) {
            return false;
        }

        double sqrtDiscriminant =
            std::sqrt(discriminant);

        double t1 =
            (-b - sqrtDiscriminant)
            / (2.0 * a);

        double t2 =
            (-b + sqrtDiscriminant)
            / (2.0 * a);

        double t = t1;

        if (t < 0.001) {
            t = t2;
        }

        if (t < 0.001) {
            return false;
        }

        record.t = t;

        record.point =
            ray.at(t);

        record.normal =
            (
                record.point - center
            ) / radius;

        record.material =
            material;

        return true;
    }

    bool boundingBox(
        AABB& outputBox
    ) const override {

        Vec3 radiusVector(
            radius,
            radius,
            radius
        );

        outputBox =
            AABB(
                center - radiusVector,
                center + radiusVector
            );

        return true;
    }
};