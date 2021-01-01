
#ifndef Image_Width
#define Image_Width 640
#endif

#ifndef Image_Height
#define Image_Height 480
#endif

#ifndef RayStepCount
#define RayStepCount 120
#endif

#ifndef SampleDepth
#define SampleDepth 12
#endif

#define Image_Pitch ((((Image_Width * 3 - 1) / 4) + 1) * 4)

#include<cstdint>
#include<cstddef>
#include<array>
#include<cmath>

constexpr float FloatEpsilon = 0.00001f;
constexpr float CastEpsilon = 0.001f;
constexpr float CastEpsilon2 = CastEpsilon * 2;
constexpr float MaxDistance = 9999999.0f;

constexpr float pow(float x, uint32_t exp)
{
	float ret = 1.0f;
	while(exp --)
	{
		ret *= x;
	}
	return ret;
}

class vec4
{
public:
	float x, y, z, w;

	constexpr vec4() : x(0), y(0), z(0), w(0) {}
	constexpr vec4(const float s) : x(s), y(s), z(s), w(s) {}
	constexpr vec4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w) {}
	constexpr vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	constexpr vec4 operator + () const
	{
		return *this;
	}
	constexpr vec4 operator - () const
	{
		return vec4(-x, -y, -z, -w);
	}
	constexpr vec4 operator + (const vec4& v) const
	{
		return vec4
		(
			x + v.x,
			y + v.y,
			z + v.z,
			w + v.w
		);
	}
	constexpr vec4 operator - (const vec4& v) const
	{
		return vec4
		(
			x - v.x,
			y - v.y,
			z - v.z,
			w - v.w
		);
	}
	constexpr vec4 operator * (const float s) const
	{
		return vec4
		(
			x * s,
			y * s,
			z * s,
			w * s
		);
	}
	constexpr vec4 operator * (const vec4& v) const
	{
		return vec4
		(
			x * v.x,
			y * v.y,
			z * v.z,
			w * v.w
		);
	}
	constexpr vec4 operator / (const vec4& v) const
	{
		return vec4
		(
			x / v.x,
			y / v.y,
			z / v.z,
			w / v.w
		);
	}
	constexpr vec4& operator += (const vec4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	constexpr vec4& operator -= (const vec4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	constexpr vec4& operator *= (const float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	constexpr vec4& operator *= (const vec4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	constexpr vec4& operator /= (const vec4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}
	constexpr vec4& operator ++ ()
	{
		x ++;
		y ++;
		z ++;
		w ++;
		return *this;
	}
	constexpr vec4& operator -- ()
	{
		x --;
		y --;
		z --;
		w --;
		return *this;
	}
	constexpr vec4 operator ++ (int)
	{
		vec4 ret(*this);
		x ++;
		y ++;
		z ++;
		w ++;
		return ret;
	}
	constexpr vec4 operator -- (int)
	{
		vec4 ret(*this);
		x --;
		y --;
		z --;
		w --;
		return ret;
	}
	constexpr float dot(const vec4& v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	constexpr vec4 cross(const vec4& v) const
	{
		return vec4
		(
			y * v.z - v.y * z,
			z * v.x - v.z * x,
			x * v.y - v.x - y,
			0.0
		);
	}
	constexpr float length() const
	{
		return sqrtf(dot(*this));
	}
	constexpr float distance(const vec4& v) const
	{
		return ((*this) - v).length();
	}
	constexpr vec4 normalize() const
	{
		float len = length();
		if(fabsf(len) >= FloatEpsilon)
		{
			return (*this) / len;
		}
	}
	constexpr vec4 reflect(const vec4& normal) const
	{
		return (*this) - normal * normal.dot(*this) * 2.0f;
	}
	constexpr vec4 mix(const vec4& v, const float a) const
	{
		return (*this) * (1.0f - a) + v * a;
	}
};

enum ObjectType
{
	Obj_Null = 0,
	Obj_Ground,
	Obj_Sphere
};

class RenderObject
{
public:
	ObjectType Type;
	vec4 Color1;
	vec4 Color2;

	// No class inheritance for constexpr, so there's a workaround
	// Members for 'Ground'
	float GridDensity;

	// Members for 'Sphere'
	vec4 Position;
	float Radius;

	constexpr RenderObject() :
		Type(Obj_Null),
		Color1(0),
		Color2(0),
		GridDensity(0),
		Position(0),
		Radius(0)
	{
	}

	constexpr bool IsGround() const
	{
		return Type == Obj_Ground;
	}

	constexpr bool IsSphere() const
	{
		return Type == Obj_Sphere;
	}

	constexpr float SDF(const vec4& Point) const;
	constexpr vec4 Normal(const vec4& Point) const;
	constexpr vec4 Color(const vec4& Point) const;

private:
	constexpr float Ground_SDF(const vec4& Point) const
	{
		return Point.y;
	}
	constexpr vec4 Ground_Normal(const vec4& Point) const
	{
		if(Point.y >= 0) return vec4(0, 1, 0, 0);
		else return vec4(0, -1, 0, 0);
	}
	constexpr vec4 Ground_Color(const vec4& Point) const
	{
		int32_t px = Point.x * GridDensity;
		int32_t pz = Point.z * GridDensity;
		return (px & 1) ^ (pz & 1) ? Color1 : Color2;
	}
	constexpr float Sphere_SDF(const vec4& Point) const
	{
		return Point.distance(Position) - Radius;
	}
	constexpr vec4 Sphere_Normal(const vec4& Point) const
	{
		return (Point - Position).normalize();
	}
	constexpr vec4 Sphere_Color(const vec4& Point) const
	{
		return Color1;
	}
};

constexpr float RenderObject::SDF(const vec4& Point) const
{
	if(IsGround()) return Ground_SDF(Point);
	if(IsSphere()) return Sphere_SDF(Point);
	return MaxDistance;
}

constexpr vec4 RenderObject::Normal(const vec4& Point) const
{
	if(IsGround()) return Ground_Normal(Point);
	if(IsSphere()) return Sphere_Normal(Point);
	return vec4(0);
}

constexpr vec4 RenderObject::Color(const vec4& Point) const
{
	if(IsGround()) return Ground_Color(Point);
	if(IsSphere()) return Sphere_Color(Point);
	return vec4(0);
}

class Scene
{
public:
	vec4 CamPos;
	RenderObject SceneObjects[16];
	size_t ObjectCount;
	vec4 LightDir;
	uint32_t LightPow;
	vec4 LightColor;
	vec4 FogColor;
	vec4 SkyColor;
	float FogDistance;

	constexpr Scene() :
		CamPos(0, 2, 7, 0),
		SceneObjects
		{
			Ground(vec4(1.0f), vec4(0.5f), 2.0f),
			Sphere(vec4( 0, 2, 0, 0), 2, vec4(0.75f, 1.0f, 0.1f, 1.0f)),
			Sphere(vec4( 1, 1, 2, 0), 1, vec4(0.1f, 0.75f, 1.0f, 1.0f)),
			Sphere(vec4(-3, 1, 0, 0), 1, vec4(1.0f, 0.1f, 0.75f, 1.0f))
		},
		ObjectCount(4),
		LightDir(vec4(1, -1, 1, 0).normalize()),
		LightPow(20),
		LightColor(1.0f, 0.8f, 0.6f, 0.0f),
		FogColor(0.8f, 0.9f, 1.0f, 0.0f),
		SkyColor(0.2f, 0.5f, 0.8f, 0.0f),
		FogDistance(100.0f)
	{
	}

	constexpr vec4 GetSkyColor(const vec4& RayToSky)
	{
		float SkyFog = 1.0 - RayToSky.y;
		if (SkyFog < 0) SkyFog = 0;
		float SkySun = -LightDir.dot(RayToSky);
		return SkyColor.mix(FogColor, SkyFog) + LightColor * pow(SkySun, LightPow);
	}

	constexpr float Map_Dist(const vec4& Point, size_t *ObjectIndex)
	{
		float NearestDist = MaxDistance;
		size_t ObjId = 0;

		for(size_t i = 0; i < ObjectCount; i++)
		{
			float Dist = SceneObjects[i].SDF(Point);
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				ObjId = i;
			}
		}

		if (ObjectIndex) *ObjectIndex = ObjId;
		return NearestDist;
	}

	constexpr vec4 Map_Normal(const vec4& Point, size_t ObjectIndex)
	{
		return SceneObjects[ObjectIndex].Normal(Point);
	}

	constexpr vec4 Map_Color(const vec4& Point, size_t ObjectIndex)
	{
		return SceneObjects[ObjectIndex].Color(Point);
	}

	constexpr float Map_Cast(const vec4& RayOrg, const vec4& RayDir, size_t *ObjectIndex)
	{
		float Dist = 0.0f;
		for(int i = 0; i < RayStepCount; i++)
		{
			vec4 CurOrg = RayOrg + RayDir * Dist;
			size_t OI = 0;
			float CurDist = Map_Dist(CurOrg, &OI);
			Dist += CurDist;
			if(CurDist <= CastEpsilon)
			{
				if(ObjectIndex) *ObjectIndex = OI;
				return Dist;
			}
		}
		return -1;
	}

	constexpr vec4 RenderScene(const vec4& RayOrg, const vec4& RayDir)
	{
		vec4 CurOrg = RayOrg;
		vec4 CurDir = RayDir;
		vec4 Mask(1);

		for(int i = 0; i < SampleDepth; i++)
		{
			size_t OI = 0;
			float CastDist = Map_Cast(CurOrg, CurDir, &OI);
			if(CastDist < 0) break;

			vec4 CastPos = CurOrg + CurDir * CastDist;
			vec4 CastNormal = Map_Normal(CastPos, OI);
			vec4 ObjColor = Map_Color(CastPos, OI);

			float Foggy = CastDist / FogDistance;
			if(Foggy > 1.0f) Foggy = 1.0f;
			Mask *= ObjColor.mix(FogColor, Foggy);

			CurOrg += CurDir * CastDist;
			CurDir = CurDir.reflect(CastNormal);
			CurOrg += CurDir * CastEpsilon2;
		}
		return GetSkyColor(CurDir) * Mask;
	}

private:
	// Pretend as the constructor of 'subclass' Ground.
	constexpr RenderObject Ground(const vec4& Color1, const vec4& Color2, const float GridDensity)
	{
		RenderObject ret;
		ret.Type = Obj_Ground;
		ret.Color1 = Color1;
		ret.Color2 = Color2;
		ret.GridDensity = GridDensity;
		return ret;
	}
	// Pretend as the constructor of 'subclass' Sphere.
	constexpr RenderObject Sphere(const vec4& Position, const float Radius, const vec4& Color)
	{
		RenderObject ret;
		ret.Type = Obj_Sphere;;
		ret.Position = Position;
		ret.Radius = Radius;
		ret.Color1 = Color;
		return ret;
	}
};

// The bitmap data initialized during compile time.
__attribute__((section(".Pixels")))
constexpr auto Pixels{[]() constexpr
{
	Scene s;
	std::array<uint8_t, Image_Pitch * Image_Height> result{};
	const size_t Pitch = Image_Pitch;
	for (int y = 0; y < Image_Height; y++)
	{
		size_t i = y * Pitch;
		float v = (y * 2.0f - Image_Height) / Image_Height;
		for(int x = 0; x < Image_Width; x++)
		{
			float u = (x * 2.0f - Image_Width) / Image_Height;
			vec4 PixelRay = vec4(u, v, -1.75f, 0).normalize();
			vec4 ColorValue = s.RenderScene(s.CamPos, PixelRay) * 255;
			if(ColorValue.x < 0) ColorValue.x = 0; else if(ColorValue.x > 255) ColorValue.x = 255;
			if(ColorValue.y < 0) ColorValue.y = 0; else if(ColorValue.y > 255) ColorValue.y = 255;
			if(ColorValue.z < 0) ColorValue.z = 0; else if(ColorValue.z > 255) ColorValue.z = 255;
			result[i++] = ColorValue.z;
			result[i++] = ColorValue.y;
			result[i++] = ColorValue.x;
		}
	}
	return result;
}()};

typedef struct __attribute__((packed)) BitmapFileHeader_struct
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
}BitmapFileHeader_t, * BitmapFileHeader_p;

typedef struct __attribute__((packed)) BitmapInfoHeader_struct
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biGrounds;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
}BitmapInfoHeader_t, * BitmapInfoHeader_p;

extern __attribute__((section(".BitmapFile"))) char EndOfBitmap;

__attribute__((section(".BMFH")))
BitmapFileHeader_t BMFH =
{
	0x4D42,
	(size_t)&EndOfBitmap,
	0, 0,
	(size_t)&Pixels
};

__attribute__((section(".BMIF")))
BitmapInfoHeader_t BMIF =
{
	40,
	Image_Width,
	Image_Height,
	1, 24,
	0,
	(size_t)&EndOfBitmap - (size_t)&Pixels,
	0,
	0,
	0,
	0
};

// Using a function for the entry, to prevent removal of the structures during LTO.
extern "C" int dummy();
int dummy()
{
	return (int)(
		(size_t)&BMFH +
		(size_t)&BMIF +
		(size_t)&Pixels +
		0);
}


