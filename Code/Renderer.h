#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_m.h>
#include "Camera.h"
#include "Light.h"

#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void BeginFrame(Camera& camera);

	void DrawPlane();
	void DrawCube();

	void RenderDepthMap(const glm::mat4& lightSpaceMatrix);
	void RenderLighting(const glm::vec3& lightPosition, const glm::mat4& lightSpaceMatrix);
	void RenderSkybox();


	Shader m_baseShader;
	Shader m_depthShader;
	Shader m_pbrShader;
	Shader m_equirectangularToCubemapShader;
	Shader m_irradianceShader;
	Shader m_prefilterShader;
	Shader m_brdfShader;
	Shader m_backgroundShader;

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	unsigned int m_captureFBO;
	unsigned int m_captureRBO;

	unsigned int m_planeVBO;
	unsigned int m_planeVAO;

	unsigned int m_cubeVAO = 0;
	unsigned int m_cubeVBO = 0;

	unsigned int m_irradianceMap;
	unsigned int m_prefilterMap;
	unsigned int m_brdfLUTTexture;
	unsigned int m_envCubemap;


private:
	void SetupDepthMap();
	void SetupPlane();
	void SetupCube();
	void SetupPBR();

	Camera* m_camera = nullptr;
	static Camera s_defaultCamera;

	std::vector<Light> m_lights;
};