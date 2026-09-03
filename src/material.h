#pragma once

#include "vec3.h"

class Material {
public:

    Vec3 color;

    double ambient;
    double diffuse;

    // Reflection
    bool metallic;
    double reflectivity;

    // Refraction
    double transmission;
    double ior;

    // Texture
    bool textured;


    Material(
        const Vec3& color,
        double ambient = 0.1,
        double diffuse = 0.9,
        bool metallic = false,
        double reflectivity = 0.0,
        double transmission = 0.0,
        double ior = 1.0,
        bool textured = false
    )
        : color(color),
          ambient(ambient),
          diffuse(diffuse),
          metallic(metallic),
          reflectivity(reflectivity),
          transmission(transmission),
          ior(ior),
          textured(textured) {
    }
};