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
#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader(void);
	~Shader(void);

	GLuint getProgramID() const { return programID; }

	bool loadShader(GLuint type, std::string filename);
	bool compile();
	bool enable();
	bool disable();

	bool setUniform1f(std::string name, float f);
	bool setUniform1f(std::string name, int count, float* f);
	bool setUniform2f(std::string name, float f1, float f2);
	bool setUniform3f(std::string name, float f1, float f2, float f3);
	bool setUniform4f(std::string name, float f1, float f2, float f3, float f4);
	bool setUniform1i(std::string name, int i);
	bool setUniform1i(std::string name, int count, int* i);
	bool setUniformMat4(std::string name, const glm::mat4& m);
	bool setUniformMat4(std::string name, GLfloat *m);
	bool setUniformMat4(std::string name, std::vector<glm::mat4*>m);
	bool setUniformTexture(std::string name, GLuint textureID, GLuint unitID);
	bool setUniformLocation(std::string name, GLuint& loc);

private:
	GLuint programID;

	std::string vertexShaderPath;
	std::string vertexShaderSource;
	GLuint vertexShaderID;

	std::string fragmentShaderPath;
	std::string fragmentShaderSource;
	GLuint fragmentShaderID;

	std::string geometryShaderPath;
	std::string geometryShaderSource;
	GLuint geometryShaderID;

	std::string tessCtrlShaderPath;
	std::string tessCtrlShaderSource;
	GLuint tessCtrlShaderID;

	std::string tessEvalShaderPath;
	std::string tessEvalShaderSource;
	GLuint tessEvalShaderID;

	std::string computeShaderPath;
	std::string computeShaderSource;
	GLuint computeShaderID;

	bool hasVertexShader;
	bool hasFragmentShader;
	bool hasGeometryShader;
	bool hasTessCtrlShader;
	bool hasTessEvalShader;
	bool hasComputeShader;

	bool loadShaderSource(std::string &source, std::string filename);
};

#endif
