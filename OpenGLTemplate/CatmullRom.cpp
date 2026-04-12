#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>


CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	m_PathWidth = 30.f;
}

CCatmullRom::~CCatmullRom()
{
}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;

}


void CCatmullRom::CreateListGPUData(GLuint& vaoId, std::vector<glm::vec3>& points)
{
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	int textureCoordinateRetriever = 0;
	for (int i = 0; i < points.size(); i++)
	{
		float u = (i % 2 == 0) ? 0.0f : 1.0f;
		float v = (float)(i / 2) / 80.0f;

		glm::vec2 texCoord(u, v);

		vbo.AddData(&points[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}

void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
/*	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));*/
	ReadTrackCsv();
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}

void CCatmullRom::SetTrackFile(const std::string& newFile)
{
	m_File = newFile;
}

// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}

void CCatmullRom::ReadTrackCsv()
{
	std::ifstream f(m_File);
	std::string line;

	while (std::getline(f, line))
	{
		std::stringstream ss(line);
		std::string val;

		glm::vec3 splinePoint;
		glm::vec3 splinePointUpVector;

		float x, y, z;

		std::getline(ss, val, ',');
		x = static_cast<float>(stof(val));

		std::getline(ss, val, ',');
		y = static_cast<float>(stof(val));

		std::getline(ss, val, ',');
		z = static_cast<float>(stof(val));

		splinePoint = glm::vec3(x, y, z);


		std::getline(ss, val, ',');
		x = static_cast<float>(stof(val));

		std::getline(ss, val, ',');
		y = static_cast<float>(stof(val));

		std::getline(ss, val, ',');
		z = static_cast<float>(stof(val));

		splinePointUpVector = glm::vec3(x, y, z);

		m_controlPoints.push_back(splinePoint);
		m_controlUpVectors.push_back(splinePointUpVector);
	}
}



void CCatmullRom::CreateCentreline()
{
	SetControlPoints();
	UniformlySampleControlPoints(5000);

	CreateListGPUData(m_vaoCentreline, m_centrelinePoints);
}


void CCatmullRom::CreateOffsetCurves()
{
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3& p = m_centrelinePoints[i];
		glm::vec3& pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];

		glm::vec3 T = glm::normalize(pNext - p);

		glm::vec3 Up = (m_centrelineUpVectors.size() > i) ? m_centrelineUpVectors[i] : glm::vec3(0, 1, 0);

		glm::vec3 N = glm::normalize(glm::cross(T, Up));

		glm::vec3 B = glm::normalize(glm::cross(N, T));

		glm::vec3 l = p - ((m_PathWidth / 2.0f) * N);
		glm::vec3 r = p + ((m_PathWidth / 2.0f) * N);

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);

		m_centrelineUpVectors[i] = B;
	}

	CreateListGPUData(m_vaoLeftOffsetCurve, m_leftOffsetPoints);
	CreateListGPUData(m_vaoRightOffsetCurve, m_rightOffsetPoints);
}


void CCatmullRom::CreateTrack()
{
	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		m_trianglePoints.push_back(m_leftOffsetPoints[i]);
		m_trianglePoints.push_back(m_rightOffsetPoints[i]);
	}
	m_trianglePoints.push_back(m_leftOffsetPoints[0]);
	m_trianglePoints.push_back(m_rightOffsetPoints[0]);


	CreateListGPUData(m_vaoTrack, m_trianglePoints);
}

void CCatmullRom::CreateTrack(std::string a_sDirectory, std::string a_sFilename)
{
	m_texture.Load(a_sDirectory + a_sFilename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
	m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		m_trianglePoints.push_back(m_leftOffsetPoints[i]);
		m_trianglePoints.push_back(m_rightOffsetPoints[i]);
	}
	m_trianglePoints.push_back(m_leftOffsetPoints[0]);
	m_trianglePoints.push_back(m_rightOffsetPoints[0]);


	CreateListGPUData(m_vaoTrack, m_trianglePoints);
}


void CCatmullRom::RenderCentreline()
{
	glBindVertexArray(m_vaoCentreline);
	glLineWidth(2);
	glPointSize(5);
	glDrawArrays(GL_LINE_STRIP, 0, m_centrelinePoints.size());
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glLineWidth(2);
	glPointSize(5);
	glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());


	glBindVertexArray(m_vaoRightOffsetCurve);
	glLineWidth(2);
	glPointSize(5);
	glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::Render()
{
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	//glLineWidth(2);
	//glPointSize(5);	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_trianglePoints.size());
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);