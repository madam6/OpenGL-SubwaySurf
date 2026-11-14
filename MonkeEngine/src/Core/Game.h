#pragma once

#include "Core/Common.h"
#include "Core/GameWindow.h"
#include "Geometry/Sun.h"

class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class COpenAssetImportMesh;
class CAudio;
class CCrystal;
class CCatmullRom;
class Entity;
class Renderer;
class CFrameBufferObject;
struct Renderable;
struct RenderData
{
	glm::mat4 modelMatrix;
	glm::mat3 normalMatrix;

	std::shared_ptr<Renderable> mesh = nullptr;

	glm::vec3 Ma{ 1.0f };
	glm::vec3 Md{ 1.0f };
	glm::vec3 Ms{ 0.0f };
	float shininess = 1.0f;

	GLuint diffuseTexture = 0;
	GLuint cubeMapTexture = 0;
	bool useTexture = false;
	bool isInstanced = false;
	bool useCubeMap = false;

	bool isRecovering = false;
	std::vector<glm::mat4> instanceMatrices;
	bool isOutline = false;

	CShaderProgram* shader = nullptr;

	bool renderSkybox = false;
};

struct FrameData
{
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::vec3 cameraPosition;
	float time;

	struct Light
	{
		glm::vec4 position;
		glm::vec3 La, Ld, Ls;
	};

	std::vector<Light> lights;
	glm::mat4 lightSpaceMatrix;
	bool isDepthPass{ false };
};

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render(bool depthPass);

	// Configure all the shaders and record them in the m_ShaderPrograms map
	void InitShaders();

	std::vector<std::string> ReadEntityLines(const std::string& filename);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	CPlane *m_pPlanarTerrain;
	CPlane* m_pHeartIcon;
	CPlane* m_pSpeedLinesOverlay;
	CFrameBufferObject* m_DepthBuffer;
	CFreeTypeFont *m_pFtFont;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;

	std::unique_ptr<CCrystal> m_pCrystal;
	std::unique_ptr<CSun> m_pSun;
	std::unique_ptr<Renderer> m_Renderer;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;

	std::vector<std::shared_ptr<Entity>> m_entities;

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	// Get any entity in the world by name
	std::shared_ptr<Entity> FetchEntityByName(const std::string& name);
	// Allows to spawn multiple entities defined from a single entity template in entities.cfg
	std::shared_ptr<Entity> SpawnEntityFromTemplate(const std::string& templateName);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
	CShaderProgram* GetShader(const std::string& name) const;
private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void DisplayHUD();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;

	// Allows for an easy query to find a certain shader by name
	std::unordered_map<std::string, CShaderProgram*> m_ShaderPrograms;

	std::unordered_map<std::string, std::vector<std::string>> m_EntityTemplates;
	int m_frameCount;
	double m_elapsedTime;
	friend class CurrencyManagerComponent;

};
