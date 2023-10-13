#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <vector>

class CubieRenderer
{
public:
	void Initialize();
	void Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& modelMatrix);
	void Render(const glm::mat4& transformMatrix);
	void ClearResources();

	float GetCubieExtension()const { return 2.0f * m_offset; }

private:
	const float m_offset = 0.5f;

	GLuint m_arrayBufferObject;
	GLuint m_vertexBufferObject[5];
	GLuint m_shaderProgram;
	GLuint m_projectionLocation;
	GLuint m_viewLocation;
	GLuint m_modelLocation;
	GLuint m_texture;
	GLuint m_normalMap;
	GLuint m_environmentMap;

private:
	static void TranscribeToFloatArray(const std::vector <glm::vec3>& vectors, float* float_array);
	static void TranscribeToFloatArray(const std::vector<glm::vec2>& vectors, float* float_array);
	void AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec2>& texCoords);
	void AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colors);
};

