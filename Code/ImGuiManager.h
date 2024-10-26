#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "Application.h"

class Camera;
class Car;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

	void Render(Camera& camera, Car& car);
	void Begin();
	void End();
};
