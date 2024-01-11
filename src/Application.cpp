#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<assert.h>  
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__));

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);    
}

static bool GLLogCall(const char* function,const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error <<") " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


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
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) {
        std::cout << "error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Loop until the user closes the window */

    float positions[] = {
    -0.5f,  -0.5f,  //0
     0.5f,  -0.5f,  //1
     0.5f,   0.5f,  //2
    -0.5f,   0.5f   //3
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };
    //VAO Vertex Array Object it is global
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    //创建顶点缓冲区
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 *  sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float) * 2 ,0) ;

    // 创建索引缓冲区
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // 创建ShaderProgram
    ShaderProgramSource source;
    source = ParseShader("resource/shaders/Basic.shader");

    std::cout << "" << source.FragmentSource << std::endl;
    std::cout << source.VertexSource << std::endl;


    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    glUseProgram(shader);
    //glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*int location = glGetUniformLocation*/
    int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);

    float increacement = 0.05f;
    float RGB_r = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glUniform4f(location, RGB_r, 0.3f, 0.8f, 1.0f);


        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        //glEnableVertexAttribArray(0);
        //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        //glDrawArrays(GL_TRIANGLES,0,3);
        // 
        //glDrawElements();
        // glDrawElement画得索引
        /*GLClearError();*/
        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        
        
        if (RGB_r >= 1.0f) {
            increacement = -0.05f;
        }
        else if(RGB_r <0){
            increacement = 0.05f;
        }

        RGB_r += increacement;
        

        /*assert(GLLogCall());*/ 

        /*ASSERT(GLLogCall());*/
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}