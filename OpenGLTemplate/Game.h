#pragma once

#include "Common.h"
#include "GameWindow.h"
// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCrystal;
class CCatmullRom;
class Entity;
class Renderer;
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
	bool useCubeMap = false;

	CShaderProgram* shader = nullptr;

	bool renderSkybox = false;
};

struct FrameData
{
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::vec3 cameraPosition;

	struct Light
	{
		glm::vec4 position;
		glm::vec3 La, Ld, Ls;
	};

	std::vector<Light> lights;
};

enum Shaders
{
	MainShader = 0,
	FontShader = 1,
	CrystalShader = 2
};

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	void InitShaders();

	std::vector<std::string> ReadEntityLines(const std::string& filename);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;

	std::unique_ptr<CCatmullRom> m_CatmulRom;
	std::unique_ptr<CCrystal> m_pCrystal;
	std::unique_ptr<Renderer> m_Renderer;

	float m_currentDistance;
	float m_cameraSpeed;

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
	std::shared_ptr<Entity> FetchEntityByName(const std::string& name);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
	CShaderProgram* GetShader(const std::string& name) const;
private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	std::unordered_map<std::string, CShaderProgram*> m_ShaderPrograms;
	int m_frameCount;
	double m_elapsedTime;


};
