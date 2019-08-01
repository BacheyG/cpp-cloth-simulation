/*
 *
 * Copyright © 2010-2016 Balázs Tóth <tbalazs@iit.bme.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "shader.hpp"

Shader::Shader(void) :
	hasGeometryShader(false), hasFragmentShader(false)

{
}

Shader::~Shader(void)
{
  glDeleteProgram(programID);
}

bool Shader::loadShader(GLuint type, std::string filename)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (loadShaderSource(vertexShaderSource, filename))
		{
			vertexShaderPath = filename;
			hasVertexShader = true;
		}
		break;

	case GL_FRAGMENT_SHADER:
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (loadShaderSource(fragmentShaderSource, filename))
		{
			fragmentShaderPath = filename;
			hasFragmentShader = true;
		}
		break;

	case GL_GEOMETRY_SHADER:
		geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
		if (loadShaderSource(geometryShaderSource, filename))
		{
			geometryShaderPath = filename;
			hasGeometryShader = true;
		}
		break;

	case GL_TESS_CONTROL_SHADER:
		tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
		if (loadShaderSource(tessCtrlShaderSource, filename))
		{
			tessCtrlShaderPath = filename;
			hasTessCtrlShader = true;
		}
		break;

	case GL_TESS_EVALUATION_SHADER:
		tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
		if (loadShaderSource(tessEvalShaderSource, filename))
		{
			tessEvalShaderPath = filename;
			hasTessEvalShader = true;
		}
		break;

	case GL_COMPUTE_SHADER:
		computeShaderID = glCreateShader(GL_COMPUTE_SHADER);
		if (loadShaderSource(computeShaderSource, filename))
		{
			computeShaderPath = filename;
			hasComputeShader = true;
		}
		break;

	default:
		return false;
	}

	return true;
}

bool Shader::loadShaderSource(std::string &source, std::string filename)
{
	std::ifstream shaderStream(filename, std::ios::in);
	if (shaderStream.is_open())
	{
		if (source.length() > 0) source.clear();
		std::string line = "";
		while (std::getline(shaderStream, line))
		{
			source += "\n" + line;
		}
		shaderStream.close();
	}
	else
	{
		std::cerr << "Can't load shader file: " << filename << std::endl;
		return false;
	}
	return true;
}

bool Shader::compile()
{
	GLint result = GL_FALSE;
	int InfoLogLength;

	if (hasVertexShader) {
		const char* vertexShaderSourcePtr = vertexShaderSource.c_str();
		glShaderSource(vertexShaderID, 1, &vertexShaderSourcePtr, NULL);
		glCompileShader(vertexShaderID);

		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> vertexShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(vertexShaderID, InfoLogLength, NULL, &vertexShaderErrorMessage[0]);
			std::cout << "Vertex shader: " << vertexShaderErrorMessage.data() << std::endl;
		}
	}

	if (hasFragmentShader) {
		const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
		glShaderSource(fragmentShaderID, 1, &fragmentShaderSourcePtr, NULL);
		glCompileShader(fragmentShaderID);

		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> fragmentShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(fragmentShaderID, InfoLogLength, NULL, &fragmentShaderErrorMessage[0]);
			std::cout << "Fragment shader: " << fragmentShaderErrorMessage.data() << std::endl;
		}
	}

	if (hasGeometryShader) {
		const char* geometryShaderSourcePtr = geometryShaderSource.c_str();
		glShaderSource(geometryShaderID, 1, &geometryShaderSourcePtr, NULL);
		glCompileShader(geometryShaderID);

		glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(geometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> geometryShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(geometryShaderID, InfoLogLength, NULL, &geometryShaderErrorMessage[0]);
			std::cout << "Geometry shader: " << geometryShaderErrorMessage.data() << std::endl;
		}
	}

	if (hasTessCtrlShader) {
		const char* tessCtrlShaderSourcePtr = tessCtrlShaderSource.c_str();
		glShaderSource(tessCtrlShaderID, 1, &tessCtrlShaderSourcePtr, NULL);
		glCompileShader(tessCtrlShaderID);

		glGetShaderiv(tessCtrlShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(tessCtrlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> tessCtrlShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(tessCtrlShaderID, InfoLogLength, NULL, &tessCtrlShaderErrorMessage[0]);
			std::cout << "Tess Control shader: " << tessCtrlShaderErrorMessage.data() << std::endl;
		}
	}

	if (hasTessEvalShader) {
		const char* tessEvalShaderSourcePtr = tessEvalShaderSource.c_str();
		glShaderSource(tessEvalShaderID, 1, &tessEvalShaderSourcePtr, NULL);
		glCompileShader(tessEvalShaderID);

		glGetShaderiv(tessEvalShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(tessEvalShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> tessEvalShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(tessEvalShaderID, InfoLogLength, NULL, &tessEvalShaderErrorMessage[0]);
			std::cout << "Tess Control shader: " << tessEvalShaderErrorMessage.data() << std::endl;
		}
	}

	if (hasComputeShader) {
		const char* computeShaderSourcePtr = computeShaderSource.c_str();
		glShaderSource(computeShaderID, 1, &computeShaderSourcePtr, NULL);
		glCompileShader(computeShaderID);

		glGetShaderiv(computeShaderID, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE)
		{
			glGetShaderiv(computeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> computeShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(computeShaderID, InfoLogLength, NULL, &computeShaderErrorMessage[0]);
			std::cout << "Compute shader: " << computeShaderErrorMessage.data() << std::endl;
		}
	}

	programID = glCreateProgram();
	if (hasVertexShader)   glAttachShader(programID, vertexShaderID);
	if (hasTessCtrlShader) glAttachShader(programID, tessCtrlShaderID);
	if (hasTessEvalShader) glAttachShader(programID, tessEvalShaderID);
	if (hasGeometryShader) glAttachShader(programID, geometryShaderID);
	if (hasFragmentShader) glAttachShader(programID, fragmentShaderID);
	if (hasComputeShader)  glAttachShader(programID, computeShaderID);

	if (hasVertexShader)
	{
		glBindAttribLocation(programID, 0, "vPosition");
		glBindAttribLocation(programID, 1, "vNormal");
		glBindAttribLocation(programID, 2, "vTexCoord");
	}

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if (result != GL_TRUE)
	{
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> programErrorMessage(InfoLogLength);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &programErrorMessage[0]);
		std::cout << "Link: " << programErrorMessage.data() << std::endl;
	}

	if (hasVertexShader)   glDeleteShader(vertexShaderID);
	if (hasTessCtrlShader) glDeleteShader(tessCtrlShaderID);
	if (hasTessEvalShader) glDeleteShader(tessEvalShaderID);
	if (hasGeometryShader) glDeleteShader(geometryShaderID);
	if (hasFragmentShader) glDeleteShader(fragmentShaderID);
	if (hasComputeShader)  glDeleteShader(computeShaderID);

	return true;
}

bool Shader::enable()
{
	glUseProgram(programID);
	return true;
}

bool Shader::disable()
{
	glUseProgram(0);
	return true;
}

bool Shader::setUniformMat4(std::string name, const glm::mat4& m)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, &m[0][0]);
	return true;
}

bool Shader::setUniformMat4(std::string name, GLfloat *m)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, m);
	return true;
}

bool Shader::setUniformMat4(std::string name, std::vector<glm::mat4*> m)
{
	float* mArray = new float[m.size() * 16];
	for (unsigned int i = 0; i < m.size(); ++i)
	{
		::memcpy(&mArray[i * 16], &m[i][0][0], 16 * sizeof(float));
	}
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(id, (GLsizei)m.size(), GL_FALSE, mArray);
	delete[] mArray;
	return false;
}


bool Shader::setUniform1f(std::string name, float f1)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform1f(id, f1);
	return true;
}

bool Shader::setUniform1f(std::string name, int count, float* f)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform1fv(id, count, f);
	return true;
}

bool Shader::setUniform2f(std::string name, float f1, float f2)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform2f(id, f1, f2);
	return true;
}

bool Shader::setUniform3f(std::string name, float f1, float f2, float f3)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform3f(id, f1, f2, f3);
	return true;
}

bool Shader::setUniform4f(std::string name, float f1, float f2, float f3, float f4)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform4f(id, f1, f2, f3, f4);
	return true;
}

bool Shader::setUniform1i(std::string name, int i)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform1i(id, i);
	return true;
}

bool Shader::setUniform1i(std::string name, int count, int *i)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glUniform1iv(id, count, i);
	return true;
}

bool Shader::setUniformTexture(std::string name, GLuint textureID, GLuint unitID)
{
	GLuint id = glGetUniformLocation(programID, name.c_str());
	glActiveTexture(GL_TEXTURE0 + unitID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(id, unitID);
	return true;
}

bool Shader::setUniformLocation(std::string name, GLuint& loc)
{
	loc = glGetUniformLocation(programID, name.c_str());
	return true;
}
