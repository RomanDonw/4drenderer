/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "mat4.h"
#include "mat5.h"

#define GLDEBUG() (printf("OpenGL error %u at %llu:%s in function %s\n", glGetError(), __LINE__, __FILE__, __func__))

unsigned int winwidth, winheight;
static void onresize(GLFWwindow *window, int width, int height)
{
    winwidth = width > 0 ? width : 1;
    winheight = height > 0 ? height : 1;
    glViewport(0, 0, winwidth, winheight);
}

static bool isshadercompilationsuccessful(GLuint shader);
static void gentransform(mat5f out, const vec4f pos, const float euler[6], const vec4f scale);

#define PI4 0.78539816339744830962

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

        char *buff = malloc(size + 1);
        if (!buff) { puts("memory allocation failed"); fclose(f); goto errorquit_afterinitglfw; }
        if (fread(buff, size, 1, f) < 1) { puts("error reading vertex shader source"); return 1; }
        fclose(f);
        buff[size] = '\0';

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, (void *)&buff, NULL);
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
        glShaderSource(fs, 1, (void *)&buff, NULL);
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
        0.5, -0.5, 2.5, 0,
        0, 0.5, 2.5, 0,
        -0.5, -0.5, 2.5, 0,
        
        0.5, -0.5, -2.5, 5,
        0, 0.5, -2.5, 5,
        -0.5, -0.5, -2.5, 5,
    };
    static const unsigned int indices[] =
    {
        
        0, 1, 2,
        3, 4, 5,

        3, 4, 0,
        4, 1, 0,

        2, 5, 4,
        4, 1, 2,

        0, 2, 5,
        5, 3, 0
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

    GLint u_model = glGetUniformLocation(prog, "model");
    printf("%i\n", u_model);

    mat5f model;
    vec4f pos = {1, 2, 0, 0}, scale = {1, 1, 1, 1};
    float rot[6] = {0};

    GLint u_perp = glGetUniformLocation(prog, "perp");
    mat4f perp;
    printf("%i\n", u_perp);

    GLint u_view = glGetUniformLocation(prog, "view");
    printf("%i\n", u_view);
    vec4f campos = {0};
    float camrot[6] = {0};
    
    // ===========================================

    glDisable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glClearColor(0, 0, 0, 1);
    double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(w))
    {
        double currtime = glfwGetTime();
        double delta = currtime - lasttime;
        lasttime = currtime;

        // ===========================================
        
        glfwPollEvents();
        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
        if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) campos[2] += 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) campos[2] -= 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) campos[0] -= 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) campos[0] += 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) campos[1] -= 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) campos[1] += 1 * delta;
        
        if (glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS) campos[3] -= 1 * delta;
        if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) campos[3] += 1 * delta;

        char sign = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? -1 : 1;
        if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS) rot[0] += sign * delta;
        if (glfwGetKey(w, GLFW_KEY_T) == GLFW_PRESS) rot[1] += sign * delta;
        if (glfwGetKey(w, GLFW_KEY_Y) == GLFW_PRESS) rot[2] += sign * delta;
        if (glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS) rot[3] += sign * delta;
        if (glfwGetKey(w, GLFW_KEY_G) == GLFW_PRESS) rot[4] += sign * delta;
        if (glfwGetKey(w, GLFW_KEY_H) == GLFW_PRESS) rot[5] += sign * delta;

        // ===========================================
        
        mat4f_perspective(perp, PI4, (float)winwidth / winheight, 0.1, 1000);
        glUniformMatrix4fv(u_perp, 1, GL_FALSE, perp);

        // ===========================================
        
        gentransform(model, pos, rot, scale);
        glUniform1fv(u_model, 25, model);

        // ===========================================

        {
            vec5f front = {0, 0, -1, 0, 0}, right = {1, 0, 0, 0, 0}, up = {0, 1, 0, 0, 0}, over = {0, 0, 0, 1, 0};
            mat5f tmp;
            gentransform(tmp, campos, camrot, NULL);
            mat5f_mulv2(front, tmp);
            mat5f_mulv2(right, tmp);
            mat5f_mulv2(up, tmp);
            mat5f_mulv2(over, tmp);
            vec4f_norm2(front);
            vec4f_norm2(right);
            vec4f_norm2(up);
            vec4f_norm2(over);
            
            mat5f_lookat(tmp, campos, front, right, up, over);
            glUniform1fv(u_view, 25, tmp);
        }

        // ===========================================

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, NULL);
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

static void gentransform(mat5f out, const vec4f pos, const float euler[6], const vec4f scale)
{
    mat5f tmp;
    mat5f_translate(out, pos);

    mat5f_rotatexy(tmp, euler[0]);
    mat5f_mulm2(out, tmp);
    mat5f_rotateyz(tmp, euler[1]);
    mat5f_mulm2(out, tmp);
    mat5f_rotatezx(tmp, euler[2]);
    mat5f_mulm2(out, tmp);

    mat5f_rotatexw(tmp, euler[3]);
    mat5f_mulm2(out, tmp);
    mat5f_rotateyw(tmp, euler[4]);
    mat5f_mulm2(out, tmp);
    mat5f_rotatezw(tmp, euler[5]);
    mat5f_mulm2(out, tmp);

    if (scale)
    {
        mat5f_scale(tmp, scale);
        mat5f_mulm2(out, tmp);
    }
}