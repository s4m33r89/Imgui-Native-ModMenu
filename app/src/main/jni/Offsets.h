namespace UE4 {  
    constexpr auto Gworld = 0x78CE350;
    constexpr auto Gname = 0x75FCE50; 
    constexpr auto PointerSize = 0x4;
    constexpr auto InternalIndex = 0x8;
    constexpr auto ClassPrivate = 0xC;
    constexpr auto FNameID = 0x10;
    constexpr auto Child = 0x20;
    constexpr auto Name = 0x8;

}
namespace Offsets {
    namespace GameState {
        constexpr auto ElapsedTime = 0x340;
    }
    namespace Level {
        constexpr auto EntityList = 0x70;
        constexpr auto EntityListSize = EntityList + UE4::PointerSize;
    } 
    namespace PlayerCameraManager {
        constexpr auto CameraCache = 0x370;
    }
    namespace STExtraBaseCharacter {
        constexpr auto NearDeatchComponent = 0x1190;
        constexpr auto NearDeathBreath = 0x11a4;
        constexpr auto BreathMax = 0x104;
        constexpr auto bIsWeaponFiring = 0xf20;
        constexpr auto WeaponManagerComponent = 0x16d0;
    }
    namespace STCharacterMovementComponent {
        constexpr auto STCharacterMovement = 0x1340;
    }

    namespace STExtraWeapon {
        constexpr auto CurrentWeaponReplicated = 0x464;
    }
    namespace STExtraPlayerCharacter {
        constexpr auto STPlayerController = 0x29fc;
    }
    namespace PrimitiveComponent {
        constexpr auto LastSubmitTime = 0x300;
        constexpr auto LastRenderTime = 0x304;
        constexpr auto LastRenderTimeOnScreen = 0x308;
    }
    namespace STExtraCharacter {
        constexpr auto Health = 0x95c;
        constexpr auto HealthMax = 0x960;
        constexpr auto bDead = 0x998;
        constexpr auto CurrentVehicle = 0x9b8;
        constexpr auto bIsGunADS = 0xb35;
    }
    namespace PlayerController {
        constexpr auto PlayerCameraManager = 0x360;
        constexpr auto MyHUD = 0x35c;
    }
    namespace UAEPlayerController {
        constexpr auto PlayerKey = 0x64c;
    }
    namespace UAECharacter {
        constexpr auto TeamID = 0x690;
        constexpr auto PlayerName = 0x668;
        constexpr auto bIsAI = 0x708;
        constexpr auto PlayerKey = 0x680;
    }
    namespace Character {
        constexpr auto Mesh = 0x340;
    }

    namespace Actor {
        constexpr auto RootComponent = 0x16c;
        constexpr auto ReplicatedMovement = 0x80;
    }

    namespace MovementComponent {
        constexpr auto Velocity = 0xC8;
    }

    namespace Controller {
        constexpr auto ControlRotation = 0x318;
    }
    
    namespace StaticMeshComponent {
        constexpr auto MinLOD = 0x5e0;
    }

    namespace PickUpListWrapperActor {
        constexpr auto PickUpDataList = 0x684;
    }

    namespace PlayerTombBox {
        constexpr auto BoxPickupWrapperActor = 0x454;
    }

    namespace PickUpWrapperActor {
        constexpr auto ItemDefineID_TypeSpecificID = 0x3e8 + 0x4;
    }

    namespace STExtraShootWeapon {
        constexpr auto ShootWeaponComponent = 0xa50;
        constexpr auto ShootWeaponEffectComp = 0xb74;
    }
    namespace ShootWeaponEntity {
        constexpr auto CrossHairInitialSize = 0x8b4;
        
        constexpr auto ShootWeaponEntityComp = 0xb70;
        constexpr auto ShootWeaponEntityComponent = 0x118;
        constexpr auto BulletFireSpeed = 0x404;

        constexpr auto AccessoriesVRecoilFactor = 0x848;
        constexpr auto AccessoriesHRecoilFactor = 0x84c;
        constexpr auto AccessoriesRecoveryFactor = 0x850;


        constexpr auto ShotGunCenterPerc = 0x898;
        constexpr auto ShotGunVerticalSpread = 0x89c;
        constexpr auto ShotGunHorizontalSpread = 0x8a0;
        constexpr auto GameDeviationFactor = 0x8a4;
        constexpr auto GameDeviationAccuracy = 0x8a8;
        constexpr auto AccessoriesDeviationFactor = 0x8ac;

        constexpr auto VehicleWeaponDeviationAngle = 0x8c0;
        constexpr auto RecoilKickADS = 0x950;
    }

    namespace ShootWeaponEffectComponent {
        constexpr auto CameraShakeInnerRadius = 0x198;
        constexpr auto CameraShakeOuterRadius = 0x19c;
        constexpr auto CameraShakFalloff = 0x1a0;
    }



    namespace SceneComponent {
        constexpr auto RelativeLocation = 0x118;
        constexpr auto RelativeRotation = 0x124;
        constexpr auto ComponentToWorld = 0x140;
        constexpr auto absoluteLocation = ComponentToWorld + 0x10;
    }
    
}

