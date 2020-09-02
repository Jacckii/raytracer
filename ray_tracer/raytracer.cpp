#include "raytracer.h"

Raytracer::Raytracer()
{
	cam = Cam(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0), 1.f);
	resolution.invalidate();
	should_update = true;
}

Raytracer::~Raytracer()
{
}

bool Raytracer::processImage()
{
	if (!should_update)
		return false;

	if (!resolution.isValid())
		return false;

	auto px_count = resolution.x * resolution.y;

	canvas.clear();
	canvas.reserve((unsigned int)px_count);
	
	for (unsigned int i = 0; i < px_count; i++) {
		auto canvas_point = pxCountToScreenPos(i);
		float u = canvas_point.x / resolution.x;
		float v = canvas_point.y / resolution.y;
		Vec3 screen_point = cam.p0 + (cam.p1 - cam.p0) * u + (cam.p2 - cam.p0) * v;
		Vec3 ray_direction = screen_point - cam.origin;
		float best_lenght = std::numeric_limits<float>::max();
		int best_index = -1;
		Ray best_ray = Ray();
		for (unsigned int t = 0; t < objects.size(); t++) {
			Ray ray = Ray(cam.origin, ray_direction.normalize(), 1000000.f);
			if (objects[t]->intersect(ray)) {
				if (ray.length < best_lenght) {
					best_lenght = ray.length;
					best_index = t;
					best_ray = ray;
				}
			}
		}

		if (objects.size() > best_index && best_index >= 0) {
			float global_light_intensity = 0.1f;
			Color obj_color = objects[best_index]->getColor();
			Color current_color = Color(obj_color.r * global_light_intensity, obj_color.g * global_light_intensity, obj_color.b * global_light_intensity, 255);
			Vec3 ray_hit_point = best_ray.origin + best_ray.direction * best_ray.length;
			for (auto& iter : lights)
			{
				Vec3 diffuse_light_normal = Vec3();
				if (objects[best_index]->getType() == TRIANGLE)
					diffuse_light_normal = (ray_hit_point + objects[best_index]->getNormal() * 1e-3);
				else
					diffuse_light_normal = (ray_hit_point - objects[best_index]->getOrigin()).normalize();

				Vec3 light_direction = (iter.origin - ray_hit_point).normalize();
				float light_strenght = clamp(diffuse_light_normal.dot(light_direction), 0.f, std::numeric_limits<float>::max());
				Vec3 multiplicator = Vec3(iter.color.r / 255.f, iter.color.g / 255.f, iter.color.b / 255.f) * light_strenght;
				float alpha = (iter.color.a / 255.f) * light_strenght;
				auto object_color = objects[best_index]->getColor();
				Color add_color = Color(object_color.r * multiplicator.x, object_color.g * multiplicator.y, object_color.b * multiplicator.z, object_color.a * alpha);
				current_color = current_color + add_color;
			}
			canvas.push_back(current_color);
			continue;
		}
		canvas.push_back(Color(0, 0, 0, 255));
	}

	should_update = false;
	return true;
}

Vec2 Raytracer::pxCountToScreenPos(float index)
{
	float y = 0;
	while (index > resolution.x) {
		index -= resolution.x;
		y++;
	}

	return Vec2(index, y);
}
