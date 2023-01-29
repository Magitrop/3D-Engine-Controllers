#include <windows.h>
#include <fstream>
#include <filesystem>

#ifdef __gl_h_
#undef __gl_h_
#endif // __gl_h_
#include <glad/glad.h>

#include <thread>
#include <chrono>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine.h>
#include "Initializer.h"
#include "EventsController.h"
#include "LightingController.h"
#include "RenderingController.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>
using Json = nlohmann::json;

void LoadShaders()
{
	//ResourceManager->UploadShader(Shader("source\\TextVertexShader.vertexshader", "source\\TextFragmentShader.fragmentshader"), "Text");
	ResourceManager->UploadShader(new Shader("source\\simple-vert.glsl", "source\\simple-frag.glsl"), "Simple");
	ResourceManager->UploadShader(new Shader("source\\depth-vert.glsl", "source\\depth-frag.glsl"), "Depth");
	ResourceManager->UploadShader(new Shader("source\\shadows-vert.glsl", "source\\shadows-frag.glsl"), "Shadows");
	ResourceManager->UploadShader(new Shader("source\\multishadows-vert.glsl", "source\\multishadows-frag.glsl"), "Multishadows");
	ResourceManager->UploadShader(new Shader("source\\standard-vert.glsl", "source\\standard-frag.glsl"), "Standard");
	ResourceManager->UploadShader(new Shader("source\\grid-vert.glsl", "source\\grid-frag.glsl"), "Grid");
	ResourceManager->UploadShader(new Shader("source\\axis-vert.glsl", "source\\axis-frag.glsl"), "Axis");
	ResourceManager->UploadShader(new Shader("source\\texture-vert.glsl", "source\\texture-frag.glsl"), "Texture");
}
void LoadModels()
{
	ResourceManager->UploadModel(new Model("source\\castle.fbx"), "Castle");
}

std::vector<std::string> customComponents;
std::map<std::string, const void*> loadedCustomComponents;

int main()
{
	EventsController->SetPlayMode(true);
	InitializationHandler->Init({ 1600, 800 });
	LoadShaders();
	//LoadModels();

	std::ifstream projectFile("source\\MainScene.scene");
	std::ifstream configFile("source\\components.config");
	std::ifstream resourcesStream("source\\resources.config");
	std::stringstream saveBuffer, componentsBuffer, resourcesBuffer;
	saveBuffer << projectFile.rdbuf();
	componentsBuffer << configFile.rdbuf();
	resourcesBuffer << resourcesStream.rdbuf();

	try
	{
		Json sceneFile = Json::parse(saveBuffer.str());
		Json componentsFile = Json::parse(componentsBuffer.str());
		Json resourcesFile = Json::parse(resourcesBuffer.str());

		Json components = componentsFile["Components"];
		if (components.is_array())
			for (auto& o : components.get<Json::array_t>())
				customComponents.push_back(o["name"]);

		std::vector<std::thread> loadingModelThreads;
		std::vector<std::tuple<std::string, Model*, GLFWwindow*>> pendingLoadedModels;
		Json models = resourcesFile["Models"];
		if (models.is_array())
			for (auto& m : models.get<Json::array_t>())
			{
				std::string name = m["name"];
				std::string path = m["path"];
				glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
				auto window = glfwCreateWindow(1, 1, "Loading", nullptr, InitializationHandler->GetWindow());
				glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
				loadingModelThreads.push_back(
					std::thread(
						[=, &pendingLoadedModels]()
						{
							glfwMakeContextCurrent(window);
							Model* model = new Model(path);
							pendingLoadedModels.push_back(std::make_tuple(name, model, window));
							/*for (auto& mesh : model->meshes)
								mesh->SetupMesh();
							ResourceManager->UploadModel(model, name);
							glfwDestroyWindow(window);*/
						})
				);
			}
		for (auto& t : loadingModelThreads)
			t.join();
		auto it = pendingLoadedModels.begin();
		while (it != pendingLoadedModels.end())
		{
			for (auto& mesh : std::get<1>(*it)->meshes)
				mesh->SetupMesh();
			ResourceManager->UploadModel(std::get<1>(*it), std::get<0>(*it));
			glfwDestroyWindow(std::get<2>(*it));
			it = pendingLoadedModels.erase(it);
		}

		Json scene = sceneFile["Scene"];
		if (scene.is_array())
			for (auto& o : scene.get<Json::array_t>())
			{
				GameObject* obj = ObjectsManager->Instantiate();
				obj->name = o.begin().key();
				for (auto& comp : o.begin().value().get<Json::array_t>())
				{
					std::string componentName = comp.begin().key();
					if (componentName == "Transform")
					{
						obj->AddComponent<TransformComponent>()->Deserialize(comp);
					}
					else if (componentName == "Camera")
					{
						obj->AddComponent<CameraComponent>()->Deserialize(comp);
					}
					else if (componentName == "Model Renderer")
					{
						obj->AddComponent<ModelRendererComponent>()->Deserialize(comp);
					}
					else if (componentName == "Line Renderer")
					{
						obj->AddComponent<LineRendererComponent>()->Deserialize(comp);
					}
					else if (componentName == "Light Source")
					{
						obj->AddComponent<LightSourceComponent>()->Deserialize(comp);
					}
					else
					{
						if (loadedCustomComponents.find(componentName) == loadedCustomComponents.end())
						{
							std::string componentPath = componentName + ".dll";
							HINSTANCE hDll = LoadLibraryA(componentPath.c_str());
							if (hDll)
							{
								loadedCustomComponents[componentName] = hDll;
								reinterpret_cast<void(*)(SingletonManager*)>(GetProcAddress(hDll, "_set_singleton_manager"))(SingletonManager::Instance());
								reinterpret_cast<void(*)(GameObject*, const Json&)>(GetProcAddress(hDll, "_add_comp"))(obj, comp);
							}
							else
							{
								std::cout << "Unable to load component " << componentName << std::endl;
							}
						}
						else
						{
							reinterpret_cast<void(*)(GameObject*, const Json&)>
								(GetProcAddress((HMODULE)loadedCustomComponents[componentName], "_add_comp"))
								(obj, comp);
						}
					}
				}
			}
	}
	catch (Json::parse_error) 
	{
		std::cout << "Error with .scene or .config file, the game cannot be loaded." << std::endl;
		return 1;
	}

	GLFWwindow* window = InitializationHandler->GetWindow();

	EventsController->SetAsMainCamera(ObjectsManager->FindObjectOfType<CameraComponent>());
	Lightings->lightPosition = Vector3(-1, 1, 1);
	while (!glfwWindowShouldClose(window))
	{
		auto t_start = std::chrono::high_resolution_clock::now();
		// ^^^ starting elapsed time calculation

		LightingController::PrepareDepthMap();

		glViewport(0, 0, Screen->GetWindowResolution().x, Screen->GetWindowResolution().y);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, LightingController::GetDepthMapID());
		ResourceManager->GetShader("Shadows")->SetInt("shadowMap", 0);

		for (int i = -1; i <= 1; i++)
		{
			RenderingController::Render(i);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		EventsController->Update();
		glfwPollEvents();
		glfwSwapBuffers(window);

		// vvv starting elapsed time calculation
		auto t_end = std::chrono::high_resolution_clock::now();
		TimeManager->deltaTime = std::chrono::duration<float, std::milli>(t_end - t_start).count();
		TimeManager->time += TimeManager->deltaTime;
	}

	InitializationHandler->Quit();
	return 0;
}