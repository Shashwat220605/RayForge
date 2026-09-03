#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <cmath>
#include <random>

#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "plane.h"
#include "scene.h"
#include "hit_record.h"
#include "material.h"
#include "camera.h"
#include "light.h"
#include "triangle.h"
#include "mesh.h"
#include "obj_loader.h"
#include "texture.h"


// ============================================================
// BMP WRITER
// ============================================================

void writeBMP(
    const std::string& filename,
    const std::vector<Vec3>& pixels,
    int width,
    int height
) {
    int rowSize =
        (3 * width + 3) & (~3);

    int pixelDataSize =
        rowSize * height;

    int fileSize =
        54 + pixelDataSize;

    std::ofstream file(
        filename,
        std::ios::binary
    );

    file.put('B');
    file.put('M');

    file.put(fileSize & 0xFF);
    file.put((fileSize >> 8) & 0xFF);
    file.put((fileSize >> 16) & 0xFF);
    file.put((fileSize >> 24) & 0xFF);

    for (int i = 0; i < 4; i++) {
        file.put(0);
    }

    file.put(54);
    file.put(0);
    file.put(0);
    file.put(0);


    // ========================================================
    // DIB HEADER
    // ========================================================

    file.put(40);
    file.put(0);
    file.put(0);
    file.put(0);

    file.put(width & 0xFF);
    file.put((width >> 8) & 0xFF);
    file.put((width >> 16) & 0xFF);
    file.put((width >> 24) & 0xFF);

    file.put(height & 0xFF);
    file.put((height >> 8) & 0xFF);
    file.put((height >> 16) & 0xFF);
    file.put((height >> 24) & 0xFF);

    file.put(1);
    file.put(0);

    file.put(24);
    file.put(0);

    for (int i = 0; i < 24; i++) {
        file.put(0);
    }


    // ========================================================
    // PIXELS
    // ========================================================

    for (int y = height - 1; y >= 0; y--) {

        int padding =
            rowSize - width * 3;

        for (int x = 0; x < width; x++) {

            Vec3 color =
                pixels[y * width + x];

            color.x =
                std::clamp(
                    color.x,
                    0.0,
                    1.0
                );

            color.y =
                std::clamp(
                    color.y,
                    0.0,
                    1.0
                );

            color.z =
                std::clamp(
                    color.z,
                    0.0,
                    1.0
                );

            uint8_t r =
                static_cast<uint8_t>(
                    color.x * 255.999
                );

            uint8_t g =
                static_cast<uint8_t>(
                    color.y * 255.999
                );

            uint8_t b =
                static_cast<uint8_t>(
                    color.z * 255.999
                );

            file.put(b);
            file.put(g);
            file.put(r);
        }

        for (int i = 0; i < padding; i++) {
            file.put(0);
        }
    }

    file.close();
}


// ============================================================
// SHADOW TEST
// ============================================================

bool sampleIsInShadow(
    const HitRecord& record,
    const Vec3& lightPosition,
    const Scene& scene
) {
    Vec3 direction =
        lightPosition - record.point;

    double distanceToLight =
        direction.length();

    direction =
        direction.normalized();

    Vec3 shadowOrigin =
        record.point
        + record.normal * 0.001;

    Ray shadowRay(
        shadowOrigin,
        direction
    );

    HitRecord shadowRecord;

    if (
        scene.hit(
            shadowRay,
            shadowRecord
        )
    ) {

        if (
            shadowRecord.t
            < distanceToLight
        ) {
            return true;
        }
    }

    return false;
}


// ============================================================
// SOFT SHADOW
// ============================================================

double shadowAmount(
    const HitRecord& record,
    const AreaLight& light,
    const Scene& scene
) {
    const int samplesPerSide = 5;

    int totalSamples =
        samplesPerSide
        * samplesPerSide;

    int blockedSamples = 0;


    for (
        int y = 0;
        y < samplesPerSide;
        y++
    ) {

        for (
            int x = 0;
            x < samplesPerSide;
            x++
        ) {

            Vec3 samplePosition =
                light.sample(
                    x,
                    y,
                    samplesPerSide
                );

            if (
                sampleIsInShadow(
                    record,
                    samplePosition,
                    scene
                )
            ) {
                blockedSamples++;
            }
        }
    }


    return static_cast<double>(
        blockedSamples
    ) / totalSamples;
}


// ============================================================
// SKY
// ============================================================

Vec3 skyColor(
    const Ray& ray
) {
    Vec3 direction =
        ray.direction.normalized();

    double t =
        0.5 *
        (direction.y + 1.0);

    t =
        std::clamp(
            t,
            0.0,
            1.0
        );

    Vec3 skyBottom(
        0.90,
        0.95,
        1.00
    );

    Vec3 skyTop(
        0.25,
        0.45,
        0.80
    );

    return
        skyBottom * (1.0 - t)
        +
        skyTop * t;
}


// ============================================================
// REFLECTION
// ============================================================

Vec3 reflect(
    const Vec3& direction,
    const Vec3& normal
) {
    return
        direction
        -
        normal
        *
        (
            2.0
            *
            Vec3::dot(
                direction,
                normal
            )
        );
}


// ============================================================
// REFRACTION
// ============================================================

bool refract(
    const Vec3& direction,
    const Vec3& normal,
    double etaRatio,
    Vec3& refractedDirection
) {
    Vec3 unitDirection =
        direction.normalized();

    Vec3 unitNormal =
        normal.normalized();

    double cosTheta =
        std::min(
            Vec3::dot(
                unitDirection * -1.0,
                unitNormal
            ),
            1.0
        );

    Vec3 perpendicular =
        (
            unitDirection
            +
            unitNormal * cosTheta
        )
        *
        etaRatio;

    double perpendicularLengthSquared =
        Vec3::dot(
            perpendicular,
            perpendicular
        );

    double parallelSquared =
        1.0
        -
        perpendicularLengthSquared;


    if (
        parallelSquared < 0.0
    ) {
        return false;
    }


    Vec3 parallel =
        unitNormal
        *
        (
            -std::sqrt(
                parallelSquared
            )
        );


    refractedDirection =
        perpendicular
        +
        parallel;

    return true;
}


// ============================================================
// FRESNEL / SCHLICK
// ============================================================

double fresnelReflectance(
    const Vec3& direction,
    const Vec3& normal,
    double refractionRatio
) {
    double cosTheta =
        std::clamp(
            -Vec3::dot(
                direction.normalized(),
                normal.normalized()
            ),
            0.0,
            1.0
        );

    double r0 =
        (
            1.0
            -
            refractionRatio
        )
        /
        (
            1.0
            +
            refractionRatio
        );

    r0 =
        r0 * r0;

    return
        r0
        +
        (
            1.0 - r0
        )
        *
        std::pow(
            1.0 - cosTheta,
            5.0
        );
}


// ============================================================
// TEXTURE COLOR
// ============================================================

Vec3 textureColor(
    const HitRecord& record
) {

    // ========================================================
    // IMPORTANT
    //
    // Only materials explicitly marked as textured
    // are allowed to use a texture.
    // ========================================================

    if (
        !record.material->textured
    ) {
        return record.material->color;
    }


    // ========================================================
    // GROUND TEXTURE
    // ========================================================

    if (
        record.normal.y > 0.9
    ) {

        return Texture::planeChecker(

            record.point,

            Vec3(
                0.12,
                0.12,
                0.12
            ),

            Vec3(
                0.55,
                0.55,
                0.55
            ),

            2.5
        );
    }


    // ========================================================
    // SPHERE TEXTURE
    // ========================================================

    return Texture::sphereChecker(

        record.normal,

        Vec3(
            0.85,
            0.03,
            0.02
        ),

        Vec3(
            0.15,
            0.01,
            0.01
        ),

        8.0
    );
}


// ============================================================
// RAY COLOR
// ============================================================

Vec3 rayColor(
    const Ray& ray,
    const Scene& scene,
    const AreaLight& light,
    int depth
) {
    const int maxDepth = 4;


    // ========================================================
    // MAXIMUM BOUNCES
    // ========================================================

    if (
        depth >= maxDepth
    ) {
        return skyColor(ray);
    }


    // ========================================================
    // FIND CLOSEST OBJECT
    // ========================================================

    HitRecord record;

    if (
        !scene.hit(
            ray,
            record
        )
    ) {
        return skyColor(ray);
    }


    // ========================================================
    // MATERIAL CHECK
    // ========================================================

    if (
        !record.material
    ) {

        return Vec3(
            1.0,
            0.0,
            1.0
        );
    }


    // ========================================================
    // TEXTURE / MATERIAL COLOR
    // ========================================================

    Vec3 materialColor =
        textureColor(
            record
        );


    // ========================================================
    // FRONT FACE
    // ========================================================

    bool frontFace =
        Vec3::dot(
            ray.direction,
            record.normal
        ) < 0.0;


    Vec3 orientedNormal =
        frontFace
        ?
        record.normal
        :
        record.normal * -1.0;


    // ========================================================
    // LIGHT
    // ========================================================

    Vec3 lightDirection =
        (
            light.position
            -
            record.point
        ).normalized();


    // ========================================================
    // DIFFUSE LIGHT
    // ========================================================

    double diffuse =
        Vec3::dot(
            orientedNormal,
            lightDirection
        );

    diffuse =
        std::max(
            0.0,
            diffuse
        );


    // ========================================================
    // SOFT SHADOW
    // ========================================================

    double shadow =
        shadowAmount(
            record,
            light,
            scene
        );

    double visibility =
        1.0 - shadow;


    // ========================================================
    // LOCAL LIGHTING
    // ========================================================

    double brightness =
        record.material->ambient
        +
        diffuse
        *
        record.material->diffuse
        *
        visibility;

    Vec3 localColor =
        materialColor
        *
        brightness;


    // ========================================================
    // MATERIAL TYPE
    // ========================================================

    bool reflective =
        record.material->reflectivity
        >
        0.0;

    bool transparent =
        record.material->transmission
        >
        0.0;


    if (
        !reflective
        &&
        !transparent
    ) {

        return localColor;
    }


    // ========================================================
    // REFLECTION RAY
    // ========================================================

    Vec3 incomingDirection =
        ray.direction.normalized();

    Vec3 reflectionDirection =
        reflect(
            incomingDirection,
            orientedNormal
        ).normalized();


    Vec3 reflectionOrigin =
        record.point
        +
        orientedNormal * 0.001;


    Ray reflectionRay(
        reflectionOrigin,
        reflectionDirection
    );


    Vec3 reflectedColor =
        rayColor(
            reflectionRay,
            scene,
            light,
            depth + 1
        );


    // ========================================================
    // GLASS
    // ========================================================

    if (
        transparent
    ) {

        double refractionRatio;

        if (
            frontFace
        ) {
            refractionRatio =
                1.0
                /
                record.material->ior;
        }
        else {
            refractionRatio =
                record.material->ior;
        }


        // ----------------------------------------------------
        // REFRACTION
        // ----------------------------------------------------

        Vec3 refractedDirection;

        bool canRefract =
            refract(
                incomingDirection,
                orientedNormal,
                refractionRatio,
                refractedDirection
            );


        double reflectionWeight =
            fresnelReflectance(
                incomingDirection,
                orientedNormal,
                refractionRatio
            );


        Vec3 transmittedColor(
            0.0,
            0.0,
            0.0
        );


        if (
            canRefract
        ) {

            Vec3 refractionOrigin =
                record.point
                -
                orientedNormal * 0.001;


            Ray refractionRay(
                refractionOrigin,
                refractedDirection.normalized()
            );


            transmittedColor =
                rayColor(
                    refractionRay,
                    scene,
                    light,
                    depth + 1
                );
        }
        else {

            reflectionWeight =
                1.0;
        }


        Vec3 glassColor =
            reflectedColor
            *
            reflectionWeight
            +
            transmittedColor
            *
            (
                1.0
                -
                reflectionWeight
            );


        double transmission =
            std::clamp(
                record.material->transmission,
                0.0,
                1.0
            );


        return
            localColor
            *
            (
                1.0
                -
                transmission
            )
            +
            glassColor
            *
            transmission;
    }


    // ========================================================
    // METALLIC REFLECTION
    // ========================================================

    double reflection =
        std::clamp(
            record.material->reflectivity,
            0.0,
            1.0
        );


    if (
        record.material->metallic
    ) {

        reflectedColor =
            Vec3(
                reflectedColor.x
                *
                materialColor.x,

                reflectedColor.y
                *
                materialColor.y,

                reflectedColor.z
                *
                materialColor.z
            );
    }


    return
        localColor
        *
        (
            1.0
            -
            reflection
        )
        +
        reflectedColor
        *
        reflection;
}


// ============================================================
// MAIN
// ============================================================

int main() {

    const int width = 800;
    const int height = 600;

    const int samplesPerPixel = 4;


    std::vector<Vec3> pixels(
        width * height
    );


    // ========================================================
    // CAMERA
    // ========================================================

    Camera camera(

        Vec3(
            0.0,
            1.0,
            3.5
        ),

        Vec3(
            0.0,
            0.0,
            -1.5
        ),

        Vec3(
            0.0,
            1.0,
            0.0
        ),

        static_cast<double>(width)
        /
        static_cast<double>(height),

        55.0,

        0.30,

        4.0
    );


    // ========================================================
    // RED MATERIAL
    // ========================================================

    auto redMaterial =
        std::make_shared<Material>(

            Vec3(
                0.85,
                0.03,
                0.02
            ),

            0.15,
            0.85,

            false,
            0.0,

            0.0,
            1.0,

            true
        );


    // ========================================================
    // BLUE MATERIAL
    // ========================================================

    auto blueMaterial =
        std::make_shared<Material>(

            Vec3(
                0.03,
                0.15,
                0.85
            ),

            0.15,
            0.85,

            false,
            0.0,

            0.0,
            1.0,

            false
        );


    // ========================================================
    // YELLOW MATERIAL
    // ========================================================

    auto yellowMaterial =
        std::make_shared<Material>(

            Vec3(
                0.95,
                0.70,
                0.02
            ),

            0.15,
            0.85,

            false,
            0.0,

            0.0,
            1.0,

            false
        );


    // ========================================================
    // GROUND MATERIAL
    // ========================================================

    auto groundMaterial =
        std::make_shared<Material>(

            Vec3(
                0.42,
                0.42,
                0.42
            ),

            0.25,
            0.75,

            false,
            0.0,

            0.0,
            1.0,

            true
        );


    // ========================================================
    // METALLIC MATERIAL
    // ========================================================

    auto metalMaterial =
        std::make_shared<Material>(

            Vec3(
                0.85,
                0.88,
                0.92
            ),

            0.05,
            0.25,

            true,
            0.90,

            0.0,
            1.0,

            false
        );


    // ========================================================
    // GLASS MATERIAL
    // ========================================================

    auto glassMaterial =
        std::make_shared<Material>(

            Vec3(
                0.95,
                0.98,
                1.00
            ),

            0.0,
            0.05,

            false,
            0.0,

            1.0,
            1.50,

            false
        );


    // ========================================================
    // LIGHT
    // ========================================================

    AreaLight light(

        Vec3(
            -2.0,
            4.0,
            2.0
        ),

        1.5,

        Vec3(
            1.0,
            1.0,
            1.0
        )
    );


    // ========================================================
    // SCENE
    // ========================================================

    Scene scene;


    // ========================================================
    // RED SPHERE
    // ========================================================

    scene.add(
        std::make_shared<Sphere>(

            Vec3(
                -0.9,
                0.0,
                -1.2
            ),

            0.5,

            redMaterial
        )
    );


    // ========================================================
    // BLUE SPHERE
    // ========================================================

    scene.add(
        std::make_shared<Sphere>(

            Vec3(
                0.8,
                0.0,
                -1.4
            ),

            0.5,

            blueMaterial
        )
    );


    // ========================================================
    // YELLOW SPHERE
    // ========================================================

    scene.add(
        std::make_shared<Sphere>(

            Vec3(
                0.0,
                0.65,
                -1.8
            ),

            0.45,

            yellowMaterial
        )
    );


    // ========================================================
    // METALLIC SPHERE
    // ========================================================

    scene.add(
        std::make_shared<Sphere>(

            Vec3(
                1.65,
                0.05,
                -1.9
            ),

            0.55,

            metalMaterial
        )
    );


    // ========================================================
    // GLASS SPHERE
    // ========================================================

    scene.add(
        std::make_shared<Sphere>(

            Vec3(
                -1.65,
                0.10,
                -2.0
            ),

            0.55,

            glassMaterial
        )
    );


    // ========================================================
    // GROUND
    // ========================================================

    scene.add(
        std::make_shared<Plane>(

            Vec3(
                0.0,
                -0.5,
                0.0
            ),

            Vec3(
                0.0,
                1.0,
                0.0
            ),

            groundMaterial
        )
    );

    auto triangleMaterial =
    std::make_shared<Material>(
        Vec3(
            0.05,
            0.75,
            0.20
        ),
        0.15,
        0.85
    );

auto triangle =
    std::make_shared<Triangle>(
        Vec3(-0.6, -0.45, -2.5),
        Vec3( 0.6, -0.45, -2.5),
        Vec3( 0.0,  0.65, -2.5),
        triangleMaterial
    );

scene.add(triangle);


    // ========================================================
    // RANDOM NUMBER GENERATOR
    // ========================================================

    std::mt19937 rng(42);

    std::uniform_real_distribution<double> random01(
        0.0,
        1.0
    );


    // ========================================================
    // RENDER
    // ========================================================

    std::cout
        << "Rendering RayForge Day 8...\n";

    std::cout
        << "Objects: 5 spheres + ground\n";

    std::cout
        << "Textures: ON\n";

    std::cout
        << "Red sphere texture: ON\n";

    std::cout
        << "Blue sphere texture: OFF\n";

    std::cout
        << "Yellow sphere texture: OFF\n";

    std::cout
        << "Ground texture: ON\n";

    std::cout
        << "Metal reflections: ON\n";

    std::cout
        << "Glass refraction: ON\n";

    std::cout
        << "Depth of field: ON\n";


    // ========================================================
    // RENDER LOOP
    // ========================================================

    for (
        int y = 0;
        y < height;
        y++
    ) {

        for (
            int x = 0;
            x < width;
            x++
        ) {

            Vec3 pixelColor(
                0.0,
                0.0,
                0.0
            );


            // =================================================
            // ANTI-ALIASING + DEPTH OF FIELD
            // =================================================

            for (
                int sample = 0;
                sample < samplesPerPixel;
                sample++
            ) {

                int sampleX =
                    sample % 2;

                int sampleY =
                    sample / 2;


                // ------------------------------------------------
                // JITTERED PIXEL SAMPLE
                // ------------------------------------------------

                double jitterX =
                    random01(rng);

                double jitterY =
                    random01(rng);


                double u =
                    (
                        static_cast<double>(x)
                        +
                        (
                            sampleX
                            +
                            jitterX
                        )
                        /
                        2.0
                    )
                    /
                    width;


                double v =
                    (
                        static_cast<double>(y)
                        +
                        (
                            sampleY
                            +
                            jitterY
                        )
                        /
                        2.0
                    )
                    /
                    height;


                // ------------------------------------------------
                // RANDOM POINT ON LENS
                // ------------------------------------------------

                double lensAngle =
                    2.0
                    *
                    3.14159265358979323846
                    *
                    random01(rng);


                double lensRadius =
                    std::sqrt(
                        random01(rng)
                    );


                double lensX =
                    lensRadius
                    *
                    std::cos(
                        lensAngle
                    );


                double lensY =
                    lensRadius
                    *
                    std::sin(
                        lensAngle
                    );


                // ------------------------------------------------
                // CAMERA RAY
                // ------------------------------------------------

                Ray ray =
                    camera.getRay(
                        u,
                        v,
                        lensX,
                        lensY
                    );


                // ------------------------------------------------
                // TRACE
                // ------------------------------------------------

                pixelColor =
                    pixelColor
                    +
                    rayColor(
                        ray,
                        scene,
                        light,
                        0
                    );
            }


            // =================================================
            // AVERAGE SAMPLES
            // =================================================

            pixelColor =
                pixelColor
                /
                static_cast<double>(
                    samplesPerPixel
                );


            pixels[
                y * width + x
            ] =
                pixelColor;
        }


        if (
            y % 25 == 0
        ) {

            int progress =
                static_cast<int>(
                    100.0
                    *
                    y
                    /
                    height
                );

            std::cout
                << "Progress: "
                << progress
                << "%\n";
        }
    }


    // ========================================================
    // SAVE IMAGE
    // ========================================================

    writeBMP(
        "output/image.bmp",
        pixels,
        width,
        height
    );


    std::cout
        << "RayForge Day 8 completed!\n";

    std::cout
        << "Image saved to output/image.bmp\n";


    return 0;
}