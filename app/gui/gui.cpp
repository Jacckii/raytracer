#include "..\..\ray_tracer\raytracer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <stdint.h>
#include <vector>

// Data
static ID3D10Device*            g_pd3dDevice = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D10RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND hwnd;
WNDCLASSEX wc;

ID3D10Texture2D* texture2d = NULL;
ID3D10ShaderResourceView* view = NULL;

void updateTexture(float width, float height, uint32_t *data) {
	if (texture2d)
		texture2d->Release();

	D3D10_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = width * height * 4;

	auto result = g_pd3dDevice->CreateTexture2D(&desc, &initData, &texture2d);
	result = g_pd3dDevice->CreateShaderResourceView(texture2d, NULL, &view);
}

void renderPixels(std::vector<Color>& data, float width, float height)
{
	static std::vector<uint32_t> dx_data;
	dx_data.clear();
	for (auto& iter : data) {
		dx_data.push_back(iter.r | (iter.g << 8) | (iter.b << 16) | (iter.a << 24));
	}
	updateTexture(width, height, dx_data.data());	
}

// Main code
void initGui()
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("RayTracer"), NULL };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, _T("RayTracer"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX10_Init(g_pd3dDevice);
}

const char* object_type_names[] = { "Plane", "Triangle", "Sphere", "Light" };

static bool ShowPlaceholderObject(const char* prefix, int id, int type, Raytracer& tracer)
{
	// Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	if (type == LIGHT)
		ImGui::PushID(tracer.getObjectsData().size() + id);
	else
		ImGui::PushID(id);

	// Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
	ImGui::AlignTextToFramePadding();
	bool node_open = ImGui::TreeNode("Object", "%s_%u - %s", prefix, id, object_type_names[type]);
	ImGui::NextColumn();
	ImGui::AlignTextToFramePadding();

	if (ImGui::Button("Remove")) {	
		if (type == LIGHT)
		{
			auto& objects = tracer.getLightData();
			objects.erase(objects.begin() + id);
			tracer.forceUpdate();
			ImGui::PopID();
			return true;
		}
		else
		{
			auto& objects = tracer.getObjectsData();
			objects.erase(objects.begin() + id);
			tracer.forceUpdate();
			ImGui::PopID();
			return true;
		}
	}

	ImGui::NextColumn();
	if (node_open)
	{
		if (type == PLANE)
		{
			auto& objects = tracer.getObjectsData();
			ImGui::Separator();
			auto& object = (std::shared_ptr<Plane>&)objects[id];
			Vec3 origin = object->getOrigin();
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Origin", flags, "Origin");
			ImGui::NextColumn();
			ImGui::SliderFloat("X##planeOrigin", &(origin.x), -10, 10);
			ImGui::SliderFloat("Y##planeOrigin", &(origin.y), -10, 10);
			ImGui::SliderFloat("Z##planeOrigin", &(origin.z), -10, 10);
			ImGui::NextColumn();

			if (object->getOrigin() != origin) {
				object->setOrigin(origin);
				tracer.forceUpdate();
			}
			ImGui::Separator();

			Vec3 normal = object->getNormal();
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Normal", flags, "Normal");
			ImGui::NextColumn();
			ImGui::SliderFloat("X##planeNormal", &(normal.x), -10, 10);
			ImGui::SliderFloat("Y##planeNormal", &(normal.y), -10, 10);
			ImGui::SliderFloat("Z##planeNormal", &(normal.z), -10, 10);
			ImGui::NextColumn();

			if (object->getNormal() != normal) {
				object->setNormal(normal);
				tracer.forceUpdate();
			}

			ImGui::Separator();

			Color col = object->getColor();
			float color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			float unchanged_color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Color", flags, "Color");
			ImGui::NextColumn();
			
			ImGui::ColorPicker4("Color##plane", color);
			ImGui::NextColumn();

			for (int i = 0; i < 4; i++)	{
				if (color[i] != unchanged_color[i])	{
					object->setColor(Color(color[0] * 255.f, color[1] * 255.f, color[2] * 255.f, color[3] * 255.f));
					tracer.forceUpdate();
					break;
				}
			}

			ImGui::TreePop();
		}		
		else if (type == TRIANGLE)
		{
			auto& objects = tracer.getObjectsData();
			ImGui::Separator();
			auto& object = (std::shared_ptr<Triangle>&)objects[id];
			auto origin = object->getPoints();
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Origin", flags, "Origin");
			ImGui::NextColumn();
			for (int i = 0; i < 3; i++)
			{
				ImGui::Text("Point %d:", i);
				ImGui::PushID(i);
				ImGui::SliderFloat("X##triangleOrigin", &(origin[i].x), -10, 10);
				ImGui::SliderFloat("Y##triangleOrigin", &(origin[i].y), -10, 10);
				ImGui::SliderFloat("Z##triangleOrigin", &(origin[i].z), -10, 10);
				ImGui::PopID();
			}
			ImGui::NextColumn();

			if (object->getPoints()[0] != origin[0] || object->getPoints()[1] != origin[1] || object->getPoints()[2] != origin[2]) {
				object->setPoints(origin[0], origin[1], origin[2]);
				tracer.forceUpdate();
			}
			ImGui::Separator();

			Color col = object->getColor();
			float color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			float unchanged_color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Color", flags, "Color");
			ImGui::NextColumn();

			ImGui::ColorPicker4("Color##triangle", color);
			ImGui::NextColumn();

			for (int i = 0; i < 4; i++) {
				if (color[i] != unchanged_color[i]) {
					object->setColor(Color(color[0] * 255.f, color[1] * 255.f, color[2] * 255.f, color[3] * 255.f));
					tracer.forceUpdate();
					break;
				}
			}

			ImGui::TreePop();
		}
		else if (type == SPHERE)
		{
			auto& objects = tracer.getObjectsData();
			ImGui::Separator();
			auto& object = (std::shared_ptr<Sphere>&)objects[id];
			Vec3 origin = object->getOrigin();
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Origin", flags, "Origin");
			ImGui::NextColumn();
			ImGui::SliderFloat("X##sphereOrigin", &(origin.x), -10, 10);
			ImGui::SliderFloat("Y##sphereOrigin", &(origin.y), -10, 10);
			ImGui::SliderFloat("Z##sphereOrigin", &(origin.z), -10, 10);
			ImGui::NextColumn();

			if (object->getOrigin() != origin) {
				object->setOrigin(origin);
				tracer.forceUpdate();
			}
			ImGui::Separator();

			float radius = object->getRadius();
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Radius", flags, "Radius");
			ImGui::NextColumn();
			ImGui::SliderFloat("Radius##plane", &radius, -10, 10);
			ImGui::NextColumn();

			if (object->getRadius() != radius) {
				object->setRadius(radius);
				tracer.forceUpdate();
			}

			ImGui::Separator();

			Color col = object->getColor();
			float color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			float unchanged_color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Color", flags, "Color");
			ImGui::NextColumn();

			ImGui::ColorPicker4("Color##sphere", color);
			ImGui::NextColumn();

			for (int i = 0; i < 4; i++) {
				if (color[i] != unchanged_color[i]) {
					object->setColor(Color(color[0] * 255.f, color[1] * 255.f, color[2] * 255.f, color[3] * 255.f));
					tracer.forceUpdate();
					break;
				}
			}

			ImGui::TreePop();
		}
		else if (type == LIGHT)
		{
			auto& lights = tracer.getLightData();
			ImGui::Separator();
			auto& object = (Light&)lights[id];
			Vec3 un_changed_origin = object.origin;
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Origin", flags, "Origin");
			ImGui::NextColumn();
			ImGui::SliderFloat("X##light", &(object.origin.x), -10, 10);
			ImGui::SliderFloat("Y##light", &(object.origin.y), -10, 10);
			ImGui::SliderFloat("Z##light", &(object.origin.z), -10, 10);
			ImGui::NextColumn();

			if (un_changed_origin != object.origin) {
				tracer.forceUpdate();
			}
			ImGui::Separator();

			Color col = object.color;
			float color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			float unchanged_color[] = { col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f };
			ImGui::AlignTextToFramePadding();
			ImGui::TreeNodeEx("Color", flags, "Color");
			ImGui::NextColumn();

			ImGui::ColorPicker4("Color##lights", color);
			ImGui::NextColumn();

			for (int i = 0; i < 4; i++) {
				if (color[i] != unchanged_color[i]) {
					object.color = Color(color[0] * 255.f, color[1] * 255.f, color[2] * 255.f, color[3] * 255.f);
					tracer.forceUpdate();
					break;
				}
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();

	return false;
}

void runGui(MSG& msg, Raytracer& tracer) {
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		return;
	}

	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("RayTracer"); 

		if (ImGui::CollapsingHeader("Scene settings")) {
			static int width = 500, height = 500;
			ImGui::Text("Canvas size: ");
			ImGui::InputInt("Width", &width);
			ImGui::InputInt("Height", &height);
			tracer.setCanvasSize(Vec2(width, height));
		}
		if (ImGui::CollapsingHeader("Camera settings"))	{
			auto origin = tracer.getCamOrigin();
			auto dir = tracer.getCamDirection();
			ImGui::Text("Camera origin: ");
			ImGui::SameLine();
			if (ImGui::Button("Reset##camorigin"))
				origin = Vec3(0, 0, 0);
			ImGui::SliderFloat("X##camorig", &(origin.x), -10, 10);
			ImGui::SliderFloat("Y##camorig", &(origin.y), -10, 10);
			ImGui::SliderFloat("Z##camorig", &(origin.z), -10, 10);
			ImGui::Text("Camera direction: ");
			ImGui::SameLine();
			if (ImGui::Button("Reset##camdir"))
				dir = Vec3(0, 0, 1);
			ImGui::SliderFloat("X##camdir", &(dir.x), -10, 10);
			ImGui::SliderFloat("Y##camdir", &(dir.y), -10, 10);
			ImGui::SliderFloat("Z##camdir", &(dir.z), -10, 10);
			tracer.setCamOrigin(origin);
			tracer.setCamDirection(dir);
		}
		if (ImGui::CollapsingHeader("Obects")) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			ImGui::Separator();

			auto& objects = tracer.getObjectsData();
			for (int obj_i = 0; obj_i < objects.size(); obj_i++)
				if (ShowPlaceholderObject("Item", obj_i, objects[obj_i]->getType(), tracer))
					break;

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::PopStyleVar();
		}
		if (ImGui::CollapsingHeader("Lights")) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			ImGui::Separator();

			auto& objects = tracer.getLightData();
			for (int obj_i = 0; obj_i < objects.size(); obj_i++)
				if (ShowPlaceholderObject("Item ", obj_i, LIGHT, tracer))
					break;

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::PopStyleVar();
		}
		if (ImGui::Button("DEMO")) {
			tracer.getObjectsData().clear();
			tracer.getLightData().clear();
			tracer.addPlane(Vec3(-1, -1, -1), Vec3(0, 1, 0), Color(255, 255, 255));
			tracer.addSphere(Vec3(0, 0, 3), 1.f, Color(255, 0, 0));
			tracer.addTriangle(Vec3(3, 0, 3), Vec3(0, 0, 3), Vec3(1, 2, 3), Color(120, 200, 20));
			tracer.addLight(Vec3(0, 1, 1), Color(255, 255, 255));
			tracer.addLight(Vec3(5, 0, 1), Color(255, 255, 255));
			tracer.addLight(Vec3(1, 3, 3), Color(255, 255, 255));
		}

		if (ImGui::Button("Add plane")) {
			tracer.addPlane(Vec3(0, 0, 0), Vec3(0, 1, 0), Color(255, 255, 255));
		}

		if (ImGui::Button("Add triangle")) {
			tracer.addTriangle(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Color(255, 255, 255));
		}

		if (ImGui::Button("Add sphere")) {
			tracer.addSphere(Vec3(0, 0, 3), 1.f, Color(255, 255, 255));
		}

		if (ImGui::Button("Add light")) {
			tracer.addLight(Vec3(0, 0, 0), Color(255, 255, 255));
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Begin("Preview", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Image(view, ImVec2(tracer.getCanvasSize().x, tracer.getCanvasSize().y));
	ImGui::End();

	// Rendering
	ImGui::Render();
	g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

	//g_pSwapChain->Present(1, 0); // Present with vsync
	g_pSwapChain->Present(0, 0); // Present without vsync
}

void destroyGui() {
	ImGui_ImplDX10_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D10Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
