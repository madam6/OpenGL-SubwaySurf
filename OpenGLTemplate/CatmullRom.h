#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"
#include "Component.h"
#include "Renderable.h"

class CCatmullRom : public Renderable
{
public:
	CCatmullRom();
	~CCatmullRom();


	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();

	void CreateTrack();
	void CreateTrack(std::string a_sDirectory, std::string a_sFilename);
	void Render() override;

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3& p, glm::vec3& up = _dummy_vector); // Return a point on the centreline based on a certain distance along the control curve.
	void SetTrackFile(const std::string& newFile);
private:

	void CreateListGPUData(GLuint& vaoId, std::vector<glm::vec3>& points);


	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);

	void ReadTrackCsv();

	glm::vec3 Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t);


	std::vector<float> m_distances;
	CTexture m_texture;

	float m_PathWidth;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;

	std::string m_File;

	static glm::vec3 _dummy_vector;
	std::vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	std::vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	std::vector<glm::vec3> m_centrelinePoints;	// Centreline points
	std::vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	std::vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	std::vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points
	std::vector<glm::vec3> m_trianglePoints;


	unsigned int m_vertexCount;				// Number of vertices in the track VBO
};
