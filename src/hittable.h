#pragma once

#include "ray.h"
#include "hit_record.h"
#include "aabb.h"

class Hittable {
public:

    virtual bool hit(
        const Ray& ray,
        HitRecord& record
    ) const = 0;

    virtual bool boundingBox(
        AABB& outputBox
    ) const = 0;

    virtual ~Hittable() = default;
};