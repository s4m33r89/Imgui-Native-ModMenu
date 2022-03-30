#pragma once

#include <string>
#include <unordered_set>
#include "Offsets.h"
#include "Tools.h"
#include "UE4.h"

    
class Global {
   
public:

    static void onFirstTimeOnLobby();

    static uintptr_t GetGName();

    static const char *GetFNameFromID(uint32_t id);

    static const char *GetFullClassname(uintptr_t addr);

    static MinimalViewInfo GetPOV();
    
    static FMinimalViewInfo GetPOVV();

    static Vector3 WorldToScreen(Vector3 pos);
    
    static Vector33 WorldToScreen2(Vector33 pos); 

    static FTransform GetBoneTransform(uintptr_t entity, int idx);

    static FTransform GetComponentToWorld(uintptr_t entity);

    static Vector3 GetBoneLocation(uintptr_t Player, int boneIdx);

    static Vector33 GetBoneWithRotation(uintptr_t Mesh, int id);

    static Vector3 GetHeadLocation(uintptr_t entity);

    int GetBotsCount();
    
    static int GetEnemiesCount();

    static const char *GetVehicleName(const char *entityClass);

    static bool IsPlane(uintptr_t entity);
    
    static bool IsVisible(uintptr_t Player);
    
};
template<typename T = uintptr_t>
T ReadType(uintptr_t addr) {
    T result{};
    if (Tools::PVM_ReadAddr((void *) addr, &result, sizeof(T))) {
        return result;
    }
    return result;
}
template<typename T>
void Write2(uintptr_t addr, T value) {
    Tools::PVM_WriteAddr((void *) addr, &value, sizeof(T));
}

bool Global::IsPlane(uintptr_t entity) {
    bool IsPlayerInPlane = ReadType(entity + 0x748) || ReadType(entity + 0x754);
    return IsPlayerInPlane;
}

void Global::onFirstTimeOnLobby() {
    LOGI(__FUNCTION__);
}

    
bool Global::IsVisible(uintptr_t Player) 
{
    auto SkeletalMeshComponent = ReadType(Player + Offsets::Character::Mesh);
    float LastRenderTime = ReadType<float>(SkeletalMeshComponent + 0x2fc);
    float LastSubmitTime = ReadType<float>(SkeletalMeshComponent + 0x2f8);
    const float VisionTick = 0.06f;
    return LastSubmitTime + VisionTick >= LastRenderTime;
}

uintptr_t Global::GetGName() {
    return ReadType(ReadType(g_UE4 + UE4::Gname ) + 0xC0);
}

const char *Global::GetFNameFromID(uint32_t id) {
    uintptr_t GName = Global::GetGName();
    if (GName) {
        uintptr_t secPartAddr = ReadType(GName + (id / 0x4000) * UE4::PointerSize);
        if (secPartAddr) {
            uintptr_t nameAddr = ReadType(secPartAddr + (id % 0x4000) * UE4::PointerSize);
            if (Tools::IsPtrValid((void *) nameAddr)) {
                return (const char *) (nameAddr + UE4::Name);
            }
        }
    }
    return 0;
}

const char *Global::GetFullClassname(uintptr_t addr) {
    uintptr_t GName = (Global::GetGName());
    if (GName) {

        uintptr_t ClassPrivate = ReadType(addr + UE4::ClassPrivate);
        if (ClassPrivate) {
            auto FNameID = ReadType<uint32_t>(ClassPrivate + UE4::FNameID);

            auto clazz = Global::GetFNameFromID(FNameID);
            if (!clazz)
                return 0;

            std::string classname = clazz;

            auto Child = ReadType(ClassPrivate + UE4::Child);
            while (Child) {
                FNameID = ReadType<uint32_t>(Child + UE4::FNameID);

                clazz = Global::GetFNameFromID(FNameID);
                if (!clazz)
                    return "";

                classname += ".";
                classname += clazz;

                Child = ReadType(Child + UE4::Child);
            }

            return classname.c_str();
        }
    }
    return 0;
}
MinimalViewInfo Global::GetPOV() {
    if (PData::PlayerController) {
        auto PlayerCameraManager = ReadType(PData::PlayerController + Offsets::PlayerController::PlayerCameraManager);
        if (PlayerCameraManager) {
            CameraCacheEntry CameraCache = ReadType<CameraCacheEntry>(PlayerCameraManager + Offsets::PlayerCameraManager::CameraCache);
            return CameraCache.POV;
        }
    }
    return {};
}

FMinimalViewInfo Global::GetPOVV() {
    if (PData::PlayerController) {
        auto PlayerCameraManager = ReadType(PData::PlayerController + Offsets::PlayerController::PlayerCameraManager);
        if (PlayerCameraManager) {
            CameraCacheEntry2 CameraCached = ReadType<CameraCacheEntry2>(PlayerCameraManager + Offsets::PlayerCameraManager::CameraCache);
            return CameraCached.POV;
        }
    }
    return {};
}

Vector3 Global::WorldToScreen(Vector3 worldLocation) {
    MinimalViewInfo camViewInfo = Global::GetPOV();

    FMatrix tempMatrix = RotatorToMatrix(camViewInfo.Rotation);

    Vector3 vAxisX(tempMatrix.M[0][0], tempMatrix.M[0][1], tempMatrix.M[0][2]);
    Vector3 vAxisY(tempMatrix.M[1][0], tempMatrix.M[1][1], tempMatrix.M[1][2]);
    Vector3 vAxisZ(tempMatrix.M[2][0], tempMatrix.M[2][1], tempMatrix.M[2][2]);

    Vector3 vDelta = worldLocation - camViewInfo.Location;

    Vector3 vTransformed(Vector3::Dot(vDelta, vAxisY), Vector3::Dot(vDelta, vAxisZ), Vector3::Dot(vDelta, vAxisX));

    float fov = camViewInfo.FOV;
    float screenCenterX = (glWidth / 2.0f);
    float screenCenterY = (glHeight / 2.0f);

    float X = (screenCenterX + vTransformed.X * (screenCenterX / tanf(fov * ((float)3.14159265358979323846 / 360.0f))) / vTransformed.Z);
    float Y = (screenCenterY - vTransformed.Y * (screenCenterX / tanf(fov * ((float)3.14159265358979323846 / 360.0f))) / vTransformed.Z);
    float Z = vTransformed.Z;

    return {X, Y, Z};
}

Vector33 Global::WorldToScreen2(Vector33 worldLocation) {
    FMinimalViewInfo camViewInfo = Global::GetPOVV();

    FMatrix tempMatrix = RotatorToMatrix(camViewInfo.Rotation);

    Vector33 vAxisX(tempMatrix.M[0][0], tempMatrix.M[0][1], tempMatrix.M[0][2]);
    Vector33 vAxisY(tempMatrix.M[1][0], tempMatrix.M[1][1], tempMatrix.M[1][2]);
    Vector33 vAxisZ(tempMatrix.M[2][0], tempMatrix.M[2][1], tempMatrix.M[2][2]);

    Vector33 vDelta = worldLocation - camViewInfo.Location;

    Vector33 vTransformed = Vector33(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));


    float fov = camViewInfo.FOV;
    float screenCenterX = (glWidth / 2.0f);
    float screenCenterY = (glHeight / 2.0f);

    float x = (screenCenterX + vTransformed.x * (screenCenterX / tanf(fov * ((float)3.14159265358979323846 / 360.0f))) / vTransformed.z);
    float y = (screenCenterY - vTransformed.y * (screenCenterX / tanf(fov * ((float)3.14159265358979323846 / 360.0f))) / vTransformed.z);
    float z = vTransformed.z;

    return {x, y, z};
}

bool WorldToScreen(Vector3 WorldLocation, Vector3& Screenlocation)
    {
        MinimalViewInfo camViewInfo = Global::GetPOV();

        FMatrix tempMatrix = RotatorToMatrix(camViewInfo.Rotation);
 
        Vector3 vAxisX, vAxisY, vAxisZ;
 
        vAxisX = Vector3(tempMatrix.M[0][0], tempMatrix.M[0][1], tempMatrix.M[0][2]);
        vAxisY = Vector3(tempMatrix.M[1][0], tempMatrix.M[1][1], tempMatrix.M[1][2]);
        vAxisZ = Vector3(tempMatrix.M[2][0], tempMatrix.M[2][1], tempMatrix.M[2][2]);
 
        Vector3 vDelta = WorldLocation - camViewInfo.Location;
        Vector3 vTransformed = Vector3(vDelta.Dot(vDelta, vAxisY), vDelta.Dot(vDelta, vAxisZ), vDelta.Dot(vDelta, vAxisX));
 
        if (vTransformed.Z < 0.0001f)
            return false;
 
        float FovAngle = camViewInfo.FOV;
 
        float ScreenCenterX = glWidth / 2;
        float ScreenCenterY = glHeight / 2;
 
        Screenlocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
        Screenlocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
        return true;
	}
    
FTransform Global::GetBoneTransform(uintptr_t entity, int idx) {
    auto Mesh = ReadType(entity + 0x340);
    if (Mesh) {
        uintptr_t Bones = ReadType(Mesh + 0x5e0);
        if (Bones) {
            return ReadType<FTransform>(Bones + (idx * 48));
        }
    }
    return {};
}


FTransform Global::GetComponentToWorld(uintptr_t entity) {
    uintptr_t Mesh = ReadType(entity + Offsets::Character::Mesh);
    if (Mesh) {
        return ReadType<FTransform>(Mesh + Offsets::SceneComponent::ComponentToWorld);
    }
    return {};
}

Vector3 Global::GetBoneLocation(uintptr_t Player, int boneIdx) {
    if (Player) {
        return TransformToLocation(Global::GetComponentToWorld(Player), Global::GetBoneTransform(Player, boneIdx));
    }
    return {};
}

FTransform GetBoneIndex(uintptr_t entity, int index){
    auto Mesh = ReadType(entity + 0x340);
    uintptr_t bonearray = ReadType<uint32_t>(Mesh + 0x5e0);
    return ReadType<FTransform>(bonearray + (index * 48));
}

Vector33 Global::GetBoneWithRotation(uintptr_t entity, int id) {
    auto Mesh = ReadType(entity + 0x340);
    FTransform bone = GetBoneIndex(Mesh, id);
    FTransform ComponentToWorld_ =  ReadType<FTransform>(Mesh + Offsets::SceneComponent::ComponentToWorld);
    D3DMatrix Matrix;
    Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld_.ToMatrixWithScale());
    return Vector33(Matrix._41, Matrix._42, Matrix._43);
}




Vector3 Global::GetHeadLocation(uintptr_t entity) {
    return Global::GetBoneLocation(entity, BoneID::Head);
}

int Global::GetEnemiesCount() {
    int result = 0;
    for (uintptr_t Player : PData::Players) {
        if (Player) {
            if (PData::LocalPlayer) {
                auto TeamID = ReadType<int>(Player + Offsets::UAECharacter::TeamID);
                auto MyTeamID = ReadType<int>(PData::LocalPlayer + Offsets::UAECharacter::TeamID);
                if (TeamID == MyTeamID)
                    continue;
            }

            auto Head = Global::GetHeadLocation(Player);
            float distance = (Vector3::Distance(Head, Global::GetPOV().Location) / 100.0f);

            if (distance <= 500.f)
                result++;
        }
    }
    return result;
}


const char *Global::GetVehicleName(const char *entityClass) {
    if (!entityClass)
        return 0;

    if (strstr(entityClass, "BRDM"))
        return "BRDM";
    if (strstr(entityClass, "Scooter"))
        return "Scooter";
    if (strstr(entityClass, "Motorcycle"))
        return "Motorcycle";
    if (strstr(entityClass, "Motorcycle"))
        return "Motorcycle Cart";
    if (strstr(entityClass, "Snow Mobile"))
        return "Snow Mobile";
    if (strstr(entityClass, "Tuk"))
        return "Tuk";
    if (strstr(entityClass, "Buggy"))
        return "Buggy";
    if (strstr(entityClass, "open"))
        return "Sports";
    if (strstr(entityClass, "close"))
        return "Sports";
    if (strstr(entityClass, "Dacia"))
        return "Dacia";
    if (strstr(entityClass, "Rony"))
        return "Rony";
    if (strstr(entityClass, "UAZ"))
        return "UAZ";
    if (strstr(entityClass, "MiniBus"))
        return "Minibus";
    if (strstr(entityClass, "PG117"))
        return "PG117";
    if (strstr(entityClass, "AquaRail"))
        return "Aquarail";
    if (strstr(entityClass, "AirDropPlane"))
        return "Airplane";
    if (strstr(entityClass, "Snowbike"))
        return "Snow Bike";

    return "Vehicle";
}
