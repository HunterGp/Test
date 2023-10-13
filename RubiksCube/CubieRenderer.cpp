#include "CubieRenderer.h"
#include "ShaderUtil.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void CubieRenderer::Initialize()
{
	float floatArrayVec3[6 * 6 * 3];
	float floatArrayVec2[6 * 6 * 2];
	std::vector<glm::vec3> positionField;
	std::vector<glm::vec3> normalsField;
	std::vector<glm::vec3> tangentsField;
	std::vector<glm::vec2> uvField;
	std::vector<glm::vec3> colorField;

	for (int sideType = 0; sideType < 3; sideType++)
	{
		for (int direction = -1; direction < 2; direction += 2)
		{
			AddSidePosition(sideType, direction, positionField, normalsField, tangentsField, uvField);
			AddSideColor(sideType, direction, colorField);
		}
	}

	m_shaderProgram = ShaderUtil::CreateShaderProgram("VBlinnPhong.glsl", "FBlinnPhong.glsl");

	m_projectionLocation = glGetUniformLocation(m_shaderProgram, "Projection");
	m_viewLocation = glGetUniformLocation(m_shaderProgram, "View");
	m_modelLocation = glGetUniformLocation(m_shaderProgram, "Model");
	glGenVertexArrays(1, &m_arrayBufferObject);
	glGenBuffers(5, m_vertexBufferObject);

	glBindVertexArray(m_arrayBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[0]);
	TranscribeToFloatArray(positionField, floatArrayVec3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floatArrayVec3), floatArrayVec3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[1]);
	TranscribeToFloatArray(normalsField, floatArrayVec3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floatArrayVec3), floatArrayVec3, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[2]);
	TranscribeToFloatArray(tangentsField, floatArrayVec3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floatArrayVec3), floatArrayVec3, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[3]);
	TranscribeToFloatArray(uvField, floatArrayVec2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floatArrayVec2), floatArrayVec2, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[4]);
	TranscribeToFloatArray(colorField, floatArrayVec3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floatArrayVec3), floatArrayVec3, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int w, h, comp;

	unsigned char* image = stbi_load("CubeTexture.jpg", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	image = stbi_load("CubeNormal.png", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &m_normalMap);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	std::vector<std::string> envTextureFaces =
	{
		"posx.jpg",
		"negx.jpg",
		"posy.jpg",
		"negy.jpg",
		"posz.jpg",
		"negz.jpg"
	};

	glGenTextures(1, &m_environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	for (unsigned int i = 0; i < envTextureFaces.size(); i++)
	{
		image = stbi_load(envTextureFaces[i].c_str(), &w, &h, &comp, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubieRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& modelMatrix)
{
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_arrayBufferObject);
	GLint texLoc;
	texLoc = glGetUniformLocation(m_shaderProgram, "edgeTexture");
	glUniform1i(texLoc, 0);

	texLoc = glGetUniformLocation(m_shaderProgram, "normalMap");
	glUniform1i(texLoc, 1);

	texLoc = glGetUniformLocation(m_shaderProgram, "environmentMap");
	glUniform1i(texLoc, 2);

	glUniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(m_viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void CubieRenderer::Render(const glm::mat4& transformMatrix)
{
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_arrayBufferObject);

	glUniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void CubieRenderer::ClearResources()
{
	glDeleteProgram(m_shaderProgram);
	glDeleteBuffers(2, m_vertexBufferObject);
	glDeleteVertexArrays(1, &m_arrayBufferObject);
}

void CubieRenderer::TranscribeToFloatArray(const std::vector<glm::vec3>& vectors, float* float_array)
{
	assert(vectors.size() == 36); //Illegal amount of entries
	int pointer = 0;	
	for (const glm::vec3& position : vectors)
	{
		float_array[pointer++] = position.x;
		float_array[pointer++] = position.y;
		float_array[pointer++] = position.z;
	}
}

void CubieRenderer::TranscribeToFloatArray(const std::vector<glm::vec2>& vectors, float* float_array)
{
	assert(vectors.size() == 36); //Illegal amount of entries
	int pointer = 0;
	for (const glm::vec2& position : vectors)
	{
		float_array[pointer++] = position.x;
		float_array[pointer++] = position.y;
	}
}

void CubieRenderer::AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec2>& texCoords)
{
	glm::vec3 cornerPoints[2][2];
	glm::vec2 uvCoords[2][2];

	int localXDim = (sideType + 1) % 3;
	int localYDim = (sideType + 2) % 3;
	int localZDim = sideType;

	glm::vec3 normal = glm::vec3(direction, direction, direction);
	normal[localXDim] = 0;
	normal[localYDim] = 0;

	glm::vec3 tangent = glm::vec3(direction, direction, direction);
	tangent[localYDim] = 0;
	tangent[localZDim] = 0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			glm::vec3 localPoint(direction * m_offset);
			localPoint[localXDim] = (2 * i - 1) * m_offset * direction; //Times direction to fix normalMap rotation on some Sides
			localPoint[localYDim] = (2 * j - 1) * m_offset;
			cornerPoints[i][j] = localPoint;

			glm::vec2 localUV = glm::vec2(i, j);
			uvCoords[i][j] = localUV;
		}
	}

	positions.push_back(cornerPoints[0][0]);
	positions.push_back(cornerPoints[1][0]);
	positions.push_back(cornerPoints[0][1]);
	
	positions.push_back(cornerPoints[1][0]);
	positions.push_back(cornerPoints[1][1]);
	positions.push_back(cornerPoints[0][1]);

	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

	tangents.push_back(tangent);
	tangents.push_back(tangent);
	tangents.push_back(tangent);
			  
	tangents.push_back(tangent);
	tangents.push_back(tangent);
	tangents.push_back(tangent);

	texCoords.push_back(uvCoords[0][1]);
	texCoords.push_back(uvCoords[1][1]);
	texCoords.push_back(uvCoords[0][0]);
					
	texCoords.push_back(uvCoords[1][1]);
	texCoords.push_back(uvCoords[1][0]);
	texCoords.push_back(uvCoords[0][0]);
}

void CubieRenderer::AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colors)
{
	glm::vec3 red = glm::vec3(0.7f, 0.0f, 0.0f);
	glm::vec3 green = glm::vec3(0.0f, 0.6f, 0.0f);
	glm::vec3 blue = glm::vec3(0.0f, 0.0f, 0.7f);
	glm::vec3 orange = glm::vec3(0.8f, 0.3f, 0.0f);
	glm::vec3 yellow = glm::vec3(0.7f, 0.7f, 0.0f);
	glm::vec3 white = glm::vec3(0.9f, 0.9f, 0.9f);

	std::vector<glm::vec3> colorValues = { red, orange, green, blue, white, yellow };

	int colorIndex = direction == 1 ? sideType * 2 : sideType * 2 + 1;
	glm::vec3 color = colorValues[colorIndex];

	for (int i = 0; i < 6; i++)
		colors.push_back(color);
}
