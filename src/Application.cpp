#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string filepath) {
    std::ifstream stream(filepath);
    std::string line;


    enum ShaderType{
        NONE=-1,VERTEX = 0, FRAGMENT = 1
    };
    std::stringstream ss[2];

    ShaderType shadertype = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos){
            if (line.find("vertex") != std::string::npos) {
                // vertex mode
                shadertype = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                //fragment mode
                shadertype = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)shadertype] << line << '\n';
;        }

        //if (line == "#shader vertex") {

        //}
        //else if (line == "#shader fragment") {

        //}
    }

    return { ss[0].str(),ss[1].str() };
}

static int CompileShader(unsigned int type, const std::string& source) {
    //create a empty shader
    unsigned int id = glCreateShader(type);
    //get the source string pointer
    const char* src = source.c_str();
    //set the idshader with the src
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length,message);
        std::cout << "Failed to complie"<< (type == GL_VERTEX_SHADER?  "vertex"  :  "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }


    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
   
    unsigned int program = glCreateProgram();
    //use the shader string to compile to shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);



    glDeleteShader(vs);
    glDeleteShader(fs);


    return program;

    
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Loop until the user closes the window */

    float positions[] = {
    -0.5f, -0.5f,
    0.0f, 0.5f,
    0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float) * 2 ,0) ;


    ShaderProgramSource source;
    source = ParseShader("resource/shaders/Basic.shade");

    std::cout << "" << source.FragmentSource << std::endl;
    std::cout << source.VertexSource << std::endl;


    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    glUseProgram(shader);
    //glBindBuffer(GL_ARRAY_BUFFER,0);
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES,0,3);
        //glDrawElements();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}