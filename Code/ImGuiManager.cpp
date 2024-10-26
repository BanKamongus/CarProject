#include "ImGuiManager.h"
#include "Car.h"
#include "Camera.h"

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImFontConfig fontConfig;
	fontConfig.SizePixels = 16.0f;
	io.Fonts->AddFontDefault(&fontConfig);

	ImGui::StyleColorsDark();
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow());

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

}

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

glm::vec3 InputVec3(const std::string& text, const glm::vec3 & v)
{
	float values[3]{ v.x, v.y, v.z };
	ImGui::InputFloat3(text.c_str(), values);

	return { values[0], values[1], values[2] };
}

void ImGuiManager::Render(Camera& camera, Car& car)
{
	Begin();

	ImGui::Begin("Main");

	glm::vec3 tempPos = InputVec3("Camera Pos", camera.GetPosition());
	camera.SetPosition(tempPos);

	glm::vec3 carPos = InputVec3("Car Pos", car.GetPosition());
	car.SetPosition(carPos);

	glm::vec3 carScale = InputVec3("Car Scale", car.GetScale());
	car.SetScale(carScale);


	ImGui::End();

	End();
}

void ImGuiManager::End()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}