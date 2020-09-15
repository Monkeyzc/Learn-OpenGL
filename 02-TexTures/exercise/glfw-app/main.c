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

#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"


float mixValue = 0.2f;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 outColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   outColor = aColor;\n"
    "   TexCoord = aTexCoord;\n"
    "}\n";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 outColor;\n"
    "in vec2 TexCoord;\n"
    "uniform float mixValue;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main()\n"
    "{\n"
    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue) * vec4(outColor, 1.0);\n"
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
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mixValue += 0.01f;
        if (mixValue >= 1.0f) {
            mixValue = 1.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mixValue -= 0.01f;
        if (mixValue <= 0.0f) {
            mixValue = 0.0f;
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    int width = 640;
    int height = 480;
    
    int success = 0;
    char infoLog[1024] = {0, };
    
    
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
    
    glfwWindowHint(GLFW_SAMPLES, 4);

    
    
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
    
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
    
    // vertex shafer
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        printf("vertexShader glGetShaderiv error: %s\n", infoLog);
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
        printf("fragmentShader glGetShaderiv error: %s\n", infoLog);
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
        printf("shaderProgram glGetProgramiv error: %s\n", infoLog);
        goto __TERMINAL;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    float vertices[] = {
        // --- 位置 ---          --- 颜色 ---      --- 纹理坐标 ---
        0.5f,   0.5f, 0.0f,  1.0f, 0.0f, 0.0f,      3.0f, 3.0f,                      // right top
        0.5f,  -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,      3.0f, 0.0f,                      // right bottom
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,      0.0f, 0.0f,                       // left bottom
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,      0.0f, 3.0f,                      // left top
    };
    
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    // 初始化
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // 绑定VAO
    glBindVertexArray(VAO);
    
    // 把顶点数组复制到缓冲中, 供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    // 颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 *sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // 纹理
    unsigned int texture1, texture2;
    
    // texture1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 当进行放大 Magnify 和 缩小 Minify 操作, 设置的纹理过滤选项, 领近过滤/线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // 加载图片
    int imageWidth, imageHeight, imageNrChannels;
    // 颠倒图片的Y轴
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("./assert/container.jpg", &imageWidth, &imageHeight, &imageNrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // 多级渐远纹理
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // 释放内存
    stbi_image_free(data);
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // 加载图片
    // GL_RGBA 图片包含alpha 通道
    data = stbi_load("./assert/awesomeface-1.png", &imageWidth, &imageHeight, &imageNrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // 释放内存
    stbi_image_free(data);
    
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
    
    /*
        渲染循环(Render Loop)
     */
    while (!glfwWindowShouldClose(window)) {
        // 输入处理
        processInput(window);
        
        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
//        GLfloat timeValue = glfwGetTime();
//        GLfloat gamma = sin(timeValue) / 2 + 0.5;
        
        glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        // 事件处理
        glfwPollEvents();
    }
    
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
    
    if (EBO) {
        glDeleteBuffers(1, &EBO);
    }
    
__TERMINAL:
    glfwTerminate();
    return 0;
}
