#pragma once
#include "pch.h"

#include <glm\ext\matrix_float4x4.hpp>
#include <vector>
#include <LightSourceComponent.h>

using glm::Matrix4x4;

class Shader;
class LightingController final
{
private:
	friend class EngineEditor;

	LightingController() = delete;

	static unsigned int depthMapFBO;
	static unsigned int SHADOW_WIDTH;
	static unsigned int SHADOW_HEIGHT;
	static unsigned int depthMap;

	static void RecalculateDepthMap();
public:
	static void Initialize();
	static void SetShadowMapScale(unsigned int width, unsigned int height);
	static Vector2 GetShadowMapScale();
	static unsigned int GetShadowMapWidth();
	static unsigned int GetShadowMapHeight();
	static void PrepareDepthMap(/*std::vector<ModelRendererComponent*> meshesWithShadows*/);
	static void PrepareDepthMap(LightSourceComponent* directionalLight);
	static GLuint GetDepthMapID();
};