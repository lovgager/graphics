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
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		std::string vSource_str;
		std::string fSource_str;
		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vss, fss;
			vss << vShaderFile.rdbuf();
			fss << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vSource_str = vss.str();
			fSource_str = fss.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const char *vShaderSource = vSource_str.c_str();
		const char *fShaderSource = fSource_str.c_str();
		GLint success;
		GLchar infoLog[512];

		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vShaderSource, NULL);
		glCompileShader(vShader);
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		

		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fShaderSource, NULL);
		glCompileShader(fShader);
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(fShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		id = glCreateProgram();
		glAttachShader(id, vShader);
		glAttachShader(id, fShader);
		glLinkProgram(id);

		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
};

