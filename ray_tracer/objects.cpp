#include "objects.h"

constexpr float kEpsilon = 1e-8;

Plane::Plane(Vec3 origin, Vec3 n, Color color) : origin(origin), color(color) {
	normal = n.normalize();
	d = fabsf(normal.x *(-origin.x) + n.y*(-origin.y) + n.z*(origin.z));
}
Plane::~Plane() { }

bool Plane::intersect(Ray& ray) {
	float check = ray.direction.dot(normal);
	float t = -1 * ((normal.dot(ray.origin) + d) / check);

	if (t < ray.length && t > 0.f)
		return ray.length = t, true;

	return false;
}

Color Plane::getColor() {
	return color;
}

Vec3 Plane::getOrigin() {
	return origin;
}

Vec3 Plane::getNormal() {
	return normal;
}

void Plane::setOrigin(Vec3 orig) {
	origin = orig;
}

void Plane::setNormal(Vec3 norm) {
	normal = norm;
}

void Plane::setColor(Color col) {
	color = col;
}

int Plane::getType() {
	return PLANE;
}

Triangle::Triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color) : color(color) { 
	origin = { p1, p2, p3 }; 
	update();
}
Triangle::~Triangle() { }

bool Triangle::intersect(Ray& ray) {

	float check = ray.direction.dot(normal);

	if (fabs(check) < kEpsilon) return false;

	float dist = normal.dot(origin[0]);
	float t = (normal.dot(ray.origin) + dist) / check;
	
	if (t < 0) 
		return false;

	Vec3 hitPoint = ray.origin + (ray.direction * t);

	if (inTrinagle(hitPoint))
		return ray.length = t, true;

	return false;
}

Color Triangle::getColor() {
	return color;
}

Vec3 Triangle::getOrigin() {
	return center;
}

std::array<Vec3, 3>& Triangle::getPoints() {
	return origin;
}

void Triangle::setPoints(Vec3 p1, Vec3 p2, Vec3 p3) {
	origin[0] = p1;
	origin[1] = p2;
	origin[2] = p3;
}

Vec3 Triangle::getNormal() {
	return normal;
}

void Triangle::update() {
	Vec3 v = origin[0] - origin[1];
	Vec3 w = origin[2] - origin[0];

	normal = v.cross(w).normalize();
	center = (origin[0] + origin[1] + origin[2]) / 3;
}

void Triangle::setColor(Color col) {
	color = col;
}

int Triangle::getType() {
	return TRIANGLE;
}

bool Triangle::inTrinagle(Vec3 interset_point) {
	Vec3 v0 = origin[1] - origin[0];
	Vec3 v1 = origin[2] - origin[0];
	Vec3 v2 = interset_point - origin[0];

	float d00, d01, d02, d11, d12;
	d00 = v0.dot(v0);
	d01 = v0.dot(v1);
	d02 = v0.dot(v2);
	d11 = v1.dot(v1);
	d12 = v1.dot(v2);

	float invDenom = 1 / (d00 * d11 - d01 * d01);
	float u = (d11 *d02 - d01 * d12) * invDenom;
	float v = (d00 *d12 - d01 * d02) * invDenom;
	return ((u >= 0) && (v >= 0) && (u + v < 1));
}

Sphere::Sphere(Vec3 origin, float radius, Color color) : origin(origin), radius(radius), color(color) { }
Sphere::~Sphere() { }

bool Sphere::intersect(Ray& ray) {
	Vec3 originToSphere = origin - ray.origin;
	float projection = originToSphere.dot(ray.direction);
	Vec3 distanceVec = originToSphere - ray.direction * projection;
	float distanceSq = distanceVec.dot(distanceVec);
	float radiusSq = radius * radius;
	
	if (distanceSq > radiusSq)
		return false;

	float length = projection - sqrtf(radiusSq - distanceSq);
	if (length < ray.length && length > 0.f)
		return ray.length = length, true;
	
	return false;
}

Color Sphere::getColor() {
	return color;
}

Vec3 Sphere::getOrigin() {
	return origin;
}

void Sphere::setOrigin(Vec3 orig) {
	origin = orig;
}

Vec3 Sphere::getNormal() {
	return Vec3();
}

float Sphere::getRadius() {
	return radius;
}

void Sphere::setRadius(float rad) {
	radius = rad;
}

void Sphere::setColor(Color col) {
	color = col;
}

int Sphere::getType() {
	return SPHERE;
}

Cam::Cam() {
	origin = direction = up_vec = Vec3(0, 0, 0);
	distance = 1.f;
	Update();
}

Cam::Cam(Vec3 origin, Vec3 direction, Vec3 up_vec, float distance) : origin(origin), direction(direction), up_vec(up_vec), distance(distance) {
	Update();
}

void Cam::Update() {
	center = origin + direction * distance;
	p0 = center + Vec3(-1, 1, 0); //left bottom
	p1 = center + Vec3(1, 1, 0); //right bottom
	p2 = center + Vec3(-1, -1, 0); //left top
}
