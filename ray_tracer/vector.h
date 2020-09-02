#pragma once
#include <limits>

struct Vec3 {
	float x;
	float y;
	float z;
	
	Vec3() { x = y = z = std::numeric_limits<float>::infinity(); }
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3 operator + (const Vec3& vec) const { return Vec3(x + vec.x, y + vec.y, z + vec.z); }
	Vec3 operator - (const Vec3& vec) const { return Vec3(x - vec.x, y - vec.y, z - vec.z); }
	Vec3 operator * (const Vec3& vec) const { return Vec3(x * vec.x, y * vec.y, z * vec.z); }
	Vec3 operator * (float fl) const { return Vec3(x * fl, y * fl, z * fl); }
	Vec3 operator / (const Vec3& vec) const { return Vec3(x / vec.x, y / vec.y, z / vec.z); }
	Vec3 operator / (float fl) const { return Vec3(x / fl, y / fl, z / fl); }
	bool operator==(const Vec3& vec) const { return (vec.x == x) && (vec.y == y) && (vec.z == z); }
	bool operator!=(const Vec3& vec) const { return (vec.x != x) || (vec.y != y) || (vec.z != z); }

	inline bool isValid() const { 
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z); 
	}

	inline void invalidate() {
		x = y = z = std::numeric_limits<float>::infinity();
	}

	inline float dot(const Vec3& vec) const {
		return (x * vec.x + y * vec.y + z * vec.z);
	}

	inline Vec3 normalize() const {
		float mg = sqrt(x*x + y * y + z * z);
		return Vec3(x / mg, y / mg, z / mg);
	}

	inline Vec3 cross(const Vec3& v) {
		float Nx = (y * v.z) - (z * v.y);
		float Ny = (z * v.x) - (x * v.z);
		float Nz = (x * v.y) - (y * v.x);
		return Vec3(Nx, Ny, Nz);
	}
};

struct Vec2 {
	float x;
	float y;

	Vec2() { x = y = std::numeric_limits<float>::infinity(); }
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2 operator + (const Vec2& vec) const { return Vec2(x + vec.x, y + vec.y); }
	Vec2 operator - (const Vec2& vec) const { return Vec2(x - vec.x, y - vec.y); }
	Vec2 operator * (const Vec2& vec) const { return Vec2(x * vec.x, y * vec.y); }
	Vec2 operator * (float fl) const { return Vec2(x * fl, y * fl); }
	Vec2 operator / (const Vec2& vec) const { return Vec2(x / vec.x, y / vec.y); }
	Vec2 operator / (float fl) const { return Vec2(x / fl, y / fl); }
	bool operator==(const Vec2& vec) const { return (vec.x == x) && (vec.y == y); }
	bool operator!=(const Vec2& vec) const { return (vec.x != x) || (vec.y != y); }

	inline bool isValid() const {
		return std::isfinite(x) && std::isfinite(y);
	}

	inline void invalidate() {
		x = y = std::numeric_limits<float>::infinity();
	}

	inline float dot(const Vec2& vec) const {
		return (x * vec.x + y * vec.y);
	}

	inline Vec2 normalize() const {
		float mg = sqrt(x*x + y * y);
		return Vec2(x / mg, y / mg);
	}
};