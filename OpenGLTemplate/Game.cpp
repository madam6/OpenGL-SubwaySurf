/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Renderer.h"
#include "Camera.h"
#include "CatmullRom.h"
#include "Crystal.h"
#include "Skybox.h"
#include "EntityParser.h"
#include "EventSystem.h"
#include "ColliderComponent.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "PlayerTrackMovementComponent.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CurrencyManagerComponent.h"
#include "FrameBufferObject.h"
#include <iostream>

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_DepthBuffer = nullptr;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.f;
	m_cameraSpeed = 0.1f;
}


// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pHeartIcon;
	delete m_pSpeedLinesOverlay;
	m_DepthBuffer->Release();
	delete m_DepthBuffer;

	for (auto& [name, pointer] : m_ShaderPrograms)
	{
		delete pointer;
	}
	m_ShaderPrograms.clear();

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pAudio = new CAudio;
	m_Renderer = std::make_unique<Renderer>();
	m_DepthBuffer = new CFrameBufferObject;
	float depthBufferScale = 8.f;
	m_DepthBuffer->Create(depthBufferScale * 4096, depthBufferScale * 4096);
	m_Renderer->SetDepthBuffer(m_DepthBuffer);

	RECT dimensions = m_gameWindow.GetDimensions();
	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);

	std::array<std::string, 3> mountainTextures{ "mud.jpg", "grass.jpg", "top.jpg"};

	m_pSkybox->Create(2500.0f);
	m_pPlanarTerrain->CreatePerlinTerrain("resources\\textures\\", mountainTextures, 2000.0f, 2000.0f, 50.0f);
	m_pFtFont->LoadSystemFont("arial.ttf", 32);

	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");
	m_pAudio->PlayMusicStream();

	m_pSphere = new CSphere();
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);

	m_pCrystal = std::make_unique<CCrystal>();
	m_pCrystal->Create("resources\\textures\\", "crystalTexture.jpg");

	m_pHeartIcon = new CPlane();
	m_pHeartIcon->Create("resources\\textures\\", "heart.png", 40.0f, 40.0f, 1.0f);

	m_pSpeedLinesOverlay = new CPlane();
	m_pSpeedLinesOverlay->Create("resources\\textures\\", "SpeedLines.png", 1.0f, 1.0f, 1.0f);

	InitShaders();

	std::vector<std::string> entityLines = ReadEntityLines("resources\\entities.cfg");
	std::vector<std::string> currentEntityLines;
	std::string currentEntityName = "";

	for (const auto& line : entityLines)
	{
		if (line.find("EntityName:") == 0)
		{
			if (!currentEntityLines.empty())
			{
				m_EntityTemplates[currentEntityName] = currentEntityLines;
				m_entities.push_back(EntityParser::Create(currentEntityLines));
				currentEntityLines.clear();
			}
			currentEntityName = line.substr(11);
		}
		currentEntityLines.push_back(line);
	}

	if (!currentEntityLines.empty())
	{
		m_EntityTemplates[currentEntityName] = currentEntityLines;
		m_entities.push_back(EntityParser::Create(currentEntityLines));
	}

	float noiseScale = 0.01f;
	float mountainHeight = 50.0f;

	// Trees, displaced using perlin noise similar to what happens in vertex shader
	for (int i = 0; i < 1000; i++)
	{
		float randX = (float(rand() % 20000) / 10.0f) - 1000.0f;
		float randZ = (float(rand() % 20000) / 10.0f) - 1000.0f;

		glm::vec2 noiseCoords = glm::vec2(randX, randZ) * noiseScale;

		auto newTree = SpawnEntityFromTemplate("Tree");
		if (newTree)
		{
			newTree->SetName("Tree_" + std::to_string(i));
			newTree->Init();

			if (auto mv = newTree->FindComponent<ModelViewComponent>())
			{
				mv->SetPosition(glm::vec3(randX, 0.f, randZ));

				float randScale = 0.01f + (float(rand() % 50) / 1000.0f);
				mv->SetScale(glm::vec3(randScale));

				float randRot = (float)(rand() % 360);
				glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(randRot), glm::vec3(0.0f, 1.0f, 0.0f));
				mv->SetOrientation(rotMatrix);
			}
		}
	}

	for (size_t i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->Init();
	}
}

void Game::Render(bool depthPass)
{
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1);
	glm::vec4 sunPosition = glm::vec4(-1000, 900, -1000, 1);
	glEnable(GL_DEPTH_TEST);
	if (depthPass)
	{
		m_DepthBuffer->Bind(true);
		glCullFace(GL_FRONT);
	}
	else
	{
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		RECT dimensions = m_gameWindow.GetDimensions();
		glViewport(0, 0, dimensions.right - dimensions.left, dimensions.bottom - dimensions.top);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	float projectionScale = 1.5f;

	if (depthPass)
	{
		projMatrix = glm::ortho(projectionScale * -1000.0f, projectionScale * 1000.0f, projectionScale * -1000.0f, projectionScale *  1000.0f, 1.0f, 2500.0f);
		viewMatrix = glm::lookAt(glm::vec3(sunPosition), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	else
	{
		projMatrix = *m_pCamera->GetPerspectiveProjectionMatrix();
		viewMatrix = m_pCamera->GetViewMatrix();
	}

	CShaderProgram* pMainProgram = GetShader("MainShader");
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("matrices.projMatrix", projMatrix);
	pMainProgram->SetUniform("bIsDepthPass", depthPass ? 1 : 0);
	pMainProgram->SetUniform("lightSpaceMatrix", glm::ortho(projectionScale * -1000.0f, projectionScale * 1000.0f, projectionScale * -1000.0f, projectionScale *  1000.0f, 1.0f, 2500.0f) * glm::lookAt(glm::vec3(sunPosition), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

	if (!depthPass)
	{
		int cubeMapTextureUnit = 10;
		pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

		int shadowMapTextureUnit = 11;
		m_DepthBuffer->BindDepth(shadowMapTextureUnit);
		pMainProgram->SetUniform("shadowMap", shadowMapTextureUnit);

		pMainProgram->SetUniform("numLights", 2);
		pMainProgram->SetUniform("lights[0].position", viewMatrix * lightPosition1);
		pMainProgram->SetUniform("lights[0].La", glm::vec3(0.7f));
		pMainProgram->SetUniform("lights[0].Ld", glm::vec3(0.5f));
		pMainProgram->SetUniform("lights[0].Ls", glm::vec3(0.5f));

		pMainProgram->SetUniform("lights[1].position", viewMatrix * sunPosition);
		pMainProgram->SetUniform("lights[1].La", glm::vec3(0.0f, 0.0f, 0.0f));
		pMainProgram->SetUniform("lights[1].Ld", glm::vec3(0.6f, 0.6f, 0.2f));
		pMainProgram->SetUniform("lights[1].Ls", glm::vec3(0.8f, 0.8f, 0.2f));
	}

	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));
	pMainProgram->SetUniform("material1.shininess", 15.0f);

	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();
	modelViewMatrixStack.ApplyMatrix(viewMatrix);

	if (!depthPass)
	{
		modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(10);
		pMainProgram->SetUniform("renderSkybox", false);
		modelViewMatrixStack.Pop();
	}

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("sampler1", 1);
	pMainProgram->SetUniform("sampler2", 2);
	pMainProgram->SetUniform("isTerrain", true);
	pMainProgram->SetUniform("fMinHeight", -10.f);
	pMainProgram->SetUniform("fMaxHeight", 17.f);
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	m_pPlanarTerrain->Render();
	pMainProgram->SetUniform("isTerrain", false);
	modelViewMatrixStack.Pop();

	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));

	if (!depthPass)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(sunPosition));
		modelViewMatrixStack.Scale(15.0f);

		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

		pMainProgram->SetUniform("bUseTexture", false);

		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.5f, 1.5f, 0.0f));
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));

		m_pSphere->Render();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);

		modelViewMatrixStack.Scale(1.4f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());

		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.6f, 0.2f, 0.0f));
		m_pSphere->Render();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		pMainProgram->SetUniform("bUseTexture", true);

		modelViewMatrixStack.Pop();
	}

	for (auto& entityPtr : m_entities)
	{
		entityPtr->AddRenderData(m_Renderer->m_RenderQueue);
	}

	FrameData frameData;

	frameData.projMatrix = projMatrix;
	frameData.viewMatrix = viewMatrix;
	frameData.cameraPosition = m_pCamera->GetPosition();

	FrameData::Light light;
	light.position = lightPosition1;
	light.La = glm::vec3(0.9f);
	light.Ld = glm::vec3(0.5f);
	light.Ls = glm::vec3(0.5f);
	frameData.lights.push_back(light);

	FrameData::Light sunLight;
	sunLight.position = sunPosition;
	sunLight.La = glm::vec3(0.0f, 0.0f, 0.0f);
	sunLight.Ld = glm::vec3(0.6f, 0.6f, 0.2f);
	sunLight.Ls = glm::vec3(0.8f, 0.8f, 0.2f);
	frameData.lights.push_back(sunLight);

	static float s_totalTime = 0.0f;
	s_totalTime += (float)(m_dt / 1000.0);
	frameData.time = s_totalTime;

	frameData.isDepthPass = depthPass;
	frameData.lightSpaceMatrix = glm::ortho(projectionScale * -1000.0f, projectionScale * 1000.0f, projectionScale * -1000.0f, projectionScale *  1000.0f, 1.0f, 2500.0f) * glm::lookAt(glm::vec3(sunPosition), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	m_Renderer->Render(frameData);

	if (!depthPass)
	{
		DisplayFrameRate();
		DisplayHUD();
		SwapBuffers(m_gameWindow.Hdc());
	}
}

void Game::InitShaders()
{
	// Load shaders
	std::vector<CShader> shShaders;
	std::vector<std::string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("crystalShader.vert");
	sShaderFileNames.push_back("crystalShader.frag");
	sShaderFileNames.push_back("mc.vert");
	sShaderFileNames.push_back("mc.frag");
	sShaderFileNames.push_back("toonShader.vert");
	sShaderFileNames.push_back("toonShader.frag");
	sShaderFileNames.push_back("outlineShader.vert");
	sShaderFileNames.push_back("outlineShader.frag");
	sShaderFileNames.push_back("uiShader.vert");
	sShaderFileNames.push_back("uiShader.frag");
	sShaderFileNames.push_back("instancedTree.vert");
	sShaderFileNames.push_back("toonMesh.vert");
	sShaderFileNames.push_back("toonMesh.frag");
	sShaderFileNames.push_back("outlineMesh.vert");

	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		std::string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram* pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_ShaderPrograms["MainShader"] = pMainProgram;

	// Create a shader program for fonts
	CShaderProgram* pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_ShaderPrograms["FontShader"] = pFontProgram;
	m_pFtFont->SetShaderProgram(pFontProgram);

	CShaderProgram* pCrystalProgram = new CShaderProgram;
	pCrystalProgram->CreateProgram();
	pCrystalProgram->AddShaderToProgram(&shShaders[4]);
	pCrystalProgram->AddShaderToProgram(&shShaders[5]);
	pCrystalProgram->LinkProgram();
	m_ShaderPrograms["CrystalShader"] = pCrystalProgram;

	CShaderProgram* pMCProgram = new CShaderProgram;
	pMCProgram->CreateProgram();
	pMCProgram->AddShaderToProgram(&shShaders[6]);
	pMCProgram->AddShaderToProgram(&shShaders[7]);
	pMCProgram->LinkProgram();
	m_ShaderPrograms["mc"] = pMCProgram;

	CShaderProgram* pToonProgram = new CShaderProgram;
	pToonProgram->CreateProgram();
	pToonProgram->AddShaderToProgram(&shShaders[8]);
	pToonProgram->AddShaderToProgram(&shShaders[9]);
	pToonProgram->LinkProgram();
	m_ShaderPrograms["toonShader"] = pToonProgram;

	CShaderProgram* pOutlineProgram = new CShaderProgram;
	pOutlineProgram->CreateProgram();
	pOutlineProgram->AddShaderToProgram(&shShaders[10]);
	pOutlineProgram->AddShaderToProgram(&shShaders[11]);
	pOutlineProgram->LinkProgram();
	m_ShaderPrograms["outlineShader"] = pOutlineProgram;

	CShaderProgram* pUiProgram = new CShaderProgram;
	pUiProgram->CreateProgram();
	pUiProgram->AddShaderToProgram(&shShaders[12]);
	pUiProgram->AddShaderToProgram(&shShaders[13]);
	pUiProgram->LinkProgram();
	m_ShaderPrograms["uiShader"] = pUiProgram;

	CShaderProgram* pTreeProgram = new CShaderProgram;
	pTreeProgram->CreateProgram();
	pTreeProgram->AddShaderToProgram(&shShaders[14]);
	pTreeProgram->AddShaderToProgram(&shShaders[1]);
	pTreeProgram->LinkProgram();
	pTreeProgram->UseProgram();
	pTreeProgram->SetUniform("CubeMapTex", 10);
	m_ShaderPrograms["InstancedTreeShader"] = pTreeProgram;

	CShaderProgram* pToonMeshProgram = new CShaderProgram;
	pToonMeshProgram->CreateProgram();
	pToonMeshProgram->AddShaderToProgram(&shShaders[15]);
	pToonMeshProgram->AddShaderToProgram(&shShaders[16]);
	pToonMeshProgram->LinkProgram();
	m_ShaderPrograms["toonMesh"] = pToonMeshProgram;

	CShaderProgram* pOutlineMeshProgram = new CShaderProgram;
	pOutlineMeshProgram->CreateProgram();
	pOutlineMeshProgram->AddShaderToProgram(&shShaders[17]);
	pOutlineMeshProgram->AddShaderToProgram(&shShaders[11]);
	pOutlineMeshProgram->LinkProgram();
	m_ShaderPrograms["outlineMeshShader"] = pOutlineMeshProgram;
}

std::shared_ptr<Entity> Game::FetchEntityByName(const std::string& name)
{
	for (const auto& entity : m_entities)
	{
		if (entity->GetName() == name)
		{
			return entity;
		}
	}
	return nullptr;
}

std::vector<std::string> Game::ReadEntityLines(const std::string& filename)
{
	std::vector<std::string> lines;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Failed to open " << filename << std::endl;
		return lines;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') continue;
		lines.push_back(line);
	}
	return lines;
}

CShaderProgram* Game::GetShader(const std::string& name) const
{
	auto it = m_ShaderPrograms.find(name);
	if (it != m_ShaderPrograms.end())
	{
		return it->second;
	}

	LOG_ERROR("Shader not found: %s", name.c_str());
	return nullptr;
}

void Game::Update() 
{
	int health = 3;
	if (auto mc = FetchEntityByName("MC"))
	{
		if (auto currencyMgr = mc->FindComponent<CurrencyManagerComponent>())
		{
			health = currencyMgr->GetHealth();
		}
	}

	if (health <= 0) return;

	for (size_t i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->Update(m_dt);
	}

	float targetFOV = 60.0f;

	if (auto mc = FetchEntityByName("MC"))
	{
		if (std::shared_ptr<PlayerTrackMovementComponent> playerMov = mc->FindComponent<PlayerTrackMovementComponent>())
		{
			if (playerMov->IsSpeedBoostActive())
			{
				targetFOV = 75.0f;
			}
		}
	}

	static float currentFOV = 45.0f;
	currentFOV += (targetFOV - currentFOV) * 0.1f;

	RECT dimensions = m_gameWindow.GetDimensions();
	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	m_pCamera->SetPerspectiveProjectionMatrix(glm::radians(currentFOV), (float)width / (float)height, 0.5f, 5000.0f);

	std::vector<ColliderComponent*> colliders;
	for (size_t i = 0; i < m_entities.size(); i++)
	{
		auto collider = m_entities[i]->FindComponent<ColliderComponent>();
		if (collider && collider->IsActive())
		{
			colliders.push_back(collider.get());
		}
	}

	for (size_t i = 0; i < colliders.size(); i++)
	{
		for (size_t j = i + 1; j < colliders.size(); j++)
		{
			glm::vec3 minA = colliders[i]->GetMinBounds();
			glm::vec3 maxA = colliders[i]->GetMaxBounds();

			glm::vec3 minB = colliders[j]->GetMinBounds();
			glm::vec3 maxB = colliders[j]->GetMaxBounds();

			bool collisionX = maxA.x >= minB.x && minA.x <= maxB.x;
			bool collisionY = maxA.y >= minB.y && minA.y <= maxB.y;
			bool collisionZ = maxA.z >= minB.z && minA.z <= maxB.z;

			if (collisionX && collisionY && collisionZ)
			{
				CollisionPayload payload;
				payload.entityA = colliders[i]->GetOwner();
				payload.entityB = colliders[j]->GetOwner();

				EventData data;
				data.payload = payload;

				EventSystem::Instance().BroadcastEvent("OnCollision", data);
			}
		}
	}

	m_pCamera->Update(m_dt);
	m_pAudio->Update();
}



void Game::DisplayFrameRate()
{
	CShaderProgram* fontProgram = GetShader("FontShader");

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

void Game::DisplayHUD()
{
	RECT dimensions = m_gameWindow.GetDimensions();
	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	glDisable(GL_DEPTH_TEST);

	int score = 0;
	int recordScore = 0;
	int health = 0;
	if (auto mc = FetchEntityByName("MC")) 
	{
		if (auto currencyMgr = mc->FindComponent<CurrencyManagerComponent>()) 
		{
			score = currencyMgr->GetScore();
			recordScore = currencyMgr->GetRecordScore();
			health = currencyMgr->GetHealth();
		}
	}

	CShaderProgram* pCrystalProgram = GetShader("CrystalShader");
	pCrystalProgram->UseProgram();

	glm::mat4 hudOrtho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);
	pCrystalProgram->SetUniform("matrices.projMatrix", hudOrtho);
	pCrystalProgram->SetUniform("matrices.viewMatrix", glm::mat4(1.0f));

	static float s_hudTime = 0.0f;
	s_hudTime += (float)(m_dt / 1000.0);
	pCrystalProgram->SetUniform("uTime", s_hudTime);

	pCrystalProgram->SetUniform("numLights", 1);
	pCrystalProgram->SetUniform("lights[0].position", glm::vec4(0, 0, 1, 0));

	pCrystalProgram->SetUniform("lights[0].La", glm::vec3(1.0f));
	pCrystalProgram->SetUniform("material1.Ma", glm::vec3(1.0f));

	pCrystalProgram->SetUniform("lights[0].Ld", glm::vec3(0.0f));
	pCrystalProgram->SetUniform("material1.Md", glm::vec3(0.0f));

	pCrystalProgram->SetUniform("lights[0].Ls", glm::vec3(0.0f));
	pCrystalProgram->SetUniform("material1.Ms", glm::vec3(0.0f));
	pCrystalProgram->SetUniform("material1.shininess", 1.0f);

	pCrystalProgram->SetUniform("bUseTexture", 1);
	pCrystalProgram->SetUniform("sampler0", 0);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(width - 140.0f, height - 70.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(15.0f));

	std::vector<glm::mat4> hudInstance = { modelMatrix };
	m_pCrystal->RenderInstanced(hudInstance);

	CShaderProgram* fontProgram = GetShader("FontShader");
	fontProgram->UseProgram();
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1.0f));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	m_pFtFont->Render(width - 100, height - 70, 32, "x %d", score);
	m_pFtFont->Render(width - 260, height - 120, 32, "Lap Record: %d", recordScore);

	CShaderProgram* pUiProgram = GetShader("uiShader");
	pUiProgram->UseProgram();

	pUiProgram->SetUniform("projMatrix", hudOrtho);
	pUiProgram->SetUniform("sampler0", 0);

	bool isBoosting = false;
	if (auto mc = FetchEntityByName("MC"))
	{
		if (auto playerMov = mc->FindComponent<PlayerTrackMovementComponent>())
		{
			isBoosting = playerMov->IsSpeedBoostActive();
		}
	}

	if (isBoosting)
	{
		pUiProgram->UseProgram();

		glm::mat4 overlayModel = glm::translate(glm::mat4(1.0f), glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
		overlayModel = glm::scale(overlayModel, glm::vec3((float)width, (float)height, 1.0f));

		overlayModel = glm::rotate(overlayModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		pUiProgram->SetUniform("modelMatrix", overlayModel);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_pSpeedLinesOverlay->Render();

		glDisable(GL_BLEND);
	}

	for (int i = 0; i < health; i++)
	{
		float xPos = (width - 75.0f) - (i * 45.0f);

		float yPos = 50.0f;

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));

		modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		pUiProgram->SetUniform("modelMatrix", modelMatrix);
		m_pHeartIcon->Render();
	}

	if (health <= 0)
	{
		CShaderProgram* fontProgram = GetShader("FontShader");
		fontProgram->UseProgram();
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1.0f));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());

		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		std::string deathText = "YOU ARE DEAD";
		int textWidth = m_pFtFont->GetTextWidth(deathText, 80);
		m_pFtFont->Render((width / 2) - (textWidth / 2), height / 2, 80, deathText.c_str());

		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		std::string subText = "Press ESC to exit";
		int subWidth = m_pFtFont->GetTextWidth(subText, 32);
		m_pFtFont->Render((width / 2) - (subWidth / 2), (height / 2) - 60, 32, subText.c_str());
	}
}

std::shared_ptr<Entity> Game::SpawnEntityFromTemplate(const std::string& templateName)
{
	auto it = m_EntityTemplates.find(templateName);
	if (it != m_EntityTemplates.end())
	{
		auto newEntity = EntityParser::Create(it->second);
		m_entities.push_back(newEntity);
		return newEntity;
	}
	return nullptr;
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render(true);
	Render(false);
	m_dt = m_pHighResolutionTimer->Elapsed();
}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}
	m_entities.clear();
	m_EntityTemplates.clear();

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;
	EventData eventData;
	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		if ((l_param & (1 << 30)) == 0)
		{
			eventData.payload = static_cast<int>(w_param);
			EventSystem::Instance().BroadcastEvent("KeyDown", eventData);

			switch (w_param) 
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case '1':
				//m_pAudio->PlayEventSound();
				break;
			case VK_F1:
				//m_pAudio->PlayEventSound();
				break;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return int(game.Execute());
}
