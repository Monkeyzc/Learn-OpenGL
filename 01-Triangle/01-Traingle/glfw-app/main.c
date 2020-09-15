//
//  main.c
//  glfw-app
//
//  Created by zhaofei on 2020/9/7.
//  Copyright © 2020 zhaofei. All rights reserved.
//

#include <stdio.h>
#include "glad.h"
#include "glfw3.h"


// 顶点着色器
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";

// 片段着色器
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";


void error_callback(int error, const char *description) {
    printf("error_callback: error: %d, description: %s\n", error, description);
}

/*
 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理
 当窗口被第一次显示的时候framebuffer_size_callback也会被调用
 */
void frame_buffersize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 输入处理函数
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    int width = 640;
    int height = 480;
    
    printf("Hello, World!\n");
    if (!glfwInit()) {
        printf("glfw init failed!\n");
        return 0;
    }
    
    // 告诉GLFW我们要使用的OpenGL版本是3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 告诉GLFW我们使用的是核心模式(Core-profile)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // MAC OS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    
    
    glfwSetErrorCallback(error_callback);
    
    GLFWwindow *window = glfwCreateWindow(width, height, "Hello world", NULL, NULL);
    if (!window) {
        printf("create window failed\n");
        goto __TERMINAL;
    }
    
    glfwMakeContextCurrent(window);
    
    // GLAD是用来管理OpenGL的函数指针的, 所以在调用任何OpenGL的函数之前我们需要初始化GLAD
    // glad初始化
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("加载失败");
        goto __TERMINAL;
    }
    
    glfwSetFramebufferSizeCallback(window, frame_buffersize_callback);
    
    int success;
    char infoLog[1024];
    
    // vertex shafer
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        printf("vertexShader glGetShaderiv error: %s", infoLog);
        goto __TERMINAL;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        printf("fragmentShader glGetShaderiv error: %s", infoLog);
        goto __TERMINAL;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 1024, NULL, infoLog);
        printf("shaderProgram glGetProgramiv error: %s", infoLog);
        goto __TERMINAL;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };
    
    // 初始化
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // 绑定VAO
    glBindVertexArray(VAO);
    
    // 把顶点数组复制到换从中OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 *sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    /*
        渲染循环(Render Loop)
     */
    while (!glfwWindowShouldClose(window)) {
        // 输入处理
        processInput(window);
        
        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        // 事件处理
        glfwPollEvents();
    }
    
__TERMINAL:
    glfwTerminate();
    
    return 0;
}
