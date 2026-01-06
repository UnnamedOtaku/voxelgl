#include "GLShader.h"

GLShader::GLShader() : programID(0) {}

GLShader::~GLShader() {
	if (programID != 0) {
		glDeleteProgram(programID);
	}
}

std::string GLShader::readFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
		return "";
	}

	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

GLuint GLShader::compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	if (!checkCompileErrors(shader, (type == GL_VERTEX_SHADER) ? "VERTEX" :
		(type == GL_GEOMETRY_SHADER) ? "GEOMETRY" :
		(type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :
		(type == GL_TESS_CONTROL_SHADER) ? "TESS_CONTROL" :
		"TESS_EVALUATION")) {
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool GLShader::checkCompileErrors(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "Shader compilation error: " << type << "\n" << infoLog << std::endl;
			return false;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "Program linking error: " << type << "\n" << infoLog << std::endl;
			return false;
		}
	}

	return true;
}

bool GLShader::load(const std::string& vertexPath,
	const std::string& fragmentPath,
	const std::string& geometryPath,
	const std::string& tessControlPath) {
	// Leer shaders
	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);

	if (vertexCode.empty() || fragmentCode.empty()) {
		return false;
	}

	// Compilar shaders
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

	if (!vertexShader || !fragmentShader) {
		return false;
	}

	// Geometry shader opcional
	GLuint geometryShader = 0;
	if (!geometryPath.empty()) {
		std::string geometryCode = readFile(geometryPath);
		if (!geometryCode.empty()) {
			geometryShader = compileShader(GL_GEOMETRY_SHADER, geometryCode.c_str());
		}
	}

	// Tessellation shader opcional
	GLuint tessControlShader = 0;
	if (!tessControlPath.empty()) {
		std::string tessControlCode = readFile(tessControlPath);
		if (!tessControlCode.empty()) {
			tessControlShader = compileShader(GL_TESS_CONTROL_SHADER, tessControlCode.c_str());
		}
	}

	// Crear programa
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	if (geometryShader != 0)    glAttachShader(programID, geometryShader);
	if (tessControlShader != 0) glAttachShader(programID, tessControlShader);

	glLinkProgram(programID);

	if (!checkCompileErrors(programID, "PROGRAM")) {
		glDeleteProgram(programID);
		programID = 0;
		return false;
	}

	// Limpiar shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryShader != 0)    glDeleteShader(geometryShader);
	if (tessControlShader != 0) glDeleteShader(tessControlShader);

	std::cout << "Shaders loaded successfully" << std::endl;
	return true;
}

void GLShader::use() {
	glUseProgram(programID);
}

void GLShader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void GLShader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void GLShader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void GLShader::setVec2(const std::string& name, const glm::vec2& value) {
	glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value));
}

void GLShader::setVec3(const std::string& name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value));
}

void GLShader::setVec4(const std::string& name, const glm::vec4& value) {
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value));
}

void GLShader::setMat4(const std::string& name, const glm::mat4& value) {
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}