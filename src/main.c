#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mat5.h"

static void onresize(GLFWwindow *window, int width, int height)
{ glViewport(0, 0, width > 0 ? width : 1, height > 0 ? height : 1); }
static bool isshadercompilationsuccessful(GLuint shader);

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

        GLchar *buff = malloc(size + 1);
        if (!buff) { puts("memory allocation failed"); fclose(f); goto errorquit_afterinitglfw; }
        if (fread(buff, size, 1, f) < 1) { puts("error reading vertex shader source"); return 1; }
        fclose(f);
        buff[size] = '\0';

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &buff, NULL);
        glCompileShader(vs);
        free(buff);
        if (!isshadercompilationsuccessful(vs))
        {
            int len;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
            
            if (len > 0)
            {
                char *buff = malloc(len);
                if (!buff) { puts("memory allocation failed"); return 1; }
                glGetShaderInfoLog(vs, len, NULL, buff);
                printf("#### VERTEX SHADER ####\n%s", buff);
            }
            goto errorquit_afterinitglfw;
        }
        glAttachShader(prog, vs);

        // ===========================================
        
        if (!(f = fopen("res/fragment.glsl", "rb"))) { puts("failed to open res/fragment.glsl file"); goto errorquit_afterinitglfw; }

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        if (!(buff = malloc(size + 1))) { puts("memory allocation failed"); fclose(f); goto errorquit_afterinitglfw; }
        if (fread(buff, size, 1, f) < 1) { puts("error reading fragment shader source"); return 1; }
        fclose(f);
        buff[size] = '\0';

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &buff, NULL);
        glCompileShader(fs);
        free(buff);
        if (!isshadercompilationsuccessful(fs)) goto errorquit_afterinitglfw;
        glAttachShader(prog, fs);

        // ===========================================

        glLinkProgram(prog);
        GLint result;
        glGetProgramiv(prog, GL_LINK_STATUS, &result);
        if (!result) goto errorquit_afterinitglfw;

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

    // ===========================================

    GLint u_model = glGetUniformLocation(prog, "color");
    printf("%i\n", u_model);

    mat5f model, tmp;
    vec4f pos = {0, 0, -20, 0}, scale = {1, 1, 1, 1};
    //mat5f_translate(model, pos);
    //mat5f_scale(tmp, scale);
    //mat5f_mulm2(model, tmp);
    mat5f_idt(model);
    glUniform1fv(u_model, 25, model);

    // ===========================================

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

static bool isshadercompilationsuccessful(GLuint shader)
{
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    return result;
}