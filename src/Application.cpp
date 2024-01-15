#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main()
{
	// 在初始化后启用OpenGL错误检查
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLDebugCallback, nullptr);
#endif

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


	/* Create a Windowed mode and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window)
	{
		GLCall(glfwTerminate())
			return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	//constexpr float positions[] = {
	//	-0.5f, -0.5f,	0.0f, 0.0f,	// 0
	//	 0.5f, -0.5f,	1.0f, 0.0f,	// 1
	//	 0.5f,  0.5f,	1.0f, 1.0f,		// 2
	//	-0.5f,  0.5f,	0.0f, 1.0f		// 3
	//};
	constexpr float positions[] = {
	-50.0f, -50.0f, 0.0f, 0.0f,	// 0
	50.0f, -50.0f, 1.0f, 0.0f,	// 1
	50.0f, 50.0f, 1.0f, 1.0f,	// 2
	-50.0f, 50.0f, 0.0f, 1.0f	// 3
	 };


	const unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao))
		GLCall(glBindVertexArray(vao))

		VertexArray va;
	const VertexBuffer vb(positions, (unsigned int)(4) * 4 * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	const IndexBuffer ib(indices, 6);

	//set the outbound
	//it will be映射到 -1 1 空间
	//left right top bottom z
	////
	/*
		constexpr float positions[] = {
		-0.5f, -0.5f,	0.0f, 0.0f,	// 0
		 0.5f, -0.5f,	1.0f, 0.0f,	// 1
		 0.5f,  0.5f,	1.0f, 1.0f,		// 2
		-0.5f,  0.5f,	0.0f, 1.0f		// 3
	};
	take it for example
	ortho -2.0 to 2.0
	then the -0.5f will in -0.25 after the convert
	vp just make it to the -1 1 screen space


	Let's talk about mvp
	model: gameobject itself
	view: camera position scale rotate and so on

	*/

	//glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

	const glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(200, 0, 0));

	//const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
	//glm::mat4 mvp = proj * view * model;

	Shader shader("resource/shaders/Basic.glsl");
	shader.Bind();
	shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


	//shader.SetUniformMat4f("u_MVP", mvp);

	Texture texture("resource/textures/texture.png");
	texture.Bind();
	shader.SetUniform1i("u_Texture",0);

	va.Unbind();
	shader.Unbind();
	vb.Unbind();
	ib.Unbind();

	Renderer renderer;
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();

	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	float r = 0.0f;
	float increment = 0.05f;
	glm::vec3 translation(200, 200, 0);
	glm::vec3 translationA(200, 200, 0);
	glm::vec3 translationB(400, 400, 0);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 mvp = proj * view * model;

			/*shader.Bind();*/
		/*	shader.SetUniformMat4f("u_MVP", mvp);

		shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
		renderer.Draw(va,ib,shader);*/

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			 {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				
					renderer.Draw(va, ib, shader);
			}
		r += increment;
		{
			ImGui::Begin("ImGui");
			/*ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);*/
			ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
			ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		 ImGui::Render();
		 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and events */
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}