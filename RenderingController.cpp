#include "pch.h"
#include "RenderingController.h"
#include <RendererComponentBase.h>
#include <ObjectsManager.h>
#include <EventSystem.h>

void RenderingController::Render(int renderQueueIndex)
{
	if (!EventSystem->GetMainCamera())
		return;

	for (auto& obj : ObjectsManager->renderQueue)
		if (obj->gameObject->isActive && obj->renderQueueIndex == renderQueueIndex)
		{
			obj->OnPreRender();
			obj->Render();
			obj->OnPostRender();
		}
}
