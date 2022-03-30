#include "Includes.h"

bool isInsideFOV(int x, int y) {
    if (!Config.SilentAim.Cross)
        return true;

    int circle_x = screenWidth / 2;
    int circle_y = screenHeight / 2;
    int rad = Config.SilentAim.Cross;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

uintptr_t GetBestAimTargetFOV() {
    uintptr_t Target = 0;
    float max = std::numeric_limits<float>::infinity();

    for (uintptr_t Player : PData::Players) {
        if (Player) {
            auto Health = ReadType<float>(Player + Offsets::STExtraCharacter::Health);
            auto bDead = ReadType<bool>(Player + Offsets::STExtraCharacter::bDead);
            auto Mesh = ReadType(Player + Offsets::Character::Mesh);

            auto TeamID = ReadType<int>(Player + Offsets::UAECharacter::TeamID);
            auto MyTeamID = ReadType<int>(PData::LocalPlayer + Offsets::UAECharacter::TeamID);

            if (TeamID == MyTeamID)
                continue;

            if (!Mesh)
                continue;

            if (bDead)
                continue;

            if (Config.SilentAim.IgnoreKnocked) {
            if (Health == 0.0f)
                continue;
                }         
            
            bool bVisible = true;
//        
            if (bVisible) {
                auto root = Global::WorldToScreen(Global::GetBoneLocation(Player, 0));
                auto head = Global::WorldToScreen(Global::GetHeadLocation(Player));

                float height = abs(head.Y - root.Y);
                float width = height * 0.65f;

                Vector3 middlePoint = {head.X + (width / 2), head.Y + (height / 2), 0};

                Vector2 v2Middle = Vector2((float) (screenWidth / 2), (float) (screenHeight / 2));
                Vector2 v2Loc = Vector2(middlePoint.X, middlePoint.Y);

                float distance = Vector2::Distance(v2Middle, v2Loc);

                if (isInsideFOV((int) middlePoint.X, (int) middlePoint.Y)) {
                    if (distance < max) {
                        max = distance;
                        Target = Player;
                    }
                }
            }
        }
    }
    return Target;
}
uintptr_t GetBestAimTarget() {
    uintptr_t Target = 0;
    float max = std::numeric_limits<float>::infinity();

    for (uintptr_t Player : PData::Players) {
        if (Player) {
            auto Health = ReadType<float>(Player + Offsets::STExtraCharacter::Health);
            auto bDead = ReadType<bool>(Player + Offsets::STExtraCharacter::bDead);
            auto Mesh = ReadType(Player + Offsets::Character::Mesh);

            auto TeamID = ReadType<int>(Player + Offsets::UAECharacter::TeamID);
            auto MyTeamID = ReadType<int>(PData::LocalPlayer + Offsets::UAECharacter::TeamID);

            if (TeamID == MyTeamID)
                continue;

            if (!Mesh)
                continue;

            if (bDead)
                continue;

            if (Config.SilentAim.IgnoreKnocked) {
            if (Health == 0.0f)
                continue;
                }         

            bool bVisible = true;
            if (bVisible) {
                Vector3 myPos = Global::GetBoneLocation(PData::LocalPlayer, 0);
                Vector3 rootPos = Global::GetBoneLocation(Player, 0);

                float distance = (Vector3::Distance(rootPos, myPos) / 100.0f);

                if (distance < max) {
                    max = distance;
                    Target = Player;
                }
            }
        }
    }
    return Target;
}


void *GameAimbotData(void *){
    while (true) {
        auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        auto RootComponent = ReadType(PData::LocalPlayer + Offsets::Actor::RootComponent);
        if (!RootComponent)
            continue;
            

        if (PData::LocalPlayer != 0 && PData::PlayerController) {
                
                auto WeaponManagerComponent = ReadType(PData::LocalPlayer + Offsets::STExtraBaseCharacter::WeaponManagerComponent);
                if (WeaponManagerComponent) {
                    auto CurrentWeaponReplicated = ReadType(WeaponManagerComponent + Offsets::STExtraWeapon::CurrentWeaponReplicated);
                    if (CurrentWeaponReplicated) {
                        auto ShootWeaponEntityComponent = ReadType(CurrentWeaponReplicated + Offsets::ShootWeaponEntity::ShootWeaponEntityComp);
                        auto ShootWeaponEffectComponent = ReadType(CurrentWeaponReplicated + Offsets::STExtraShootWeapon::ShootWeaponEffectComp);
                         if (ShootWeaponEntityComponent && ShootWeaponEffectComponent) {  
                         
         
                        if (Config.SilentsMemory.NoShake) {
                            Write2<float>(ShootWeaponEffectComponent + Offsets::ShootWeaponEffectComponent::CameraShakeInnerRadius, 0);
                            Write2<float>(ShootWeaponEffectComponent + Offsets::ShootWeaponEffectComponent::CameraShakeOuterRadius, 0);
                            Write2<float>(ShootWeaponEffectComponent + Offsets::ShootWeaponEffectComponent::CameraShakFalloff, 0);
                        }
                        if (Config.SilentsMemory.CrossHair) {
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::CrossHairInitialSize,0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::GameDeviationFactor, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::GameDeviationAccuracy, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::AccessoriesDeviationFactor, 0);
                           }
                                                          
                        if (Config.SilentsMemory.Less) {
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::BulletFireSpeed, 999999.f);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::AccessoriesVRecoilFactor, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::AccessoriesHRecoilFactor, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::AccessoriesRecoveryFactor, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::ShotGunCenterPerc, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::ShotGunVerticalSpread, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::ShotGunHorizontalSpread, 0);                        
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::VehicleWeaponDeviationAngle, 0);
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::RecoilKickADS, 0);
                        }
                        
                         if (Config.SilentsMemory.instahit) {
                            Write2<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::BulletFireSpeed, 600000.0f);
                            }
                            
                         if (Config.SilentAim.Predict) {
                            PData::mBulletSpeed = ReadType<float>(ShootWeaponEntityComponent + Offsets::ShootWeaponEntity::BulletFireSpeed);
                            }
                            
                                bool triggerOk = false;
                          if (Config.SilentAim.Trigger != EAimTrigger::None) {
                          if (Config.SilentAim.Trigger == EAimTrigger::Shooting) {
                                triggerOk = ReadType<bool>(PData::LocalPlayer + Offsets::STExtraBaseCharacter::bIsWeaponFiring);
                            } else if (Config.SilentAim.Trigger == EAimTrigger::Scoping) {
                                triggerOk = ReadType<bool>(PData::LocalPlayer + Offsets::STExtraCharacter::bIsGunADS);                      
                            } else if (Config.SilentAim.Trigger == EAimTrigger::Both) {
                                triggerOk = (ReadType<bool>(PData::LocalPlayer + Offsets::STExtraBaseCharacter::bIsWeaponFiring) || ReadType<bool>(PData::LocalPlayer + Offsets::STExtraCharacter::bIsGunADS));
                            } else if (Config.SilentAim.Trigger == EAimTrigger::Any) {
                                triggerOk = (ReadType<bool>(PData::LocalPlayer + Offsets::STExtraBaseCharacter::bIsWeaponFiring) && ReadType<bool>(PData::LocalPlayer + Offsets::STExtraCharacter::bIsGunADS));
                            }                       
                            } else triggerOk = true;
                           if (triggerOk) {                            
                                auto Target = GetBestAimTarget();
                                if (Config.SilentAim.By == EAim::Distance) {
                                   Target = GetBestAimTarget();
                                }     
                                if (Config.SilentAim.By == EAim::Croshhair) {
                                    Target = GetBestAimTargetFOV();
                                }
                                if (Target) {
                                    Vector3 targetAimPos = Global::GetBoneLocation(Target, BoneID::Head);
                                    Vector3 localPos = Global::GetPOV().Location;
                                if (Config.SilentAim.Target == EAimTarget::Chest) {
                                        targetAimPos = Global::GetBoneLocation(Target, BoneID::neck_01);
                                    }
                                if (Config.SilentAim.Target == EAimTarget::Heads) {
                                        targetAimPos = Global::GetBoneLocation(Target, BoneID::spine_02);
                                    }
                                    
                                       if (Config.SilentAim.Predict) {
                                        if (PData::mBulletSpeed > 0) {
                                            auto CurrentVehicle = ReadType(Target + Offsets::STExtraCharacter::CurrentVehicle);
                                            if (CurrentVehicle) {
                                                Vector3 LinearVelocity = ReadType<Vector3>(CurrentVehicle + Offsets::Actor::ReplicatedMovement);//0x80 ? ReplicatedMovement + 0x0

                                                float dist = Vector3::Distance(targetAimPos, Target);
                                                auto timeToTravel = dist / PData::mBulletSpeed;

                                                targetAimPos += LinearVelocity * timeToTravel;
                                            } else {
                                                auto STCharacterMovement = ReadType(Target + Offsets::STCharacterMovementComponent::STCharacterMovement);
                                                if (STCharacterMovement) {
                                                    Vector3 Velocity = ReadType<Vector3>(STCharacterMovement + Offsets::MovementComponent::Velocity);

                                                    float dist = Vector3::Distance(targetAimPos, Target);
                                                    auto timeToTravel = dist / PData::mBulletSpeed;

                                                    targetAimPos += Velocity * timeToTravel;
                                                }
                                            }
                                        }
                                    }

                                     if (Config.SilentAim.Aimbot) {
                                           auto ControlRotation = (float *) (PData::PlayerController + Offsets::Controller::ControlRotation);
                                           auto aimRotation = ToRotator(localPos, targetAimPos);
                                         if (Config.SilentAim.Smooth) {
                                            aimRotation.Pitch -= ControlRotation[0];
                                            aimRotation.Yaw -= ControlRotation[1];

                                            ClampAngles(aimRotation);

                                            ControlRotation[0] += aimRotation.Pitch / (float) 0.5f;
                                            ControlRotation[1] += aimRotation.Yaw / (float) 0.5f;
                                        } else {
                                            ClampAngles(aimRotation);
                                            ControlRotation[0] = aimRotation.Pitch;
                                            ControlRotation[1] = aimRotation.Yaw;
                                        }
                                    }
                        
                                if (Config.SilentAim.Bulletrack) {
                                        //360
                                        auto STPlayerController = ReadType(PData::LocalPlayer + Offsets::STExtraPlayerCharacter::STPlayerController);
                                        if (STPlayerController) {
                                            auto PlayerCameraManager = ReadType(STPlayerController + Offsets::PlayerController::PlayerCameraManager);
                                            if (PlayerCameraManager) {
                                                CameraCacheEntry CameraCache = ReadType<CameraCacheEntry>(PlayerCameraManager + Offsets::PlayerCameraManager::CameraCache);
                                                Vector3 currViewAngle = Global::GetPOV().Location;
                                                FRotator aimRotation = ToRotator(currViewAngle, targetAimPos);
                                                CameraCache.POV.Rotation = aimRotation;
                                                Tools::writeValue(PlayerCameraManager + Offsets::PlayerCameraManager::CameraCache, &CameraCache, sizeof(CameraCacheEntry));
                                            }
                                       }
                                  }                                
                             }
                         }
                     }
                 }
             }
         }
        auto td = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(std::min(0LL, SLEEP_TIME - td), SLEEP_TIME)));
    
       }
    return 0;
}   

