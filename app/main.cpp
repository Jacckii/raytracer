#include <iostream>
#include <dinput.h>
#include <tchar.h>
#include "../ray_tracer/raytracer.h"

void initGui();
void runGui(MSG& msg, Raytracer& t);
void destroyGui();
void renderPixels(std::vector<Color>& data, float width, float height);

int main()
{
	Raytracer tracer;
	tracer.setCanvasSize(Vec2(500, 500));
	tracer.setCamOrigin(Vec3(0, 0, 0));
	tracer.setCamUpVec(Vec3(0, 1, 0));

	initGui();
	
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (tracer.processImage())
			renderPixels(tracer.getCanvasData(), tracer.getCanvasSize().x, tracer.getCanvasSize().y);

		runGui(msg, tracer);
	}

	destroyGui();
}

