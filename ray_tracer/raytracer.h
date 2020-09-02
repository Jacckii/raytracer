#pragma once

#include <vector>
#include "objects.h"
#include "vector.h"
#include "color.h"

class Raytracer {
public:
	Raytracer();
	~Raytracer();

	inline Vec2 getCanvasSize() {
		return resolution;
	}
	
	inline void setCanvasSize(const Vec2& size)	{
		if (size == resolution)
			return;

		resolution = size;
		should_update = true;
	}

	inline Vec3 getCamOrigin() {
		return cam.origin;
	}

	inline void setCamOrigin(const Vec3& origin) {
		if (origin == cam.origin)
			return;

		cam.origin = origin;
		cam.Update();
		should_update = true;
	}

	inline Vec3 getCamDirection() {
		return cam.direction;
	}

	inline void setCamDirection(const Vec3& direction) {
		if (direction == cam.direction)
			return;

		cam.direction = direction;
		cam.Update();
		should_update = true;
	}

	inline void setCamUpVec(const Vec3& up_vec) {
		if (up_vec == cam.up_vec)
			return;

		cam.up_vec = up_vec;
		cam.Update();
		should_update = true;
	}

	inline void addPlane(Vec3 origin, Vec3 normal, Color color) {
		objects.push_back(std::make_shared<Plane>(origin, normal, color));
		should_update = true;
	}

	inline void addTriangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color) {
		objects.push_back(std::make_shared<Triangle>(p1, p2, p3, color));
		should_update = true;
	}

	inline void addSphere(Vec3 origin, float radius, Color color) {
		objects.push_back(std::make_shared<Sphere>(origin, radius, color));
		should_update = true;
	}

	inline void addLight(Vec3 origin, Color color) {
		lights.emplace_back(origin, color);
		should_update = true;
	}

	inline std::vector<Color>& getCanvasData() {
		return canvas;
	}

	inline void forceUpdate() {
		should_update = true;
	}

	inline std::vector<std::shared_ptr<Objects>>& getObjectsData() {
		return objects;
	}

	inline std::vector<Light>& getLightData() {
		return lights;
	}

	bool processImage();
private:
	Vec2 pxCountToScreenPos(float index);
protected:
	std::vector<Color> canvas;
	std::vector<std::shared_ptr<Objects>> objects;
	std::vector<Light> lights;
	Vec2 resolution;
	Cam cam;
	bool should_update;
};