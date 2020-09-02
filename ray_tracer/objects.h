#pragma once

#include <array>
#include "vector.h"
#include "color.h"
#include "ray.h"

enum {
	PLANE,
	TRIANGLE,
	SPHERE,
	LIGHT
};

class Objects {
public:
	virtual ~Objects() = default;
	virtual bool intersect(Ray& ray) = 0;
	virtual Color getColor() = 0;
	virtual void setColor(Color col) = 0;
	virtual Vec3 getOrigin() = 0;
	virtual Vec3 getNormal() = 0;
	virtual int getType() = 0;
};

class Plane : public Objects {
public:
	Plane(Vec3 origin, Vec3 normal, Color color);
	~Plane() override;
	bool intersect(Ray& ray) override;
	Color getColor() override;
	Vec3 getOrigin() override;
	Vec3 getNormal() override;
	void setOrigin(Vec3 origin);
	void setNormal(Vec3 normal);
	void setColor(Color col) override;
	int getType() override;
private:
	Vec3 origin;
	Vec3 normal;
	Color color;
	float d;
};

class Triangle : public Objects {
public:
	Triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color);
	~Triangle() override;
	bool intersect(Ray& ray) override;
	Color getColor() override;
	Vec3 getOrigin() override; //getConter
	std::array<Vec3, 3>& getPoints();
	void setPoints(Vec3 p1, Vec3 p2, Vec3 p3);
	Vec3 getNormal() override;
	void update();
	void setColor(Color col) override;
	int getType() override;
private:
	bool inTrinagle(Vec3 interset_point);
	std::array<Vec3, 3> origin;
	Vec3 normal;
	Vec3 center;
	Color color;
};

class Sphere : public Objects {
public:
	Sphere(Vec3 origin, float radius, Color color);
	~Sphere() override;
	bool intersect(Ray& ray) override;
	Color getColor() override;
	Vec3 getOrigin() override;
	void setOrigin(Vec3 origin);
	Vec3 getNormal() override;
	float getRadius();
	void setRadius(float rad);
	void setColor(Color col) override;
	int getType() override;
private:
	Vec3 origin;
	float radius;
	Color color;
};

struct Cam {
	Vec3 origin;
	Vec3 up_vec;
	Vec3 direction;
	float distance;

	Vec3 center;
	Vec3 p0;
	Vec3 p1;
	Vec3 p2;
	Cam();
	Cam(Vec3 origin, Vec3 direction, Vec3 up_vec, float distance);
	void Update();
};

struct Light {
	Vec3 origin;
	Color color;
	Light() : origin(Vec3(0, 0, 0)), color(Color(255,255,255,255)) {}
	Light(Vec3 origin, Color color) : origin(origin), color(color) {}
};