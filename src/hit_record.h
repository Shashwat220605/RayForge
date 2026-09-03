#pragma once

#include <memory>

#include "vec3.h"

class Material;

class HitRecord {
public:

    Vec3 point;
    Vec3 normal;

    double t;

    std::shared_ptr<Material> material;

    HitRecord()
        : point(),
          normal(),
          t(0.0),
          material(nullptr) {}
};