#include "PostProcessingController.h"
#include "DSEngineApp.h"
#include "DSSInput.h"
PostProcessingController::PostProcessingController(Object* owner)
	:Component(owner)
{
	currentSkybox = 0;
}

PostProcessingController::~PostProcessingController()
{
}

void PostProcessingController::Start()
{
}

void PostProcessingController::Update(float deltaTime, float totalTime)
{
	int* sourceIndices = nullptr;
	darkCorner->GetSourceIndices(&sourceIndices);

	if (SInput->GetButtonDown("ToggleDarkCorner"))
		darkCorner->parameters.intensity = 1.0f - darkCorner->parameters.intensity;

	if (SInput->GetButtonDown("ToggleIL"))
		ssao->indirectLighting = !ssao->indirectLighting;

	if (SInput->GetButtonDown("TurnOffPP"))
		sourceIndices[0] = 7;

	if (SInput->GetButtonDown("TurnOnBloom"))
		sourceIndices[0] = 6;

	if (SInput->GetButtonDown("TurnOnSSAO"))
		sourceIndices[0] = 3;

	if (SInput->GetButtonDown("ToggleSkybox"))
	{
		switch (currentSkybox)
		{
		case 0:
			App->CurrentActiveScene()->mainCamera->SetSkybox(App->CurrentActiveScene()->device,
				App->CurrentActiveScene()->context, 
				L"Assets/Skybox/1/Environment1HiDef.cubemap.dds", 
				L"Assets/Skybox/1/Environment1Light.cubemap.dds");
			currentSkybox = 1;
			break;
		case 1:
			App->CurrentActiveScene()->mainCamera->SetSkybox(App->CurrentActiveScene()->device,
				App->CurrentActiveScene()->context,
				L"Assets/Skybox/SunnyCubeMap.dds");
			currentSkybox = 2;
			break;
		case 2:
			App->CurrentActiveScene()->mainCamera->SetSkybox(App->CurrentActiveScene()->device,
				App->CurrentActiveScene()->context,
				L"Assets/Skybox/mp_cupertin/mp_cupertin.dds",
				L"Assets/Skybox/mp_cupertin/mp_cupertin_irr.dds");
			currentSkybox = 0;
			break;
		default:
			App->CurrentActiveScene()->mainCamera->SetSkybox(App->CurrentActiveScene()->device,
				App->CurrentActiveScene()->context,
				L"Assets/Skybox/1/Environment1HiDef.cubemap.dds",
				L"Assets/Skybox/1/Environment1Light.cubemap.dds");
			LOG_ERROR << "ERROR SKYBOX INDEX";
			break;
		}
	}
}
