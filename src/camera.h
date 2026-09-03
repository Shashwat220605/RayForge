#pragma once

#include <cmath>

#include "ray.h"

class Camera {
public:

    // ========================================================
    // CAMERA TRANSFORM
    // ========================================================

    Vec3 position;
    Vec3 target;
    Vec3 worldUp;

    Vec3 forward;
    Vec3 right;
    Vec3 up;


    // ========================================================
    // CAMERA SETTINGS
    // ========================================================

    double aspectRatio;
    double fieldOfView;

    // Depth of field settings
    double aperture;
    double focusDistance;


    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    Camera(
        const Vec3& position,
        const Vec3& target,
        const Vec3& worldUp,
        double aspectRatio,
        double fieldOfView,
        double aperture = 0.0,
        double focusDistance = 1.0
    )
        : position(position),
          target(target),
          worldUp(worldUp),
          aspectRatio(aspectRatio),
          fieldOfView(fieldOfView),
          aperture(aperture),
          focusDistance(focusDistance) {

        updateBasis();
    }


    // ========================================================
    // UPDATE CAMERA BASIS
    // ========================================================

    void updateBasis() {

        forward =
            (target - position).normalized();


        right =
            Vec3::cross(
                forward,
                worldUp
            ).normalized();


        up =
            Vec3::cross(
                forward,
                right
            ).normalized();
    }


    // ========================================================
    // LOOK AT
    // ========================================================

    void lookAt(
        const Vec3& newTarget
    ) {

        target =
            newTarget;

        updateBasis();
    }


    // ========================================================
    // SET POSITION
    // ========================================================

    void setPosition(
        const Vec3& newPosition
    ) {

        position =
            newPosition;

        updateBasis();
    }


    // ========================================================
    // SET FIELD OF VIEW
    // ========================================================

    void setFOV(
        double newFieldOfView
    ) {

        fieldOfView =
            newFieldOfView;
    }


    // ========================================================
    // SET ASPECT RATIO
    // ========================================================

    void setAspectRatio(
        double newAspectRatio
    ) {

        if (newAspectRatio > 0.0) {

            aspectRatio =
                newAspectRatio;
        }
    }


    // ========================================================
    // SET APERTURE
    // ========================================================

    void setAperture(
        double newAperture
    ) {

        aperture =
            std::max(
                0.0,
                newAperture
            );
    }


    // ========================================================
    // SET FOCUS DISTANCE
    // ========================================================

    void setFocusDistance(
        double newFocusDistance
    ) {

        focusDistance =
            std::max(
                0.001,
                newFocusDistance
            );
    }


    // ========================================================
    // GET RAY
    //
    // lensX and lensY are coordinates on the lens.
    // They should be in the range [-1, +1].
    // ========================================================

    Ray getRay(
        double u,
        double v,
        double lensX = 0.0,
        double lensY = 0.0
    ) const {

        const double pi =
            3.14159265358979323846;


        // ====================================================
        // FIELD OF VIEW
        // ====================================================

        double fovRadians =
            fieldOfView
            * pi
            / 180.0;


        double halfHeight =
            std::tan(
                fovRadians * 0.5
            );

        double halfWidth =
            aspectRatio
            * halfHeight;


        // ====================================================
        // IMAGE PLANE COORDINATES
        // ====================================================

        double horizontal =
            (2.0 * u - 1.0)
            * halfWidth;

        double vertical =
            (2.0 * v - 1.0)
            * halfHeight;


        // ====================================================
        // DIRECTION THROUGH IMAGE PLANE
        // ====================================================

        Vec3 rayDirection =
            forward
            + right * horizontal
            + up * vertical;


        rayDirection =
            rayDirection.normalized();


        // ====================================================
        // FOCUS POINT
        // ====================================================

        double directionAlignment =
            Vec3::dot(
                rayDirection,
                forward
            );


        directionAlignment =
            std::max(
                0.001,
                directionAlignment
            );


        double focusT =
            focusDistance
            / directionAlignment;


        Vec3 focusPoint =
            position
            + rayDirection * focusT;


        // ====================================================
        // LENS OFFSET
        // ====================================================

        double lensRadius =
            aperture * 0.5;


        Vec3 lensOffset =
            right * (
                lensX * lensRadius
            )
            +
            up * (
                lensY * lensRadius
            );


        // ====================================================
        // FINAL DEPTH-OF-FIELD RAY
        // ====================================================

        Vec3 rayOrigin =
            position
            + lensOffset;


        Vec3 finalDirection =
            (
                focusPoint
                - rayOrigin
            ).normalized();


        return Ray(
            rayOrigin,
            finalDirection
        );
    }
};