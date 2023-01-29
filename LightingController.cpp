#pragma once
#include "pch.h"
#ifdef __gl_h_
#undef __gl_h_
#endif // __gl_h_
#include <glad/glad.h>
#include <Lightings.h>
#include <Screen.h>
#include <ResourceManager.h>
#include <ObjectsManager.h>
#include <RendererComponentBase.h>

#include "LightingController.h"
#include <glm\ext\matrix_transform.hpp>
#include <glm\ext\matrix_clip_space.hpp>
#include "Shader.h"

unsigned int LightingController::depthMapFBO;
unsigned int LightingController::SHADOW_WIDTH;
unsigned int LightingController::SHADOW_HEIGHT;
unsigned int LightingController::depthMap;

void LightingController::RecalculateDepthMap()
{
	float near_plane = 0.1f, far_plane = 100.f;
	//lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
	Lightings->lightProjection = glm::perspective(glm::radians(90.0f), Screen->GetAspectRatio(), near_plane, far_plane);
	/*lightView =
		glm::rotate(Matrix4x4(1), glm::radians(60.f), Vectors::right) *
		glm::rotate(Matrix4x4(1), glm::radians(lightRot.y), Vectors::up) *
		glm::translate(Matrix4x4(1), -lightPos);*/
	Lightings->lightView =
		glm::lookAt(
			Lightings->lightPosition,
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3(0.0f, 1.0f, 0.0f));
	Lightings->lightSpaceMatrix = Lightings->lightProjection * Lightings->lightView;
}

void LightingController::Initialize()
{
	SetShadowMapScale(Screen->GetWindowResolution().x * 4, Screen->GetWindowResolution().y * 4);

	if (glIsFramebuffer(depthMapFBO))
		glDeleteFramebuffers(1, &depthMapFBO);
	glGenFramebuffers(1, &depthMapFBO);

	if (glIsTexture(depthMap))
		glDeleteTextures(1, &depthMap);
	glGenTextures(1, &depthMap);

	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightingController::SetShadowMapScale(unsigned int width, unsigned int height)
{
	Lightings->SHADOW_WIDTH = SHADOW_WIDTH = width;
	Lightings->SHADOW_HEIGHT = SHADOW_HEIGHT = height;
	RecalculateDepthMap();
}

Vector2 LightingController::GetShadowMapScale()
{
	return Vector2(SHADOW_WIDTH, SHADOW_HEIGHT);
}

unsigned int LightingController::GetShadowMapWidth()
{
	return SHADOW_WIDTH;
}

unsigned int LightingController::GetShadowMapHeight()
{
	return SHADOW_HEIGHT;
}

void LightingController::PrepareDepthMap()
{
	RecalculateDepthMap();

	auto depthShader = *ResourceManager->GetShader("Depth");
	depthShader.Use();
	depthShader.setMat4("lightSpaceMatrix", Lightings->GetLightSpaceMatrix());

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GetDepthMapID());
	for (auto& it : ObjectsManager->renderQueue)
	{
		depthShader.setMat4("model", it->gameObject->transform->GetModelMatrix());
		it->RenderDepth();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void LightingController::PrepareDepthMap(LightSourceComponent* directionalLight)
{
	if (directionalLight->type != LightSourceType::Directional)
		return;

	directionalLight->RecalculateDepthMap();

	auto depthShader = ResourceManager->GetShader("Depth");
	depthShader->Use();
	depthShader->setMat4("lightSpaceMatrix", directionalLight->GetLightSpaceMatrix());

	glViewport(0, 0, Lightings->SHADOW_WIDTH, Lightings->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, directionalLight->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, directionalLight->GetDepthMapID());
	for (auto& it : ObjectsManager->renderQueue)
	{
		depthShader->setMat4("model", it->gameObject->transform->GetModelMatrix());
		it->RenderDepth();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint LightingController::GetDepthMapID()
{
	return depthMap;
}