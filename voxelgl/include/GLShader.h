#ifndef GLSHADER_H
#define GLSHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class GLShader {
private:
	GLuint programID;
	
	std::string readFile(const std::string& filePath);
	GLuint compileShader(GLenum type, const char *source);
	bool checkCompileErrors(GLuint shader, std::string type);

public:
	GLShader();
	~GLShader();

	bool load(const std::string& vertexPath      = "",
			  const std::string& fragmentPath    = "",
			  const std::string& geometryPath    = "",
			  const std::string& tessControlPath = "");
	void use();

	// Uniform setters
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, const glm::vec2& value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setVec4(const std::string& name, const glm::vec4& value);
	void setMat4(const std::string& name, const glm::mat4& value);

	GLuint getProgramID() const { return programID; }
};

#endif