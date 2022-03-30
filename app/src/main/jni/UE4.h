

#include "Global.h"

#define IM_PI                   3.14159265358979323846f
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / IM_PI) )
#define DEG2RAD( x ) ( (float)(x) * (float)(IM_PI / 180.f) )


class Vector33
{
public:
    Vector33() : x(0.f), y(0.f), z(0.f)
    {

    }

    Vector33(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
    {

    }
    ~Vector33()
    {

    }

    float x;
    float y;
    float z;

    inline float Dot(Vector33 v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline float Distanceh(Vector33 v)
    {
        return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
    }


    inline float Length()
    {
        float ls = Dot(Vector33{ x, y, z });
        return (float)sqrt(ls);
    }

    Vector33 operator+(Vector33 v)
    {
        return Vector33(x + v.x, y + v.y, z + v.z);
    }

    Vector33 operator-(Vector33 v)
    {
        return Vector33(x - v.x, y - v.y, z - v.z);
    }

    Vector33 operator/(float f) {
        return Vector33(x / f, y / f, z / f);
    }

    bool operator!=(Vector33 v)
    {
        if (x != v.x)
            return true;
        if (y != v.y)
            return true;
        if (z != v.z)
            return true;
        return false;
    }
    bool operator==(Vector33 v)
    {
        if (x != v.x)
            return false;
        if (y != v.y)
            return false;
        if (z != v.z)
            return false;
        return true;
    }
    bool operator>(Vector33 v)
    {
        if (x > v.x)
            return false;
        if (y > v.y)
            return false;
        if (z > v.z)
            return false;
        return true;
    }
    bool operator<(Vector33 v)
    {
        if (x < v.x)
            return false;
        if (y < v.y)
            return false;
        if (z < v.z)
            return false;
        return true;
    }
};
template<typename Fn>
Fn GetVFunction(void *thiz, int idx)
{
    auto VTable = *reinterpret_cast<void***>(const_cast<void*>(thiz));
    return (Fn)(VTable[idx]);
}

struct LinearColor {
    float R;
    float G;
    float B;
    float A;
};


    
struct FName
{
    int32_t ComparisonIndex;
    int32_t Number;
    char _pad[0x8];
};

template<typename T>
struct TArray {
    struct FString;
    
    T* Data;
    int32_t Count;
    int32_t Max;
    
    T &operator[](int idx) {
        return Data[idx];
    }

    T &operator[](int idx) const {
        return Data[idx];
    }
     
  
};



struct FString : private TArray<unsigned short>
{
    FString()
    {
    }

    FString(const std::wstring s)
    {
        Max = Count = !s.empty() ? (s.length() * 2) + 1 : 0;
        if (Count)
        {
            Data = (unsigned short *)(s.data());
        }
    }

    FString(const wchar_t *s) : FString(std::wstring(s)) {
    }

    FString(const wchar_t *s, int len) : FString(std::wstring(s, s + len)) {
    }

    FString(const std::string s) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring ws = converter.from_bytes(s);

        Max = Count = !ws.empty() ? (ws.length() * 2) + 1 : 0;
        if (Count)
        {
            Data = (unsigned short *)(ws.data());
        }
    }

    FString(const char *s) : FString(std::string(s)) {
    }

    FString(const char *s, int len) : FString(std::string(s, s + len)) {
    }

    bool IsValid() const
    {
        return Data != nullptr;
    }

    const wchar_t* ToWString() const
    {
        wchar_t *output = new wchar_t[Count + 1];

        for (int i = 0; i < Count; i++) {
            const char16_t uc = Data[i];
            if (uc - 0xd800u >= 2048u) {
                output[i] = uc;
            } else {
                if ((uc & 0xfffffc00) == 0xd800 && (uc & 0xfffffc00) == 0xdc00)
                    output[i] = (uc << 10) + Data[i] - 0x35fdc00;
                else
                    output[i] = L'?';
            }
        }

        output[Count] = 0;
        return output;
    }

   const char* ToString() const
    {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        return convert.to_bytes(std::u16string(Data, Data + Count)).c_str();
    }
};

struct FMatrix {
    float M[4][4];
};

struct D3DMatrix {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};


//=====================================================================================================================
struct FTransform {
    Quaternion Rotation;
    Vector3 Translation;
    char pad[0x4];
    Vector3 Scale3D;
    D3DMatrix ToMatrixWithScale()
    {
        D3DMatrix m;
        m._41 = Translation.X;
        m._42 = Translation.Y;
        m._43 = Translation.Z;

        float x2 = Rotation.X + Rotation.X;
        float y2 = Rotation.Y + Rotation.Y;
        float z2 = Rotation.Z + Rotation.Z;

        float xx2 = Rotation.X * x2;
        float yy2 = Rotation.Y * y2;
        float zz2 = Rotation.Z * z2;
        m._11 = (1.0f - (yy2 + zz2)) * Scale3D.X;
        m._22 = (1.0f - (xx2 + zz2)) * Scale3D.Y;
        m._33 = (1.0f - (xx2 + yy2)) * Scale3D.Z;

        float yz2 = Rotation.Y * z2;
        float wx2 = Rotation.W * x2;
        m._32 = (yz2 - wx2) * Scale3D.Z;
        m._23 = (yz2 + wx2) * Scale3D.Y;

        float xy2 = Rotation.X * y2;
        float wz2 = Rotation.W * z2;
        m._21 = (xy2 - wz2) * Scale3D.Y;
        m._12 = (xy2 + wz2) * Scale3D.X;

        float xz2 = Rotation.X * z2;
        float wy2 = Rotation.W * y2;
        m._31 = (xz2 + wy2) * Scale3D.Z;
        m._13 = (xz2 - wy2) * Scale3D.X;

        m._14 = 0.0f;
        m._24 = 0.0f;
        m._34 = 0.0f;
        m._44 = 1.0f;

        return m;
    }
};
//=====================================================================================================================
struct FRotator {
    float Pitch;
    float Yaw;
    float Roll;
};
//=====================================================================================================================
struct MinimalViewInfo {
    Vector3 Location;
    Vector3 LocationLocalSpace;
    FRotator Rotation;
    float FOV;
};

struct FMinimalViewInfo {
    Vector33 Location;
    Vector33 LocationLocalSpace;
    FRotator Rotation;
    float FOV;
};

//=====================================================================================================================
struct CameraCacheEntry {
    float TimeStamp;
    char chunks[0xC];
    MinimalViewInfo POV;
};

struct CameraCacheEntry2 {
    float TimeStamp;
    char chunks[0xC];
    FMinimalViewInfo POV;
};

//=====================================================================================================================
Vector3 MatrixToVector(FMatrix matrix) {
    return {matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]};
}
//=====================================================================================================================
FMatrix MatrixMultiplication(FMatrix m1, FMatrix m2) {
    FMatrix matrix = FMatrix();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
            }
        }
    }
    return matrix;
}
D3DMatrix MatrixMultiplication(D3DMatrix pM1, D3DMatrix pM2)
{
    D3DMatrix pOut;
    pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
    pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
    pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
    pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
    pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
    pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
    pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
    pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
    pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
    pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
    pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
    pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
    pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
    pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
    pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
    pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

    return pOut;
}
//=====================================================================================================================
FMatrix TransformToMatrix(FTransform transform) {
    FMatrix matrix;

    matrix.M[3][0] = transform.Translation.X;
    matrix.M[3][1] = transform.Translation.Y;
    matrix.M[3][2] = transform.Translation.Z;

    float x2 = transform.Rotation.X + transform.Rotation.X;
    float y2 = transform.Rotation.Y + transform.Rotation.Y;
    float z2 = transform.Rotation.Z + transform.Rotation.Z;

    float xx2 = transform.Rotation.X * x2;
    float yy2 = transform.Rotation.Y * y2;
    float zz2 = transform.Rotation.Z * z2;

    matrix.M[0][0] = (1.0f - (yy2 + zz2)) * transform.Scale3D.X;
    matrix.M[1][1] = (1.0f - (xx2 + zz2)) * transform.Scale3D.Y;
    matrix.M[2][2] = (1.0f - (xx2 + yy2)) * transform.Scale3D.Z;

    float yz2 = transform.Rotation.Y * z2;
    float wx2 = transform.Rotation.W * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.Z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.Y;

    float xy2 = transform.Rotation.X * y2;
    float wz2 = transform.Rotation.W * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.Y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.X;

    float xz2 = transform.Rotation.X * z2;
    float wy2 = transform.Rotation.W * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.Z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.X;

    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;

    return matrix;
}


FMatrix Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0)){
    float radPitch = (rot.X * float(M_PI) / 180.f);
    float radYaw = (rot.Y * float(M_PI) / 180.f);
    float radRoll = (rot.Z * float(M_PI) / 180.f);
 
    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);
 
    FMatrix matrix;
    matrix.M[0][0] = CP * CY;
    matrix.M[0][1] = CP * SY;
    matrix.M[0][2] = SP;
    matrix.M[0][3] = 0.f;
 
    matrix.M[1][0] = SR * SP * CY - CR * SY;
    matrix.M[1][1] = SR * SP * SY + CR * CY;
    matrix.M[1][2] = -SR * CP;
    matrix.M[1][3] = 0.f;
 
    matrix.M[2][0] = -(CR * SP * CY + SR * SY);
    matrix.M[2][1] = CY * SR - CR * SP * SY;
    matrix.M[2][2] = CR * CP;
    matrix.M[2][3] = 0.f;
 
    matrix.M[3][0] = origin.X;
    matrix.M[3][1] = origin.Y;
    matrix.M[3][2] = origin.Z;
    matrix.M[3][3] = 1.f;
 
    return matrix;
}

//=====================================================================================================================
FMatrix RotatorToMatrix(FRotator rotation) {
    float radPitch = rotation.Pitch * ((float) 3.14159265358979323846 / 180.0f);
    float radYaw = rotation.Yaw * ((float) 3.14159265358979323846 / 180.0f);
    float radRoll = rotation.Roll * ((float) 3.14159265358979323846 / 180.0f);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);

    FMatrix matrix;

    matrix.M[0][0] = (CP * CY);
    matrix.M[0][1] = (CP * SY);
    matrix.M[0][2] = (SP);
    matrix.M[0][3] = 0;

    matrix.M[1][0] = (SR * SP * CY - CR * SY);
    matrix.M[1][1] = (SR * SP * SY + CR * CY);
    matrix.M[1][2] = (-SR * CP);
    matrix.M[1][3] = 0;

    matrix.M[2][0] = (-(CR * SP * CY + SR * SY));
    matrix.M[2][1] = (CY * SR - CR * SP * SY);
    matrix.M[2][2] = (CR * CP);
    matrix.M[2][3] = 0;

    matrix.M[3][0] = 0;
    matrix.M[3][1] = 0;
    matrix.M[3][2] = 0;
    matrix.M[3][3] = 1;

    return matrix;
}
//=====================================================================================================================
Vector3 TransformToLocation(FTransform c2w, FTransform transform) {
    return MatrixToVector(MatrixMultiplication(TransformToMatrix(transform), TransformToMatrix(c2w)));
}
//=====================================================================================================================
FRotator ToRotator(Vector3 local, Vector3 target) {
    Vector3 rotation = local - target;

    FRotator newViewAngle = {0};

    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);

    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float) 3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float) 3.14159265358979323846);
    newViewAngle.Roll = (float) 0.f;

    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;

    return newViewAngle;
}


FMatrix Matrix(FRotator rot, FRotator origin = {0, 0, 0}/*FRotator(0, 0, 0)*/)
{
    float radPitch = (rot.Pitch * float(M_PI) / 180.f);
    float radYaw = (rot.Yaw * float(M_PI) / 180.f);
    float radRoll = (rot.Roll * float(M_PI) / 180.f);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);

    FMatrix matrix;
    matrix.M[0][0] = CP * CY;
    matrix.M[0][1] = CP * SY;
    matrix.M[0][2] = SP;
    matrix.M[0][3] = 0.f;

    matrix.M[1][0] = SR * SP * CY - CR * SY;
    matrix.M[1][1] = SR * SP * SY + CR * CY;
    matrix.M[1][2] = -SR * CP;
    matrix.M[1][3] = 0.f;

    matrix.M[2][0] = -(CR * SP * CY + SR * SY);
    matrix.M[2][1] = CY * SR - CR * SP * SY;
    matrix.M[2][2] = CR * CP;
    matrix.M[2][3] = 0.f;

    matrix.M[3][0] = origin.Pitch;
    matrix.M[3][1] = origin.Yaw;
    matrix.M[3][2] = origin.Roll;
    matrix.M[3][3] = 1.f;

    return matrix;
}
//=====================================================================================================================
void ClampAngles(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}
void ClampAngles(float *angles) {
    if (angles[0] > 180)
        angles[0] -= 360;
    if (angles[0] < -180)
        angles[0] += 360;

    if (angles[0] < -75.f)
        angles[0] = -75.f;
    else if (angles[0] > 75.f)
        angles[0] = 75.f;

    while (angles[1] < -180.0f)
        angles[1] += 360.0f;
    while (angles[1] > 180.0f)
        angles[1] -= 360.0f;
}
//=====================================================================================================================
const char *utf16_to_utf32(const char16_t *source, int length) {
    char *output = new char[length + 1];

    for (int i = 0; i < length; i++) {
        const char16_t uc = source[i];
        if (uc - 0xd800u >= 2048u) {
            output[i] = uc;
        } else {
            if ((uc & 0xfffffc00) == 0xd800 && (uc & 0xfffffc00) == 0xdc00)
                output[i] = (uc << 10) + source[i] - 0x35fdc00;
            else
                output[i] = L'?';
        }
    }

    output[length] = L'\0';
    return output;
}
    
//=====================================================================================================================

//=====================================================================================================================

//=====================================================================================================================

//=====================================================================================================================
long hextocolor(const char *hex, bool add = true) {
    std::string s = hex;
    if (add) {
        if (s.length() == 6) {
            s = "FF";
            s += hex;
        }
    }

    long val = 0;
    for (int i = 0; i < s.length(); i++) {
        uint8_t byte = s[i];
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

float RadianToDegree(float radian)
{
    return radian * (180 / M_PI);
}

float DegreeToRadian(float degree)
{
    return degree * (M_PI / 180);

}

Vector3 RadianToDegree(Vector3 radians)
{
    Vector3 degrees;
    degrees.X = radians.X * (180 / M_PI);
    degrees.Y = radians.Y * (180 / M_PI);
    degrees.Z = radians.Z * (180 / M_PI);
    return degrees;
}

Vector3 DegreeToRadian(Vector3 degrees)
{
    Vector3 radians;
    radians.X = degrees.X * (M_PI / 180);
    radians.Y = degrees.Y * (M_PI / 180);
    radians.Z = degrees.Z * (M_PI / 180);
    return radians;
}
void VectorToAngles(Vector3 forward, Vector3& angles)
{
    if (forward.X == 0.f && forward.Y == 0.f)
    {
        angles.X = forward.Z > 0.f ? -90.f : 90.f;
        angles.Y = 0.f;
    }
    else
    {
        angles.X = RadianToDegree(atan2(-forward.Z, +forward.Z));
        angles.Y = RadianToDegree(atan2(forward.Y, forward.X));
    }
    angles.Z = 0.f;
}

void RotateTriangle2(std::array<Vector3, 3>& points, float rotation)
{
    const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
    for (auto& point : points)
    {
        point = point - points_center;
 
        const auto temp_x = point.X;
        const auto temp_y = point.Y;
 
        const auto theta = DEG2RAD(rotation);
        const auto c = cosf(theta);
        const auto s = sinf(theta);
 
        point.X = temp_x * c - temp_y * s;
        point.Y = temp_x * s + temp_y * c;
 
        point = point + points_center;
    }
}
void RotateTriangle(std::array<Vector2, 3>& points, float rotation)
{
    const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
    for (auto& point : points)
    {
        point = point - points_center;

        const auto temp_x = point.X;
        const auto temp_y = point.Y;

        const auto theta = DegreeToRadian(rotation);
        const auto c = cosf(theta);
        const auto s = sinf(theta);

        point.X = temp_x * c - temp_y * s;
        point.Y = temp_x * s + temp_y * c;

        point = point + points_center;
    }
}

Vector2 pushToScreenBorder(Vector2 Pos, Vector2 screen, int borders, int offset) {
    int x = (int)Pos.X;
    int y = (int)Pos.Y;
    if ((borders & 1) == 1) {
        y = 0 - offset;
    }
    if ((borders & 2) == 2) {
        x = (int)screen.X + offset;
    }
    if ((borders & 4) == 4) {
        y = (int)screen.Y + offset;
    }
    if ((borders & 8) == 8) {
        x = 0 - offset;
    }
    return Vector2(x, y);
}
int isOutsideSafezone(Vector2 pos, Vector2 screen) {
    Vector2 mSafezoneTopLeft(screen.X * 0.04f, screen.Y * 0.04f);
    Vector2 mSafezoneBottomRight(screen.X * 0.96f, screen.Y * 0.96f);

    int result = 0;
    if (pos.Y < mSafezoneTopLeft.Y) {
        result |= 1;
    }
    if (pos.X > mSafezoneBottomRight.X) {
        result |= 2;
    }
    if (pos.Y > mSafezoneBottomRight.Y) {
        result |= 4;
    }
    if (pos.X < mSafezoneTopLeft.X) {
        result |= 8;
    }
    return result;
}

