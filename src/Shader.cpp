#include "Shader.h"
#include "Renderer.h"
#include <sstream>
#include <fstream>
#include <iostream>
Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath) , m_RendererID(0)
{
	const ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath) const
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[static_cast<int>(type)] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	const unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr))
		GLCall(glCompileShader(id))

		// TODO: Error handling
		int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result))
		if (result == GL_FALSE)
		{
			int length;
			GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length))

				char* message = static_cast<char*>(malloc(length * sizeof(char)));
			GLCall(glGetShaderInfoLog(id, length, &length, message))

				std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
			std::cout << message << std::endl;

			GLCall(glDeleteShader(id))
				return 0;
		}


	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	const unsigned int program = glCreateProgram();
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs))
		GLCall(glAttachShader(program, fs))
		GLCall(glLinkProgram(program))
		GLCall(glValidateProgram(program))

		GLCall(glDeleteShader(vs))
		GLCall(glDeleteShader(fs))

		return program;
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	return 0;
}
