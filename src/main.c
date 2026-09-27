#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static void onresize(GLFWwindow *window, int width, int height)
{ glViewport(0, 0, width > 0 ? width : 1, height > 0 ? height : 1); }

int main(void)
{
    if (!glfwInit()) { puts("failed to initialize GLFW"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *w = glfwCreateWindow(800, 600, "4D Renderer", NULL, NULL);
    if (!w) goto errorquit_afterinitglfw;
    glfwMakeContextCurrent(w);
    glfwSetWindowSizeCallback(w, onresize);

    if (!gladLoadGL(glfwGetProcAddress)) { puts("failed to load OpenGL context through GLAD"); goto errorquit_afterinitglfw; }

    GLuint prog = glCreateProgram();
    {
        FILE *f = fopen("res/vertex.glsl", "rb");
        if (!f) { puts("failed to open res/vertex.glsl file"); goto errorquit_afterinitglfw; }

        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buff = malloc(size);
        if (!buff) { puts("memory allocation failed"); fclose(f); goto errorquit_afterinitglfw; }
        fread(buff, size, 1, f);
        fclose(f);

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, (void *)&buff, &size);
        free(buff);
        glCompileShader(vs);
        glAttachShader(prog, vs);

        // ===========================================
        
        if (!(f = fopen("res/fragment.glsl", "rb"))) { puts("failed to open res/fragment.glsl file"); goto errorquit_afterinitglfw; }

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        if (!(buff = malloc(size))) { puts("memory allocation failed"); fclose(f); goto errorquit_afterinitglfw; }
        fread(buff, size, 1, f);
        fclose(f);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, (void *)&buff, &size);
        free(buff);
        glCompileShader(fs);
        glAttachShader(prog, fs);

        // ===========================================

        glLinkProgram(prog);
        glUseProgram(prog);
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    static const float vertices[] =
    {
        -0.5, -0.5, 0, 1,
        0, 0.5, 0, 1,
        0.5, -0.5, 0, 1
    };
    static const int indices[] =
    {
        0, 1, 2
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glClearColor(0, 0, 0, 1);
    while (!glfwWindowShouldClose(w))
    {
        glfwPollEvents();
        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(w);
    }

    return 0;
    errorquit_afterinitglfw:
        glfwTerminate();
    return 1;
}
