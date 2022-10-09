#ifndef FBX_WRAPPER_UESTRUCTS_H
#define FBX_WRAPPER_UESTRUCTS_H

#include <math.h>

#define PI (3.1415926535897932f)

typedef struct {
    float X;
    float Y;
} FVector2D;

typedef struct {
    float X;
    float Y;
    float Z;
} FVector;

typedef struct {
    float X;
    float Y;
    float Z;
    float W;
} FVector4;

typedef struct {
    float Pitch;
    float Yaw;
    float Roll;

    static float ClampAxis(float Angle) {
        // returns Angle in the range (-360,360)
        Angle = std::fmod(Angle, 360.f);
        if (Angle < 0.f) {
            // shift to [0,360) range
            Angle += 360.f;
        }
        return Angle;
    }

    static float NormalizeAxis(float Angle) {
        // returns Angle in the range [0,360)
        Angle = ClampAxis(Angle);
        if (Angle > 180.f) {
            // shift to (-180,180]
            Angle -= 360.f;
        }
        return Angle;
    }
} FRotator;

typedef struct {
    float X;
    float Y;
    float Z;
    float W;

    // reference
    // https://github.com/EpicGames/UnrealEngine/blob/1e5926084bbf386041103735ed6c2ab27bc1c1ee/Engine/Source/Runtime/Core/Private/Math/UnrealMath.cpp#L589
    FVector Euler() const {
        const float SingularityTest = Z * X - W * Y;
        const float YawY = 2.f * (W * Z + X * Y);
        const float YawX = (1.f - 2.f * ((Y * Y) + (Z * Z)));

        const float SINGULARITY_THRESHOLD = 0.4999995f;
        const float RAD_TO_DEG = (180.f) / PI;
        FRotator RotatorFromQuat;

        if (SingularityTest < -SINGULARITY_THRESHOLD) {
            RotatorFromQuat.Pitch = -90.f;
            RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = FRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
        } else if (SingularityTest > SINGULARITY_THRESHOLD) {
            RotatorFromQuat.Pitch = 90.f;
            RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = FRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
        } else {
            RotatorFromQuat.Pitch = std::asin(2.f * (SingularityTest)) * RAD_TO_DEG;
            RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = std::atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * ((X * X) + (Y * Y)))) * RAD_TO_DEG;
        }

        FVector VectorFromRotator;
        VectorFromRotator.X = RotatorFromQuat.Pitch;
        VectorFromRotator.Y = RotatorFromQuat.Yaw;
        VectorFromRotator.Z = RotatorFromQuat.Roll;
        return VectorFromRotator;
    }
} FQuat;

typedef struct {
    FQuat Rotation;
    FVector Translation;
    FVector Scale3D;
} FTransform;

#endif //FBX_WRAPPER_UESTRUCTS_H
