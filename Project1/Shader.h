#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <GL/glew.h>

class Shader
{
public: 

	GLuint id = 0;

	Shader(const char *vertexPath = NULL, const char *fragmentPath = NULL) {
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vss, fss;
		vss << vShaderFile.rdbuf();
		fss << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		std::string vSource_str = vss.str();
		std::string fSource_str = fss.str();
		const char *vShaderSource = vSource_str.c_str();
		const char *fShaderSource = fSource_str.c_str();

		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vShaderSource, NULL);
		glCompileShader(vShader);

		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fShaderSource, NULL);
		glCompileShader(fShader);

		id = glCreateProgram();
		glAttachShader(id, vShader);
		glAttachShader(id, fShader);
		glLinkProgram(id);

		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
};

