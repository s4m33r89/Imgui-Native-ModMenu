#include "Includes.h"
#include "Tools.h"
#include "fake_dlfcn.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "StrEnc.h"
#include "Spoof.h"
#include "plthook.h"
#include "Items.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "json.hpp"
using json = nlohmann::json;
#define SLEEP_TIME 1000LL / 60LL

uintptr_t g_UE4;
uintptr_t SilentAim_Offset;
int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
json items_data;

time_t expiredDate = 0;
std::string g_Token, g_Auth;
bool bValid = false;
bool pink = false;
bool show_another_window = false;
bool show_esp_window = false;
bool rectangle = true;
bool border = false;


#include "Colors.h"
#include "BoneID.h"
#include "PData.h"
#include "Global.h"

#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "base64/base64.h"


bool initImGui = false;
bool isCompleted = true;

float density = -1;
std::map<uintptr_t, int> GrenadesCD;


void *GameUpdateData(void *){
        while (isCompleted) {
            auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            auto World = ReadType(g_UE4 + UE4::Gworld);
                
               if (World) {
                uintptr_t PersistentLevel = ReadType(ReadType(World + 0xB8) + 0x20);
                if (PersistentLevel) {
                   
                    auto entities = ReadType<TArray<uintptr_t>>(PersistentLevel + 0x70);
                    
                    uintptr_t PlayerController = 0;
                    uintptr_t PlayerCameraManager = 0;
                    std::vector<uintptr_t> Entities;
                    std::vector<uintptr_t> Players;
                    std::vector<uintptr_t> Boxes;
                    std::vector<uintptr_t> Airdrops;
                    std::vector<uintptr_t> Vehicles;
                    std::vector<uintptr_t> Grenades;
                    std::vector<uintptr_t> Pickups;
                    std::vector<uintptr_t> GamePlaneCharacter;

                    for (int i = 0; i < entities.Count; i++) {
                        if (Tools::IsPtrValid((void *) entities[i])) {
                            uintptr_t entity = entities[i];
                            if (entity) {
                            Entities.push_back(entity);
                                auto resName = Global::GetFullClassname(entity);
                                if (resName) {
                                    if (strstr(resName, "BP_PlayerLobbyPawn_C")) {
                                        static bool b = false;
                                        if (!b) {
                                            Global::onFirstTimeOnLobby();
                                            b = true;
                                        }
                                    }

                                    
                                    if (strstr(resName, "GameStateBase")) {
                                        PData::GameState = entity;
                                    }
                                  	if (strstr(resName, "BP_PlayerPawn") || strstr(resName, "BP_PlayerCharacter") || strstr(resName, "PlanET_FakePlayer")) {
                                        Players.push_back(entity);
                                    }

                                    if (strstr(resName,"BP_Grenade_Shoulei_C") || strstr(resName,"BP_Grenade_Burn_C")) {
                                        Grenades.push_back(entity);
                                    }

                                    if (strstr(resName, "PickUpWrapperActor")) {
                                        Pickups.push_back(entity);
                                    }
                                    if (strstr(resName, "BP_STExtraPlayerController_C")){
                                        PlayerController = entity;
                                    }
                                    if (strstr(resName, "BP_PlayerCameraManager")) {
                                        PlayerCameraManager = entity;
                                    }
                                    if (strstr(resName, "PickUpListWrapperActor") || strstr(resName,"PlayerDeadInventoryBox")) {
                                        Boxes.push_back(entity);
                                    }                               
                                    if (strstr(resName, "BP_AirDropBox_C")) {
                                        Airdrops.push_back(entity);
                                    }
                                    if (strstr(resName, "STExtraVehicleBase")) {
                                        Vehicles.push_back(entity);
                                    }

                                }
                            }
                        }
                    }


                    if (PlayerController) {
                        auto localPlayerKey = ReadType<uint32_t>(PlayerController + Offsets::UAEPlayerController::PlayerKey);
                        for (uintptr_t Player : Players) {
                            if (Player) {
                                auto PlayerKey = ReadType<uint32_t>(Player + Offsets::UAECharacter::PlayerKey);
                                if (PlayerKey == localPlayerKey) {
                                    PData::LocalPlayer = Player;
                                }
                            }
                        }
                    } else {
                        PData::LocalPlayer = 0;
                    }

                    PData::PlayerController = PlayerController;
                    PData::PlayerCameraManager = PlayerCameraManager;
                    PData::Entities = Entities;
                    PData::Players = Players;
                    PData::Boxes = Boxes;
                    PData::Airdrops = Airdrops;
                    PData::Vehicles = Vehicles;
                    PData::Grenades = Grenades;

                    PData::Pickups = Pickups;
                    
                   
                }
            }
            auto td = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
            std::this_thread::sleep_for(std::chrono::milliseconds(std::max(std::min(0LL, SLEEP_TIME - td), SLEEP_TIME)));
        }
    return 0;
}
// ======================================================================== //
//vishesh
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback  GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback         GImGuiDemoMarkerCallback = NULL;
void* GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)
//vishesh
/////////////////////////////////////////////////////////////////////////////////
//vishesh
ImGuiStyle& style = ImGui::GetStyle();
static ImGuiStyle ref_saved_style;

//vishesh
// ======================================================================== //

// ======================================================================== //


enum EAimTarget {
    Heads = 0,
    Chest = 1
};
   
enum EAimTrigger {
    None = 0,
    Shooting = 1,
    Scoping = 2,
    Both = 3,
    Any = 4
};
std::map<int, bool> Items;
enum EAim {
  Distance = 0,
  Croshhair = 1
};
    
struct sConfig {
    struct sESPMenu {
        bool Line;
        bool Box;          
        bool Health;
        bool ShowKnockedHealth;
        bool Name;
        bool Distance;
        bool TeamID;
        bool Radar;
        bool Skeleton;
    };
    sESPMenu ESPMenu{0};
    
   struct sAimMenu {
        bool Aimbot;
        bool Bulletrack;
        bool Smooth;
        float Cross;
        EAim By;
        EAimTarget Target;
        EAimTrigger Trigger;
        bool IgnoreKnocked;
        bool Predict;
    };
    sAimMenu SilentAim{0};
    
    struct sMemoryMenu {
        bool Less;
        bool CrossHair;
        bool NoShake;
        bool instahit;   
    };
    sMemoryMenu SilentsMemory{0};
    
    struct sOTHER {
        bool FPS;
        bool EXPIRYTIME;
    };
    sOTHER OTHER{0};
    
    struct sColorsESP {
        float *Line;
        float *Box;
        float *Name;
        float *Distance;
        float *Skeleton;
    };
    sColorsESP ColorsESP{0};
};
sConfig Config{0};

#define CREATE_COLOR(r, g, b, a) new float[4] {(float)r, (float)g, (float)b, (float)a};
#include "Aimbot.h"

android_app *g_App = 0;

struct sRegion {
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;


int32_t ToColor(float *col) {
    return ImGui::ColorConvertFloat4ToU32(*(ImVec4 *) (col));
}


Vector3 getLocation(uintptr_t scenecomp){
   return ReadType<Vector3>(scenecomp + Offsets::SceneComponent::ComponentToWorld + 0x10);
        
 }
class FPSCounter {
protected:
    unsigned int m_fps;
    unsigned int m_fpscount;
    long m_fpsinterval;

public:
    FPSCounter() : m_fps(0), m_fpscount(0), m_fpsinterval(0) {
    }

    void update() {
        m_fpscount++;

        if (m_fpsinterval < time(0)) {
            m_fps = m_fpscount;

            m_fpscount = 0;
            m_fpsinterval = time(0) + 1;
        }
    }

    unsigned int get() const {
        return m_fps;
    }
};

FPSCounter fps;

Vector33 WorldToRadar(float Yaw, Vector33 Origin, Vector33 LocalOrigin, float PosX, float PosY, Vector33 Size, bool &outbuff) {
 bool flag = false;
 double num = (double)Yaw;
 double num2 = num * 0.017453292519943295;
 float num3 = (float)std::cos(num2);
 float num4 = (float)std::sin(num2);
 float num5 = Origin.x - LocalOrigin.x;
 float num6 = Origin.y - LocalOrigin.y;
 struct Vector33 Xector;
 Xector.x = (num6 * num3 - num5 * num4) / 150.f;
 Xector.y = (num5 * num3 + num6 * num4) / 150.f;
 struct Vector33 Xector2;
 Xector2.x = Xector.x + PosX + Size.x / 2.f;
 Xector2.y = -Xector.y + PosY + Size.y / 2.f;
 bool flag2 = Xector2.x > PosX + Size.x;
 if (flag2) {
  Xector2.x = PosX + Size.x;
 }else{
  bool flag3 = Xector2.x < PosX;
  if (flag3) {
   Xector2.x = PosX;
  }
 }
 bool flag4 = Xector2.y > PosY + Size.y;
 if (flag4) {
  Xector2.y = PosY + Size.y;
 }else{
  bool flag5 = Xector2.y < PosY;
  if (flag5){
   Xector2.y = PosY;
  }
 }
 bool flag6 = Xector2.y == PosY || Xector2.x == PosX;
 if (flag6){
  flag = true;
 }
 outbuff = flag;
 return Xector2;
}

void DrawESP(ImDrawList *draw) {
     
    int pCount = 0, bCount = 0;
        
    std::string sFPS = "EMPIRE CHEATS v1.9 FPS: ";
    sFPS += std::to_string(fps.get());

    draw->AddText({((float) density / 10.0f), 40},IM_COL32(255, 155, 0, 255),sFPS.c_str());
    if (Config.OTHER.FPS) {
        draw->AddText({((float) density / 10.0f), 40},IM_COL32(255, 000, 000, 255),sFPS.c_str());
    }

    if (Config.OTHER.EXPIRYTIME) {
        static std::string sExpiredDate = "";
        if (sExpiredDate.empty()) {
            auto lt = localtime(&expiredDate);

            char fmt[512];
            strftime(fmt, sizeof(fmt), "%F %r", lt);

            sExpiredDate = "Key Expiry: ";
            sExpiredDate += fmt;
        }

        if (Config.OTHER.EXPIRYTIME) {
            draw->AddText({((float) density / 2.0f), 40}, IM_COL32(255, 000, 000, 255),
                          sExpiredDate.c_str());
  }
              }
        for (uintptr_t Player : PData::Players) {
            if (Tools::IsPtrValid((void *) Player)) {
                auto TeamID = ReadType<int>(Player + Offsets::UAECharacter::TeamID);
                auto Health = ReadType<float>(Player + Offsets::STExtraCharacter::Health);
                auto HealthMax = ReadType<float>(Player + Offsets::STExtraCharacter::HealthMax);
                auto bIsAI = ReadType<bool>(Player + Offsets::UAECharacter::bIsAI);
                auto PlayerName = ReadType<FString>(Player + Offsets::UAECharacter::PlayerName);
                auto bDead = ReadType<bool>(Player + Offsets::STExtraCharacter::bDead);
                auto Mesh = ReadType(Player + Offsets::Character::Mesh);              
                

                auto RootComponent = ReadType(Player + Offsets::Actor::RootComponent);
                if (!RootComponent)
                    continue;

                if (!Mesh)
                    continue;

                if (PData::LocalPlayer) {
                    auto LocalKey = ReadType<int>(PData::LocalPlayer + Offsets::UAEPlayerController::PlayerKey);
                    auto PlayerKey = ReadType<int>(Player + Offsets::UAECharacter::PlayerKey);
                    
                    if (PlayerKey == LocalKey)
                        continue;

                    auto MyTeamID = ReadType<int>(PData::LocalPlayer + Offsets::UAECharacter::TeamID);
                    if (TeamID == MyTeamID)
                        continue;
                }

                if (bDead)
                    continue;

                if (bIsAI)
                    bCount++;
                else
                    pCount++;

                
                Vector3 RelativePosition = ReadType<Vector3>(RootComponent + Offsets::SceneComponent::RelativeLocation);

                Vector3 HeadPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::Head));
         
                Vector3 RootPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::Root));
                
                Vector3 headPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::neck_01));
                headPos.Z += 7;
                Vector3 NeckPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::neck_01));
                Vector3 chestPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::spine_03));
                Vector3 pelvisPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::pelvis));
                Vector3 lSholderPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::eyebrow_l));
                Vector3 rSholderPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::nose_side_r_02));
                Vector3 lElbowPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::eyebrow_r));
                Vector3 rElbowPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::nose_side_l_01));
                Vector3 lWristPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::hair_r_02));
                Vector3 rWristPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::hair_r_01));
                Vector3 lThighPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::lip_um_01));
                Vector3 rThighPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::lip_r));
                Vector3 lKneePos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::lip_um_02));
                Vector3 rKneePos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::hair_root));
                Vector3 lAnklePos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::lip_ur));
                Vector3 rAnklePos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::hair_b_01));
                        
                MinimalViewInfo CameraCache = MinimalViewInfo();
                CameraCache = Global::GetPOV();      
                
                
                FMinimalViewInfo CameraCached = FMinimalViewInfo();
                CameraCached = Global::GetPOVV();     
                
                Vector3 myRootPos;
                if (PData::LocalPlayer) {
                    myRootPos = Global::GetBoneLocation(PData::LocalPlayer, BoneID::Root);
                } else {
                    myRootPos = Global::GetPOV().Location;
                }
                
                Vector3 Location = getLocation(RootComponent);
                Vector33 pelvisposition = Global::WorldToScreen2(Global::GetBoneWithRotation(Player, BoneID::pelvis));
                float distance = (Vector3::Distance(RootPos, myRootPos) / 100.0f);
                float distancee = (Vector3::Distance(Location, Global::GetPOV().Location) / 100.0f);
                Vector3 ViewPos = Global::WorldToScreen(Global::GetBoneLocation(Player, BoneID::Head));
                ViewPos.Z += 15.f;
                
                float ViewDistance = Vector3::Distance(RootPos, ViewPos);
                
                
                long Color = IM_COL32(255, 0, 0, 255);
                
           if (HeadPos.Z > 0 && RootPos.Z > 0) {
               
                
                     if (Config.ESPMenu.Line) {                        
                         draw->AddLine({(float) glWidth / 2, 0},  {HeadPos.X, HeadPos.Y} ,ToColor(Config.ColorsESP.Line), 1.0f);
                        }
                     
                     if (Config.ESPMenu.Box) {
                        float boxHeight = abs(HeadPos.Y - RootPos.Y);
                         float boxWidth = boxHeight * 0.65f;
                         ImVec2 vStart = {HeadPos.X - (boxWidth / 2), HeadPos.Y};
                         ImVec2 vEnd = {vStart.x + boxWidth, vStart.y + boxHeight};
                         draw->AddRect(vStart, vEnd, ToColor(Config.ColorsESP.Box), 1.4f);
                        }                      
                                
                      if (Config.ESPMenu.Skeleton) {
                           draw->AddCircle({HeadPos.X, HeadPos.Y},5*0.5f, ToColor(Config.ColorsESP.Skeleton), 0); 
                           draw->AddLine({chestPos.X, chestPos.Y}, {lSholderPos.X, lSholderPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({chestPos.X, chestPos.Y}, {rSholderPos.X, rSholderPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({lSholderPos.X, lSholderPos.Y}, {lElbowPos.X, lElbowPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({rSholderPos.X, rSholderPos.Y}, {rElbowPos.X, rElbowPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({lElbowPos.X, lElbowPos.Y}, {lWristPos.X, lWristPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({rElbowPos.X, rElbowPos.Y}, {rWristPos.X, rWristPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({chestPos.X, chestPos.Y}, {pelvisPos.X, pelvisPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({pelvisPos.X, pelvisPos.Y}, {lThighPos.X, lThighPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({pelvisPos.X, pelvisPos.Y}, {rThighPos.X, rThighPos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({lThighPos.X, lThighPos.Y}, {lKneePos.X, lKneePos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({rThighPos.X, rThighPos.Y}, {rKneePos.X, rKneePos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);

                           draw->AddLine({lKneePos.X, lKneePos.Y}, {lAnklePos.X, lAnklePos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);
                           draw->AddLine({rKneePos.X, rKneePos.Y},{rAnklePos.X, rAnklePos.Y}, ToColor(Config.ColorsESP.Skeleton),  1.0f);                           
                          }
                                                             
                          if (Config.ESPMenu.Health) {
                            int CurHP = (int) std::max(0, std::min((int) Health, (int) HealthMax));
                            int MaxHP = (int) HealthMax;

                            long HPColor = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min((510 * CurHP) / MaxHP, 255), 0, 155);

                            if (Config.ESPMenu.ShowKnockedHealth) {
                                if (Health == 0.0f && !bDead) {
                                    HPColor = IM_COL32(255, 0, 0, 155);

                                    CurHP = ReadType<float>(Player + Offsets::STExtraBaseCharacter::NearDeathBreath);
                                    auto NearDeatchComponent = ReadType(Player + Offsets::STExtraBaseCharacter::NearDeatchComponent);
                            if (NearDeatchComponent) {
                                    MaxHP = ReadType<float>(NearDeatchComponent + Offsets::STExtraBaseCharacter::BreathMax);
                                    }
                                }
                            }

                            float boxWidth = density / 1.6f;
                            boxWidth -= std::min(((boxWidth / 2) / 00.0f) * distance, boxWidth / 2);
                            float boxHeight = boxWidth * 0.15f;


                            ImVec2 vStart = {HeadPos.X - (boxWidth / 2), HeadPos.Y - (boxHeight * 2.00f)};

                            ImVec2 vEndFilled = {vStart.x + (CurHP * boxWidth / MaxHP), vStart.y + boxHeight};
                            ImVec2 vEndRect = {vStart.x + boxWidth, vStart.y + boxHeight};

                            draw->AddRectFilled(vStart, vEndFilled, HPColor, 6.0f);
                            draw->AddRect(vStart, vEndRect, IM_COL32(0, 0, 0, 155), 6.0f);
                        }
                        
                    if (Config.ESPMenu.Distance) {                         
                            float boxWidth = density / 1.6f;
                            boxWidth -= std::min(((boxWidth / 2) / 00.0f) * distancee, boxWidth / 2);
                            float boxHeight = boxWidth * 0.15f;
                            std::string s;
                            
                            if (!s.empty()) {
                                s += " - ";
                            }
                            s += std::to_string((int) distancee);
                            s += "m";

                            auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, ((float) density / 30.0f));
                            draw->AddText(NULL, ((float) density / 25.0f), {HeadPos.X - (textSize.x / 2), HeadPos.Y - (boxHeight * 1.00f)}, ToColor(Config.ColorsESP.Distance), s.c_str());
                        }                                           
                    
                    if (Config.ESPMenu.Name || Config.ESPMenu.TeamID) {
                            float boxWidth = density / 1.6f;
                            boxWidth -= std::min(((boxWidth / 2) / 00.0f) * distancee, boxWidth / 2);
                            float boxHeight = boxWidth * 0.15f;
                            std::string s;

                            if (Config.ESPMenu.TeamID) {
                                if (bIsAI) {
                                    s += "";
                                } else {
                                    s += "(";
                                    s += std::to_string(TeamID);
                                    s += ")";
                                }
                            }

                            if (Config.ESPMenu.Name) {
                                if (bIsAI) {
                                    s += " BOT";
                                } else {
                                    s += PlayerName.ToString();
                                }
                            }

                            auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, ((float) density / 30.0f));
                            draw->AddText(NULL, ((float) density / 30.0f), {HeadPos.X - (textSize.x / 2), HeadPos.Y - (boxHeight * 1.83f)}, ToColor(Config.ColorsESP.Name), s.c_str());
                         }
                      }             
                  }
              }
        
                
    if (pCount + bCount > 0) {
        std::string s;
            if (pCount + bCount < 10)
                s += "    ";
            else s += "   ";
        s += std::to_string((int) pCount + bCount);

        draw->AddRectFilled(ImVec2(glWidth / 2 - 60.f, 40), ImVec2(glWidth / 2 + 60.f, 105), IM_COL32(255, 0, 0, 80), 15.0f);
        draw->AddRect(ImVec2(glWidth / 2 - 60.f, 40), ImVec2(glWidth / 2 + 60.f, 105), IM_COL32(255, 0, 0, 255), 15.0f, 0,2.5f);
        draw->AddText(nullptr, ((float) density / 12.5f), ImVec2(glWidth / 2 - 50.f, 50), IM_COL32(255, 255, 255, 255), s.c_str());
    } else{
        std::string s;
        s += "SAFE";

            draw->AddRectFilled(ImVec2(glWidth / 2 - 60.f, 40), ImVec2(glWidth / 2 + 60.f, 105), IM_COL32(0, 255, 0, 80), 15.0f);
            draw->AddRect(ImVec2(glWidth / 2 - 60.f, 40), ImVec2(glWidth / 2 + 60.f, 105), IM_COL32(0, 255, 0, 255), 15.0f, 0, 2.5f);
            draw->AddText(nullptr, ((float) density / 12.5f), ImVec2(glWidth / 2 - 50.f, 50), IM_COL32(255, 255, 255, 255), s.c_str());
        }
        fps.update();
        
      
       
    if (Config.SilentAim.By == EAim::Croshhair) {
        draw->AddCircle(ImVec2(screenWidth / 3.7, screenHeight / 3.5), Config.SilentAim.Cross*0.5f, IM_COL32(255, 0, 0, 255), 100, 0.0f);
      }
 }
std::string getClipboardText() {
    if (!g_App)
        return "";

    auto activity = g_App->activity;
    if (!activity)
        return "";

    auto vm = activity->vm;
    if (!vm)
        return "";

    auto object = activity->clazz;
    if (!object)
        return "";

    std::string result;

    JNIEnv *env;
    vm->AttachCurrentThread(&env, 0);
    {
        auto ContextClass = env->FindClass("android/content/Context");
        auto getSystemServiceMethod = env->GetMethodID(ContextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

        auto str = env->NewStringUTF("clipboard");
        auto clipboardManager = env->CallObjectMethod(object, getSystemServiceMethod, str);
        env->DeleteLocalRef(str);

        auto ClipboardManagerClass = env->FindClass("android/content/ClipboardManager");
        auto getText = env->GetMethodID(ClipboardManagerClass, "getText", "()Ljava/lang/CharSequence;");

        auto CharSequenceClass = env->FindClass("java/lang/CharSequence");
        auto toStringMethod = env->GetMethodID(CharSequenceClass, "toString", "()Ljava/lang/String;");

        auto text = env->CallObjectMethod(clipboardManager, getText);
        if (text) {
            str = (jstring) env->CallObjectMethod(text, toStringMethod);
            result = env->GetStringUTFChars(str, 0);
            env->DeleteLocalRef(str);
            env->DeleteLocalRef(text);
        }

        env->DeleteLocalRef(CharSequenceClass);
        env->DeleteLocalRef(ClipboardManagerClass);
        env->DeleteLocalRef(clipboardManager);
        env->DeleteLocalRef(ContextClass);
    }
    vm->DetachCurrentThread();

    return result;
}

// ======================================================================== //
const char *GetAndroidID(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(/*android/content/Context*/ StrEnc("`L+&0^[S+-:J^$,r9q92(as", "\x01\x22\x4F\x54\x5F\x37\x3F\x7C\x48\x42\x54\x3E\x3B\x4A\x58\x5D\x7A\x1E\x57\x46\x4D\x19\x07", 23).c_str());
    jmethodID getContentResolverMethod = env->GetMethodID(contextClass, /*getContentResolver*/ StrEnc("E8X\\7r7ys_Q%JS+L+~", "\x22\x5D\x2C\x1F\x58\x1C\x43\x1C\x1D\x2B\x03\x40\x39\x3C\x47\x3A\x4E\x0C", 18).c_str(), /*()Landroid/content/ContentResolver;*/ StrEnc("8^QKmj< }5D:9q7f.BXkef]A*GYLNg}B!/L", "\x10\x77\x1D\x2A\x03\x0E\x4E\x4F\x14\x51\x6B\x59\x56\x1F\x43\x03\x40\x36\x77\x28\x0A\x08\x29\x24\x44\x33\x0B\x29\x3D\x08\x11\x34\x44\x5D\x77", 35).c_str());
    jclass settingSecureClass = env->FindClass(/*android/provider/Settings$Secure*/ StrEnc("T1yw^BCF^af&dB_@Raf}\\FS,zT~L(3Z\"", "\x35\x5F\x1D\x05\x31\x2B\x27\x69\x2E\x13\x09\x50\x0D\x26\x3A\x32\x7D\x32\x03\x09\x28\x2F\x3D\x4B\x09\x70\x2D\x29\x4B\x46\x28\x47", 32).c_str());
    jmethodID getStringMethod = env->GetStaticMethodID(settingSecureClass, /*getString*/ StrEnc("e<F*J5c0Y", "\x02\x59\x32\x79\x3E\x47\x0A\x5E\x3E", 9).c_str(), /*(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;*/ StrEnc("$6*%R*!XO\"m18o,0S!*`uI$IW)l_/_knSdlRiO1T`2sH|Ouy__^}%Y)JsQ:-\"(2_^-$i{?H", "\x0C\x7A\x4B\x4B\x36\x58\x4E\x31\x2B\x0D\x0E\x5E\x56\x1B\x49\x5E\x27\x0E\x69\x0F\x1B\x3D\x41\x27\x23\x7B\x09\x2C\x40\x33\x1D\x0B\x21\x5F\x20\x38\x08\x39\x50\x7B\x0C\x53\x1D\x2F\x53\x1C\x01\x0B\x36\x31\x39\x46\x0C\x15\x43\x2B\x05\x30\x15\x41\x43\x46\x55\x70\x0D\x59\x56\x00\x15\x58\x73", 71).c_str());

    auto obj = env->CallObjectMethod(context, getContentResolverMethod);
    auto str = (jstring) env->CallStaticObjectMethod(settingSecureClass, getStringMethod, obj, env->NewStringUTF(/*android_id*/ StrEnc("ujHO)8OfOE", "\x14\x04\x2C\x3D\x46\x51\x2B\x39\x26\x21", 10).c_str()));
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceModel(JNIEnv *env) {
    jclass buildClass = env->FindClass(/*android/os/Build*/ StrEnc("m5I{GKGWBP-VOxkA", "\x0C\x5B\x2D\x09\x28\x22\x23\x78\x2D\x23\x02\x14\x3A\x11\x07\x25", 16).c_str());
    jfieldID modelId = env->GetStaticFieldID(buildClass, /*MODEL*/ StrEnc("|}[q:", "\x31\x32\x1F\x34\x76", 5).c_str(), /*Ljava/lang/String;*/ StrEnc(".D:C:ETZ1O-Ib&^h.Y", "\x62\x2E\x5B\x35\x5B\x6A\x38\x3B\x5F\x28\x02\x1A\x16\x54\x37\x06\x49\x62", 18).c_str());

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceBrand(JNIEnv *env) {
    jclass buildClass = env->FindClass(/*android/os/Build*/ StrEnc("0iW=2^>0zTRB!B90", "\x51\x07\x33\x4F\x5D\x37\x5A\x1F\x15\x27\x7D\x00\x54\x2B\x55\x54", 16).c_str());
    jfieldID modelId = env->GetStaticFieldID(buildClass, /*BRAND*/ StrEnc("@{[FP", "\x02\x29\x1A\x08\x14", 5).c_str(), /*Ljava/lang/String;*/ StrEnc(".D:C:ETZ1O-Ib&^h.Y", "\x62\x2E\x5B\x35\x5B\x6A\x38\x3B\x5F\x28\x02\x1A\x16\x54\x37\x06\x49\x62", 18).c_str());

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetPackageName(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(/*android/content/Context*/ StrEnc("`L+&0^[S+-:J^$,r9q92(as", "\x01\x22\x4F\x54\x5F\x37\x3F\x7C\x48\x42\x54\x3E\x3B\x4A\x58\x5D\x7A\x1E\x57\x46\x4D\x19\x07", 23).c_str());
    jmethodID getPackageNameId = env->GetMethodID(contextClass, /*getPackageName*/ StrEnc("YN4DaP)!{wRGN}", "\x3E\x2B\x40\x14\x00\x33\x42\x40\x1C\x12\x1C\x26\x23\x18", 14).c_str(), /*()Ljava/lang/String;*/ StrEnc("VnpibEspM(b]<s#[9cQD", "\x7E\x47\x3C\x03\x03\x33\x12\x5F\x21\x49\x0C\x3A\x13\x20\x57\x29\x50\x0D\x36\x7F", 20).c_str());

    auto str = (jstring) env->CallObjectMethod(context, getPackageNameId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid) {
    jclass uuidClass = env->FindClass(/*java/util/UUID*/ StrEnc("B/TxJ=3BZ_]SFx", "\x28\x4E\x22\x19\x65\x48\x47\x2B\x36\x70\x08\x06\x0F\x3C", 14).c_str());

    auto len = strlen(uuid);

    jbyteArray myJByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(myJByteArray, 0, len, (jbyte *) uuid);

    jmethodID nameUUIDFromBytesMethod = env->GetStaticMethodID(uuidClass, /*nameUUIDFromBytes*/ StrEnc("P6LV|'0#A+zQmoat,", "\x3E\x57\x21\x33\x29\x72\x79\x67\x07\x59\x15\x3C\x2F\x16\x15\x11\x5F", 17).c_str(), /*([B)Ljava/util/UUID;*/ StrEnc("sW[\"Q[W3,7@H.vT0) xB", "\x5B\x0C\x19\x0B\x1D\x31\x36\x45\x4D\x18\x35\x3C\x47\x1A\x7B\x65\x7C\x69\x3C\x79", 20).c_str());
    jmethodID toStringMethod = env->GetMethodID(uuidClass, /*toString*/ StrEnc("2~5292eW", "\x46\x11\x66\x46\x4B\x5B\x0B\x30", 8).c_str(), /*()Ljava/lang/String;*/ StrEnc("P$BMc' #j?<:myTh_*h0", "\x78\x0D\x0E\x27\x02\x51\x41\x0C\x06\x5E\x52\x5D\x42\x2A\x20\x1A\x36\x44\x0F\x0B", 20).c_str());

    auto obj = env->CallStaticObjectMethod(uuidClass, nameUUIDFromBytesMethod, myJByteArray);
    auto str = (jstring) env->CallObjectMethod(obj, toStringMethod);
    return env->GetStringUTFChars(str, 0);
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;

    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

std::string Login(const char *user_key) {
    if (!g_App)
        return "Internal Error";

    auto activity = g_App->activity;
    if (!activity)
        return "Internal Error";

    auto vm = activity->vm;
    if (!vm)
        return "Internal Error";

    auto object = activity->clazz;
    if (!object)
        return "Internal Error";

    JNIEnv *env;
    vm->AttachCurrentThread(&env, 0);

    std::string hwid = user_key;
    hwid += GetAndroidID(env, object);
    hwid += GetDeviceModel(env);
    hwid += GetDeviceBrand(env);

    std::string UUID = GetDeviceUniqueIdentifier(env, hwid.c_str());

    vm->DetachCurrentThread();

    std::string errMsg;

    struct MemoryStruct chunk{};
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, /*POST*/ StrEnc(",IL=", "\x7C\x06\x1F\x69", 4).c_str());
        std::string sRedLink = "https://mrdevilbolte.xyz/connect";

        curl_easy_setopt(curl, CURLOPT_URL, sRedLink.c_str());

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, /*https*/ StrEnc("!mLBO", "\x49\x19\x38\x32\x3C", 5).c_str());
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, /*Content-Type: application/x-www-form-urlencoded*/ StrEnc("@;Ls\\(KP4Qrop`b#d3094/r1cf<c<=H)AiiBG6i|Ta66s2[", "\x03\x54\x22\x07\x39\x46\x3F\x7D\x60\x28\x02\x0A\x4A\x40\x03\x53\x14\x5F\x59\x5A\x55\x5B\x1B\x5E\x0D\x49\x44\x4E\x4B\x4A\x3F\x04\x27\x06\x1B\x2F\x6A\x43\x1B\x10\x31\x0F\x55\x59\x17\x57\x3F", 47).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        char data[4096];
        sprintf(data, /*game=PUBG&user_key=%s&serial=%s*/ StrEnc("qu2yXK,YkJyGD@ut0.u~Nb'5(:.:chK", "\x16\x14\x5F\x1C\x65\x1B\x79\x1B\x2C\x6C\x0C\x34\x21\x32\x2A\x1F\x55\x57\x48\x5B\x3D\x44\x54\x50\x5A\x53\x4F\x56\x5E\x4D\x38", 31).c_str(), user_key, UUID.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            try {
                json result = json::parse(chunk.memory);
                if (result[/*status*/ StrEnc("(>_LBm", "\x5B\x4A\x3E\x38\x37\x1E", 6).c_str()] == true) {
                    std::string token = result[/*data*/ StrEnc("fAVA", "\x02\x20\x22\x20", 4).c_str()][/*token*/ StrEnc("{>3Lr", "\x0F\x51\x58\x29\x1C", 5).c_str()].get<std::string>();
                    time_t rng = result[/*data*/ StrEnc("fAVA", "\x02\x20\x22\x20", 4).c_str()][/*rng*/ StrEnc("+n,", "\x59\x00\x4B", 3).c_str()].get<time_t>();
                     if (rng + 30 > time(0)) {
                        std::string auth = /*PUBG*/ StrEnc("Q*) ", "\x01\x7F\x6B\x67", 4).c_str();;
                        auth += "-";
                        auth += user_key;
                        auth += "-";
                        auth += UUID;
                        auth += "-";
                        auth += /*Vm8Lk7Uj2JmsjCPVPVjrLa7zgfx3uz9E*/ StrEnc("-2:uwZdV^%]?{{wHs2V,+(^NJU;kC*_{", "\x7B\x5F\x02\x39\x1C\x6D\x31\x3C\x6C\x6F\x30\x4C\x11\x38\x27\x1E\x23\x64\x3C\x5E\x67\x49\x69\x34\x2D\x33\x43\x58\x36\x50\x66\x3E", 32).c_str();
                        std::string outputAuth = Tools::CalcMD5(auth);
     
                        g_Token = token;
                        g_Auth = outputAuth;

                        bValid = g_Token == g_Auth;
                    }
                } else {
                    errMsg = result[/*reason*/ StrEnc("LW(3(c", "\x3E\x32\x49\x40\x47\x0D", 6).c_str()].get<std::string>();
                }
            } catch (json::exception &e) {
                errMsg = "{";
                errMsg += e.what();
                errMsg += "}\n{";
                errMsg += chunk.memory;
                errMsg += "}";
            }
        } else {
            errMsg = curl_easy_strerror(res);
        }
    }
    curl_easy_cleanup(curl);

    return bValid ? "OK" : errMsg;
}
// ======================================================================== //
EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

EGLBoolean _eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
    if (glWidth <= 0 || glHeight <= 0)
        return orig_eglSwapBuffers(dpy, surface);

    if (!g_App)
        return orig_eglSwapBuffers(dpy, surface);

    screenWidth = ANativeWindow_getWidth(g_App->window);
    screenHeight = ANativeWindow_getHeight(g_App->window);
    density = AConfiguration_getDensity(g_App->config);

    if (!initImGui) {
        ImGui::CreateContext();

        ImGuiStyle *style = &ImGui::GetStyle();
        style->FrameRounding = 4.0f;
       // style->WindowRounding = 10.0f;
        style->GrabRounding = 4.0f;

        ImVec4 *colors = style->Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.10f, 0.15f, 0.19f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.19f, 0.15f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.19f, 0.15f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        style->ScaleAllSizes(std::max(1.0f, density / 150.0f));
        style->ScrollbarSize *= 0.7f;
        style->Colors[ImGuiCol_ChildBg] = ImColor(29, 34, 38);

        ImGui_ImplAndroid_Init();
        ImGui_ImplOpenGL3_Init("#version 300 es");

        ImGuiIO &io = ImGui::GetIO();
        ImGui::StyleColorsClassic();

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = NULL;

        ImFontConfig cfg;
        cfg.SizePixels = ((float) density / 20.0f);
        io.Fonts->AddFontDefault(&cfg);

        memset(&Config, 0, sizeof(sConfig));

        Config.ColorsESP.Line = CREATE_COLOR(255, 0, 0, 255);
        Config.ColorsESP.Box = CREATE_COLOR(255, 0, 255, 255);
        Config.ColorsESP.Name = CREATE_COLOR(255, 0, 0, 255);
        Config.ColorsESP.Distance = CREATE_COLOR(255, 0, 255, 255);
        Config.ColorsESP.Skeleton = CREATE_COLOR(255, 0, 0, 255);
        Config.SilentAim.Cross  = 36.0f;

        initImGui = true;
    }

    ImGuiIO &io = ImGui::GetIO();


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
    ImGui::NewFrame();

    DrawESP(ImGui::GetBackgroundDrawList());

    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.55f, (float) glHeight * 0.80f),ImGuiCond_Once); // 45% width 70% height
    if (ImGui::Begin("EMPIRE CHEATS SAFE", 0, ImGuiWindowFlags_NoSavedSettings)) {
        static bool isLogin = false;

        /*if (!isLogin) {
            ImGui::Text("Please Login! (Copy Key to Clipboard)");

            ImGui::PushItemWidth(-1);
            static char s[64];
            ImGui::InputText("##key", s, sizeof s);
            ImGui::PopItemWidth();

            if (ImGui::Button("^^ Paste Your Key ^^", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                auto key = getClipboardText();
                strncpy(s, key.c_str(), sizeof s);
            }

            static std::string err;
            if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                err = Login(s);
                if (err == "OK") {
                    isLogin = bValid && g_Auth == g_Token;
                }
            }

            if (!err.empty() && err != "OK") {
                ImGui::Text("Error: %s", err.c_str());
            }

        } else {*/
        if (!isLogin) {
            if (ImGui::Button("Putrimod Esp", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                    isLogin = true;
                }
        } else {
            ImGui::TextColored(ImVec4(0, 0.88f, 0.73f, 1.00f), "THIS MOD IS BY BEAST MODDER JOIN-: @EMPIRE_CHEAT");
        
            IMGUI_DEMO_MARKER("Esp");
            if (ImGui::CollapsingHeader("Esp")) {
                ImGuiIO &io = ImGui::GetIO();

                if (ImGui::TreeNode("WORLD VISUAL")) {

                    if (ImGui::BeginTable("split", 3)) {
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Line", &Config.ESPMenu.Line);
                    /*    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                        ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                        ImGui::ColorEdit3("##LineCol", Config.ColorsESP.Line, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
*/
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Box", &Config.ESPMenu.Box);
                   /*     ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                        ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                        ImGui::ColorEdit3("##BoxCol", Config.ColorsESP.Box, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    */
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Health", &Config.ESPMenu.Health);
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Knocked Health", &Config.ESPMenu.ShowKnockedHealth);
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Name", &Config.ESPMenu.Name);
                 /*       ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                        ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                        ImGui::ColorEdit3("##NameCol", Config.ColorsESP.Name, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    */  ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Skeleton", &Config.ESPMenu.Skeleton);
                 /*       ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                        ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                        ImGui::ColorEdit3("##SkeletonCol", Config.ColorsESP.Skeleton, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    */
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Distance", &Config.ESPMenu.Distance);
                  /*      ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                        ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                        ImGui::ColorEdit3("##DistanceCol", Config.ColorsESP.Distance, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    */
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("ESP Team ID", &Config.ESPMenu.TeamID);
                        ImGui::TableNextColumn();
                        ImGui::EndTable();
                        ImGui::EndTabItem();
                     }
                
}

                    IMGUI_DEMO_MARKER("Esp/Items");
                    if (ImGui::TreeNode("Items")) {

                        ImGui::Spacing();

                        for (auto &i : items_data) {
                            if (ImGui::TreeNode(i["Category"].get<std::string>().c_str())) {
                                for (auto &item : i["Items"]) {
                                    ImGui::Checkbox(item["itemName"].get<std::string>().c_str(),
                                                    (bool *) &Items[item["itemId"].get<int>()]);
                                    ImGui::TreePop();
                                    ImGui::TreePop();
                               }
									}
									}
									}
                            }
          IMGUI_DEMO_MARKER("SILENT AIM");
            if (ImGui::CollapsingHeader("SILENT AIM")) {
                ImGuiIO &io = ImGui::GetIO();
          
                ImGui::Checkbox("Aimbot", &Config.SilentAim.Aimbot);
                  
                    ImGui::Checkbox("Smooth", &Config.SilentAim.Smooth);
                    ImGui::Checkbox("Bulletrack 360", &Config.SilentAim.Bulletrack);
                     
                    ImGui::Text("Target By: ");
                    ImGui::SameLine();
                    static const char *gets[] = {"Distance","Croshhair"};
                    ImGui::Combo("##Target By", (int *) &Config.SilentAim.By, gets, 2, -1);  
                    ImGui::DragFloat("Size", &Config.SilentAim.Cross, 0.2f, 2.0f, 500.0f, "%.0f");
                    
                    ImGui::Text("Target: ");
                    ImGui::SameLine();
                    static const char *targets[] = {"Heads", "Chest"};
                    ImGui::Combo("##Target", (int *) &Config.SilentAim.Target, targets, 2, -1);

                    ImGui::Text("Trigger: ");
                    ImGui::SameLine();
                    static const char *triggers[] = {"None", "Shooting", "Scoping",
                                                     "Both (Shooting & Scoping)",
                                                     "Any (Shooting / Scoping)"};
                    ImGui::Combo("##Trigger", (int *) &Config.SilentAim.Trigger, triggers, 5, -1);

                    ImGui::Checkbox("Ignore Knocked", &Config.SilentAim.IgnoreKnocked);
                    ImGui::Checkbox("Prediction", &Config.SilentAim.Predict);
                    ImGui::EndTabItem();
                 }
                 
                IMGUI_DEMO_MARKER("Memory");
            if (ImGui::CollapsingHeader("Memory")) {
                ImGuiIO &io = ImGui::GetIO();
          
                    ImGui::Spacing();
                    ImGui::Text("risky ");
                   // ImGui::Checkbox("AIMBOT", &Config.MEMORY.AIMBOT);
                   // ImGui::Checkbox("small crosshiar", &small);
                   // ImGui::Checkbox("NO fog", &nofog);

/*
                   ImGui::Checkbox("Less Recoil", &Config.MEMORY.RECOIL);
                   ImGui::Checkbox("AIMBOT", &Config.MEMORY.AIMBOT);
                   ImGui::Checkbox("MAGIC BULLET", &Config.MEMORY.MAGICBULLET);
                   ImGui::Checkbox("SMALL CROSSAIR", &Config.MEMORY.SMALLCROSSAIR);
*/
                }
            
             
            IMGUI_DEMO_MARKER("COUSTUMIZE");
            if (ImGui::CollapsingHeader("COUSTUMIZE")) {

                ImGui::Spacing();

                ImGui::Checkbox("Fps", &Config.OTHER.FPS);
              //  ImGui::RadioButton("Lobby Bypass", &loby);
                ImGui::Checkbox("Expiry time", &Config.OTHER.EXPIRYTIME);
                ImGui::Checkbox("Coustomize Menu", &show_another_window);
         //       ImGui::RadioButton("Autometic logo bypass ", &bypass);
                ImGui::Checkbox("COUSTOMIZE ESP MENU", &show_esp_window);
 }
  }
    }
 if (show_esp_window) {
        ImGui::Begin("COUSTOMIZABLE  ESP COLORS ",
                     &show_esp_window);
                    ImGui::Spacing();
                    ImGui::Text("ESP Line ");
                   
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                    ImGui::ColorEdit3("##LineCol", Config.ColorsESP.Line, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);

                    ImGui::Text("ESP Box");
                    
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                    ImGui::ColorEdit3("##BoxCol", Config.ColorsESP.Box, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    
                    ImGui::Text("ESP Name");
           
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                    ImGui::ColorEdit3("##NameCol", Config.ColorsESP.Name, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    
                    ImGui::Text("ESP Skeleton");
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                    ImGui::ColorEdit3("##SkeletonCol", Config.ColorsESP.Skeleton, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                    
                    ImGui::Text("ESP Distance");
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
                    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.25f, (float) glHeight * 0.25f), ImGuiCond_Once);
                    ImGui::ColorEdit3("##DistanceCol", Config.ColorsESP.Distance, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);           
                 
                    
                       }
                             
    if (show_another_window) {
        ImGui::Begin("COUSTOMIZABLE FUNCTIONS / ESP ITEMS",
                     &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        // ImGui::Text("Src is made by :- @vishesh_404");
        ImGui::Text("CHOOSE MOD MENU COLOR FROM BELOW");
        static int e = 0;
        ImGui::RadioButton("PURPLE", &e, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Drak", &e, 1);
        ImGui::SameLine();
        ImGui::RadioButton("WHITE", &e, 2);
        switch (e) {
            case 0:
                ImGui::StyleColorsClassic();
                break;
            case 1:
                ImGui::StyleColorsDark();
                break;
            case 2:
                ImGui::StyleColorsLight();
                break;
        }


        ImGui::Checkbox("Font Color{Pink}", &pink);
        ImGui::SameLine();
        ImGui::Checkbox("Border", &border);
        ImGui::SameLine();
        ImGui::Checkbox("Rectangle Base", &rectangle);
    }

    if (rectangle) {
        ImGui::CreateContext();

        ImGuiStyle *style = &ImGui::GetStyle();
        style->WindowRounding = 10.0f;
    }
    else{
        ImGui::CreateContext();

        ImGuiStyle *style = &ImGui::GetStyle();
        style->WindowRounding = 1.0f;
    }
    if (border)
    {
        ImGui::CreateContext();

        ImGuiStyle* style = &ImGui::GetStyle();
        style->FrameBorderSize = 2.0f;


    }
    else {
        ImGui::CreateContext();

        ImGuiStyle* style = &ImGui::GetStyle();
        style->FrameBorderSize = 0.0f;
    }

        if (pink)
        {
            ImGui::CreateContext();

            ImGuiStyle *style = &ImGui::GetStyle();

            ImVec4 *colors = style->Colors;

            colors[ImGuiCol_Text] = ImVec4( 1.0f, 0.0f, 1.0f, 1.0f);
       }
    ImGui::End();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surface);
}

int32_t (*orig_onInputEvent)(struct android_app *app, AInputEvent *inputEvent);

int32_t onInputEvent(struct android_app *app, AInputEvent *inputEvent) {
    if (initImGui) {
        ImGui_ImplAndroid_HandleInputEvent(inputEvent, {(float) screenWidth / (float) glWidth, (float) screenHeight / (float) glHeight});
    }
    return orig_onInputEvent(app, inputEvent);
}

#define SLEEP_TIME 1000LL / 60LL

[[noreturn]] void *maps_thread(void *) {
    while (true) {
        auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        std::vector<sRegion> tmp;
        char line[512];
        FILE *f = fopen("/proc/self/maps", "r");
        if (f) {
            while (fgets(line, sizeof line, f)) {
                uintptr_t start, end;
                char tmpProt[16];
                if (sscanf(line, "%" PRIXPTR "-%" PRIXPTR " %16s %*s %*s %*s %*s", &start, &end, tmpProt) > 0) {
                    if (tmpProt[0] != 'r') {
                        tmp.push_back({start, end});
                    }
                }
            }
            fclose(f);
        }
        trapRegions = tmp;

        auto td = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(std::min(0LL, SLEEP_TIME - td), SLEEP_TIME)));
    }
}
    void (*orig_nativeVirtualKeyboardChanged)(JNIEnv *env, jobject clazz, jstring str);
void nativeVirtualKeyboardChanged(JNIEnv *env, jobject clazz, jstring str) {
    auto s = env->GetStringUTFChars(str, 0);
    LOGI("s: %s", s);
    return orig_nativeVirtualKeyboardChanged(env, clazz, str);
}

void *main_thread(void *) {
     g_UE4 = Tools::GetBaseAddress("libUE4.so");
    while (!g_UE4) {
        g_UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }
    

    while (!g_App) {
        g_App = *(android_app **) (g_UE4 + 0x740BB98);
        sleep(1);
    }

    while (!g_App->onInputEvent)
        sleep(1);

    orig_onInputEvent = decltype(orig_onInputEvent)(g_App->onInputEvent);
    g_App->onInputEvent = onInputEvent;

    void *ue4 = dlopen_ex("libUE4.so", 4);
    HOOK(dlsym_ex(ue4, "Java_com_epicgames_ue4_GameActivity_nativeVirtualKeyboardChanged"), nativeVirtualKeyboardChanged, &orig_nativeVirtualKeyboardChanged);
    dlclose_ex(ue4);

    plthook_t *plthook;
    if (plthook_open(&plthook, "libUE4.so") == 0) {
        if (plthook_replace(plthook, "eglSwapBuffers", (void*)_eglSwapBuffers, (void **)&orig_eglSwapBuffers) == 0) {
            LOGI("eglSwapBuffers replaced using plthook!");
        } else {
            LOGI("plthook: %s", plthook_error());
        }
        plthook_close(plthook);
    } else {
        LOGI("plthook: %s", plthook_error());
    }
    

    pthread_t t;
    pthread_create(&t, 0, GameUpdateData, 0);
    pthread_create(&t, 0, GameAimbotData, 0);
    items_data = json::parse(JSON_ITEMS);
    return 0;
}

__attribute__((constructor)) void _init() {
    pthread_t t;
    pthread_create(&t, 0, main_thread, 0);

}

extern "C" 
JNIEXPORT jstring JNICALL //SIGN GLOBAL
Java_com_pubgm_ApkUtils_GetDataGlobal(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("AQAAA4EwggN9MIICZaADAgECAgQdzoakMA0GCSqGSIb3DQEBCwUAMG4xCzAJBgNVBAYTAjg2MRMwEQYDVQQIEwpHdWFuZyBEb25nMRIwEAYDVQQHEwlTaGVuIFpoZW4xEDAOBgNVBAoTB1RlbmNlbnQxEDAOBgNVBAsTB1RlbmNlbnQxEjAQBgNVBAMTCUhlY3RvciBMdTAgFw0xNzEyMjgxMTM3MDdaGA8yMTE3MTIwNDExMzcwN1owbjELMAkGA1UEBhMCODYxEzARBgNVBAgTCkd1YW5nIERvbmcxEjAQBgNVBAcTCVNoZW4gWmhlbjEQMA4GA1UEChMHVGVuY2VudDEQMA4GA1UECxMHVGVuY2VudDESMBAGA1UEAxMJSGVjdG9yIEx1MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoLUkEokic5WdkaoxZsw1kiYwqea6s5TYNBq/6whx5hUl850hZKdIBj8oPtxzNSV2MytKJJo6gxH6o1M1WFuiMXzaCTtEKE8T2wASnxM1BJ1yW7KIZMPPZVGotAcsdhYLD9RxrR0t5kDeeRX2lBVcOhso/gGVP3uuvB2260mqInGr0qfD07pJ2pTMApiHRDMcUQEtyOON2esTPHXlME7mfUBIsAD9WwPLZKNQBcl2QkfWhAj51xZ6bpGxBSHXzOYhcfuHYGkt7SX65mE1T5Oah1ZbN3AVyo1dahE5b7FdfEz91r0B6w0LeP/yW3SwvnDH4CHbMBelsPusR9V07uhONQIDAQABoyEwHzAdBgNVHQ4EFgQUBOHr6O/GDj0JgFVqzRwoaoY3DMIwDQYJKoZIhvcNAQELBQADggEBAAgV8yC0OhdYD4+YnTdtwFlybV3mDKO6NAIM+LatvdrW/qV3s1IOjvUBp8Em6JIrvHITy6NzGPrKPMYX2gMBD8/vjjGzRtpSSzJobiOWWM+3+G0wlTFBzhKLSR5itmGkP4JLvq6//Sb0lDFnK4bDhnfOMjVcqyAI6Es+PhOPyRUDNLEu8XjjpcKIzSvybj5LHRvcEU2Kj9WH8xzzE3QoHpx35jzhcnptErLz36XZqKWuhnNqEuMgyIN214yXwlKErPRK5bOZRr6c93zxwQb6cSbKVrRZbo5HzGWG8xEPSaeYMY8rviisXPtlm6AT7iJmYGhor19kBIR0HI9V3nz06Fs="));
return str;
}
extern "C" 
JNIEXPORT jstring JNICALL //SIGN KOREA
Java_com_pubgm_ApkUtils_GetDataKorea(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("AQAAA2UwggNhMIICSaADAgECAgRGrEN5MA0GCSqGSIb3DQEBCwUAMGExCzAJBgNVBAYTAmtyMQ4wDAYDVQQIEwVTZW91bDEOMAwGA1UEBxMFU2VvdWwxDTALBgNVBAoTBFBVQkcxDzANBgNVBAsTBktSIEJpejESMBAGA1UEAxMJSnNvbiBDaG9pMB4XDTE4MDMzMDA1MDk1M1oXDTQzMDMyNDA1MDk1M1owYTELMAkGA1UEBhMCa3IxDjAMBgNVBAgTBVNlb3VsMQ4wDAYDVQQHEwVTZW91bDENMAsGA1UEChMEUFVCRzEPMA0GA1UECxMGS1IgQml6MRIwEAYDVQQDEwlKc29uIENob2kwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCf987Etd/fj7AgjeZ002BghE9IEfaYKvUi+7SN9jByXh4ujtsb+GSakoTl3+eDNbZ6TkLVhHLnab2gtcYJMsX3l7xdtzQt6rjUeZlhquGHlJd1NIuOHoB3nQS51DQA4GqsKxT/5Gm83UdhPvQ6Lz1IzTK5k1GRoHt16G3emWe5AgPyJJZG8Rkq5d0SHyGwQbeuH1KyAwes7L+E/qCNoDn8ed50ixzVx0YuCnbLSqDrGbGIfUVPAe6b1Pb7VBJ/r0Fin8+1mwLs1y+W0uA8qVEqDm3ZWz5l6muJ7xeSfiWRt9+imBK8S+vni22VBcp6FzDSpJPxK4JfyqvkPlKdMZpxAgMBAAGjITAfMB0GA1UdDgQWBBRNoRhsqQ1dHYCBa6l7Wim2zOOPxTANBgkqhkiG9w0BAQsFAAOCAQEAUnF8V9x8hnlqwgTVzXMt1mnbPlbxsSyEbzDACuFsyAQ/XGqd7whVPpiFua0nmOuyemGqzO0uMUJsMzaKtEdANSrGb3RiiN6GstGY0+OnexXZplQEdy2DxEs7k4IRNOWgfUjgTQkzA9aGRReDm3GMMCCto0or40HLNGzNafNRNPfvbmFDN34P3/fapAWgzUeszEDbkj9KjkPWlSMJfte8xGw43n6+AiHX+gFYskrJLWe8bEl3YJRfzJ2309a1OcWiHMt6W2T7hr8xyoo+4sgTwAoiVc+bQiRFLRBuzMg5JG7jgkjWzckQ2o0+RQ3dspjz6sWgfN0tCW34gIHwl3q4Sw=="));
return str;
}
extern "C" 
JNIEXPORT jstring JNICALL //SIGN BGMI
Java_com_pubgm_ApkUtils_GetDataIndia(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("AQAAA2UwggNhMIICSaADAgECAgRvSDmYMA0GCSqGSIb3DQEBCwUAMGExCzAJBgNVBAYTAmtyMQ4wDAYDVQQIEwVTZW91bDEOMAwGA1UEBxMFU2VvdWwxDTALBgNVBAoTBFBVQkcxDzANBgNVBAsTBktSIEJpejESMBAGA1UEAxMJSnNvbiBDaG9pMB4XDTIwMDgwNDA0MTMwMloXDTQ1MDcyOTA0MTMwMlowYTELMAkGA1UEBhMCa3IxDjAMBgNVBAgTBVNlb3VsMQ4wDAYDVQQHEwVTZW91bDENMAsGA1UEChMEUFVCRzEPMA0GA1UECxMGS1IgQml6MRIwEAYDVQQDEwlKc29uIENob2kwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDfasN86hVc/OrFUnr3I6nWBOf0R5QR8QM3jYyr9v+WOpPz6T9PsHbTnMaXTcY3WDCoQGiyoIuqKMAQlUPaE6NDH7GoOTOF6u4SNf2hR8T0+Lbcl5z625AFdf6KcL9Mr3o3Ffq3N+mkwOsnlJpe0WdebVOTL1+CDjpobhrMiZBlsW5ukul+P1923X3RpslQe3hsD/E/wcEITzSTrE99KqdeKT50AQqsCpU6M3BqgethV70n+hlkryJSqTAPlXlCZD3ofNFn1STFxRtGS0lvA6z4THaQa99mPFaleBxX7tMi8iKwtK8ynMvUbXu1dh6Lukfo6x6LH8IdgoQBpIJVv+MNAgMBAAGjITAfMB0GA1UdDgQWBBTlRHrvIcYfNYpQ1SlV3I563mV7HjANBgkqhkiG9w0BAQsFAAOCAQEAgTuIerIMga9hyjWX1qmPhUzl9BYUY8pqtq/X40d7eYVBk3qFG4FpsKg3dLBWull7L2q5hpCfec003nhKnwpgLTZORxO4KlyvaCE186zvfLVykO6oWFXJCzJVGNnMSeH1cNprZMcmOZp0Q0PB07TpQf5wEpb/uiP1trNAiok3li2/3GQCIiEwfU7HFjr2sn4HFmzPJHW2oVx4M61LRm4zp3FE697JWZ+iH2rGz1XDyiHWq2zAp2iumkBfJN/qG7dI6fE95aGBkqLQQnFbIwon5C6jPFWkqJ2G2EEsZNT+noygyJkDJzFnrtMW7oYANcrxVdwIMDFtrIIt2jt+VuZVLA=="));
return str;
}

extern "C" 
JNIEXPORT jstring JNICALL //SIGN Vietnam
Java_com_pubgm_ApkUtils_GetDataVietnam(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("AQAAA4EwggN9MIICZaADAgECAgRvmtqRMA0GCSqGSIb3DQEBCwUAMG8xCzAJBgNVBAYTAlZOMRQwEgYDVQQIEwtIbyBDaGkgTWluaDEUMBIGA1UEBxMLSG8gQ2hpIE1pbmgxEjAQBgNVBAoTCVZORyBHYW1lczESMBAGA1UECxMJVk5HIEdhbWVzMQwwCgYDVQQDEwNWTkcwHhcNMTQxMjI0MTM0ODE0WhcNNDIwNTExMTM0ODE0WjBvMQswCQYDVQQGEwJWTjEUMBIGA1UECBMLSG8gQ2hpIE1pbmgxFDASBgNVBAcTC0hvIENoaSBNaW5oMRIwEAYDVQQKEwlWTkcgR2FtZXMxEjAQBgNVBAsTCVZORyBHYW1lczEMMAoGA1UEAxMDVk5HMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAimd2CrBZ2Dl26SIDsVfp68sSfZMmwsbuD1UAIPVg1XGgxTDCuHzZcp8T6vdDnaWpTjz3l5vs4HMk5ZUt0ejVNy88WLV8ziG5RsnUUS2Lw1PR3cLPI7qvK+x8rINoqRphYQ1AXAJ+UwRoR9VrEkD8SyZjst6ahPGt1ZL5GrfzFiKmcHGhXTHLSac1nFwy1JMFr6TfTyQ1Quw7fH2oDL82t3eV1MyRmcxg1reGhjPPZVu4xuCJdQ1dKHrWPkhc6GkipaVc12nwPRX87fN2Lf0Oy/6c48g4S/jP31rt5afmQfzNcCoWM/XzvZ0DNyr3an+EVqbq9lqE2kjeefULUfaVRwIDAQABoyEwHzAdBgNVHQ4EFgQUV/0zdu870XbeUO4p8oP1fQkzqEQwDQYJKoZIhvcNAQELBQADggEBAHyFPvO5KLEMyd8gPeKVRql36kdMcRwWBt5imulULQnzXyMVqyAK6eoZSxk5szit7zBnTfiyVt80oOBs2PHx6QKHGc6wSInGFg+llbrMXrgJ7o4j8XJh2B5yFsZzP8ue5nQPsSOQvmatD6y8/KnBlAlV7VHMVFAfurxXK8OsD2yqr0U2wu8k2axu+qpTVaio5+tXD4y0yIMfVBg+6J5J4//b3742tGrrqwt9jLttKBmO6gh1SSUm5pNXQSDh49hDN555M4LMUlZkEN9h+Siyg53TFBQhbS61o+reZVyQt84LAS8zhK6UvteODVWwiHz+ac4hTa3A8v8lUu84rbRWQtU="));
return str;
}
extern "C" 
JNIEXPORT jstring JNICALL //SIGN Taiwan
Java_com_pubgm_ApkUtils_GetDataTaiwan(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("AQAAA4EwggN9MIICZaADAgECAgR3sEeBMA0GCSqGSIb3DQEBCwUAMG4xCzAJBgNVBAYTAjg2MRMwEQYDVQQIEwpHdWFuZyBEb25nMRIwEAYDVQQHEwlTaGVuIFpoZW4xEDAOBgNVBAoTB1RlbmNlbnQxEDAOBgNVBAsTB1RlbmNlbnQxEjAQBgNVBAMTCUhlY3RvciBMdTAgFw0xOTAyMTUxMzE5MDhaGA8yMjkyMTEyOTEzMTkwOFowbjELMAkGA1UEBhMCODYxEzARBgNVBAgTCkd1YW5nIERvbmcxEjAQBgNVBAcTCVNoZW4gWmhlbjEQMA4GA1UEChMHVGVuY2VudDEQMA4GA1UECxMHVGVuY2VudDESMBAGA1UEAxMJSGVjdG9yIEx1MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArGPsxZm30XySTRem8shDXM1MSk+3PVQ+BybaXEIQAZ9IuiHnCrniff3f/LAl4TOIM+3wUMgFQX4Pmkx00FunBc+AiwIZi7lcMPy63Z654glgGokKIQm4onvFhgPxDUNqpNeEumObiMycTIrQ1G671li91+Bb0Lyc5pP7tach3OgQbmDxP6kAOYJf2Eb/ovkaQSxT+Q5rc1DBR/jw4Oa4jgOLDdCdhn6lQ3mc9NiqDYWvA4MGz6QTdgFHUmoCTy2dyRcc4KvgPgHnFdzN2bLunPe3QNCzbpozDY31jm0Mfot+cF+VHcojVa1SJ7oBWUZj0O8srORLUVwdsRqbEvRysQIDAQABoyEwHzAdBgNVHQ4EFgQU9E5bbj3qMdz0974HLPtvKd06N2EwDQYJKoZIhvcNAQELBQADggEBAKdlbcHxDw7z+NxgepiC+17AKc3hI6jHmky25cbh3xLBqGrKOYO7bKh2YwExIZHyeeW2ZKR/9kNes42YpVYdwBnCG/0BnvzX5tXdlKlzV/uHEkj82Lo2eiIBtEsszD0kEnJfS1+7THoJaFvNFfTLmzdHsM3bZhL70T59YtPQUoLa4dlBMAQg0gc4x9tZdQWnFgyb0e7g3Ta4cOkXPIUDErQS0wMBY0n5hXsnw72vDVhW2NZor5wv6fSNLdImMjSFlo8RmdBPqc+sqvXLk8HCjGkut6vcZXR609LpfYtQH5T1bkMqdUu1PjQRLHWG8jsTtt/Ofd7ype7NHu1lp9H/voc="));
return str;
}
//TEXT_APK
extern "C" 
JNIEXPORT jstring JNICALL
Java_com_pubgm_ApkUtils_GetAPK(JNIEnv *env, jobject activityObject ) {
jstring str = env->NewStringUTF(("Kontol.apk"));
return str;
}
