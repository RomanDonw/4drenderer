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
#define PI 3.14159265358979323846
#define RAD(deg) ((deg) / (float)180 * PI)

static unsigned int winwidth, winheight;
static GLint u_perp = -1;
static void onresize(GLFWwindow *window, int width, int height)
{
    winwidth = width > 0 ? width : 1;
    winheight = height > 0 ? height : 1;
    glViewport(0, 0, winwidth, winheight);

    if (~u_perp)
    {
        mat4f mat4;
        mat4f_perspective(mat4, RAD(90) / (float)2, (float)winwidth / winheight, 0.1, 1000);
        glUniformMatrix4fv(u_perp, 1, GL_FALSE, mat4);
    }
}

static bool isshadercompilationsuccessful(GLuint shader);
static void gentransform(mat5f out, const vec4f pos, const float euler[6], const vec4f scale);
static void genrotmat(mat5f out, const float euler[6]);
static char getshadercomplog(GLuint shader, char **log);

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
        char *buff = NULL;
        if (!f) { puts("failed to open res/vertex.glsl file"); goto errorquit; }

        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (!(buff = malloc(size + 1))) { puts("memory allocation failed"); goto errorquit; }
        if (fread(buff, size, 1, f) < 1) { puts("error reading vertex shader source"); goto errorquit; }
        fclose(f); f = NULL;
        buff[size] = '\0';

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, (void *)&buff, NULL);
        glCompileShader(vs);
        free(buff); buff = NULL;
        if (!isshadercompilationsuccessful(vs))
        {
            char *log;
            if (getshadercomplog(vs, &log)) { puts("failed getting vertex shader compilation log"); goto errorquit; }
            printf("#### VERTEX SHADER COMPILING LOG ####\n%s", log);
            free(log);
            goto errorquit;
        }
        glAttachShader(prog, vs);

        // ===========================================
        
        if (!(f = fopen("res/fragment.glsl", "rb"))) { puts("failed to open res/fragment.glsl file"); goto errorquit; }

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        if (!(buff = malloc(size + 1))) { puts("memory allocation failed"); fclose(f); goto errorquit; }
        if (fread(buff, size, 1, f) < 1) { puts("error reading fragment shader source"); goto errorquit; }
        fclose(f);
        f = NULL;
        buff[size] = '\0';

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, (void *)&buff, NULL);
        glCompileShader(fs);
        free(buff);
        buff = NULL;
        if (!isshadercompilationsuccessful(fs))
        {
            char *log;
            if (getshadercomplog(fs, &log)) { puts("failed getting fragment shader compilation log"); goto errorquit; }
            printf("#### FRAGMENT SHADER COMPILING LOG ####\n%s", log);
            free(log);
            goto errorquit;
        }
        glAttachShader(prog, fs);

        // ===========================================

        glLinkProgram(prog);
        GLint result;
        glGetProgramiv(prog, GL_LINK_STATUS, &result);
        if (!result) { puts("failed linking shader program"); goto errorquit; }

        glUseProgram(prog);

        // ===========================================

        goto successquit;
        errorquit:
            if (f) fclose(f);
            free(buff);
        goto errorquit_afterinitglfw;
        successquit:
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

    u_perp = glGetUniformLocation(prog, "perp");
    GLint u_view = glGetUniformLocation(prog, "view");
    GLint u_model = glGetUniformLocation(prog, "model");

    vec4f modelpos = {1, 2, 0, 0}, modelscale = {1, 1, 1, 1};
    float modelrot[6] = {0};
    
    vec4f campos = {0};
    float camrot[6] = {0};
    
    // ===========================================

    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glDisable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 1);
    double lasttime = glfwGetTime();
    vec4f vec4;
    mat5f mat5;
    mat4f mat4;
    while (!glfwWindowShouldClose(w))
    {
        double currtime = glfwGetTime();
        double delta = currtime - lasttime;
        lasttime = currtime;

        // ===========================================

        glfwPollEvents();

        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(w, true);
        
        {
            static double lastx = 0, lasty = 0;
            double x, y;
            glfwGetCursorPos(w, &x, &y);
            double deltax = x - lastx;
            double deltay = y - lasty;
            lastx = x;
            lasty = y;

            camrot[0] -= deltax * 0.005;
            camrot[1] += deltay * 0.005;

            camrot[0] = fmodf(camrot[0], 2 * PI);
            camrot[1] = fmodf(camrot[1], 2 * PI);
        }
        
        vec5f front = {0, 0, -1, 0, 0}, right = {1, 0, 0, 0, 0}, up = {0, 1, 0, 0, 0}, over = {0, 0, 0, 1, 0};
        genrotmat(mat5, camrot);

        mat5f_mulv2(front, mat5);
        mat5f_mulv2(right, mat5);
        mat5f_mulv2(up, mat5);
        mat5f_mulv2(over, mat5);

        vec4f_norm2(front);
        vec4f_norm2(right);
        vec4f_norm2(up);
        vec4f_norm2(over);
        
        float speedmul = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 3 : 1;
        {
            if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) { vec4f_muls(vec4, front, speedmul * delta); vec4f_sub2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) { vec4f_muls(vec4, front, speedmul * delta); vec4f_add2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) { vec4f_muls(vec4, right, speedmul * delta); vec4f_sub2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) { vec4f_muls(vec4, right, speedmul * delta); vec4f_add2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { vec4f_muls(vec4, up, speedmul * delta); vec4f_sub2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) { vec4f_muls(vec4, up, speedmul * delta); vec4f_add2(campos, vec4); }
            if (glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS) campos[3] -= speedmul * delta;
            if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) campos[3] += speedmul * delta;
        }

        char sign = glfwGetKey(w, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ? -1 : 1;
        if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS) modelrot[0] = fmodf(sign * delta * speedmul + modelrot[0], RAD(360));
        if (glfwGetKey(w, GLFW_KEY_T) == GLFW_PRESS) modelrot[1] = fmodf(sign * delta * speedmul + modelrot[1], RAD(360));
        if (glfwGetKey(w, GLFW_KEY_Y) == GLFW_PRESS) modelrot[2] = fmodf(sign * delta * speedmul + modelrot[2], RAD(360));
        if (glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS) modelrot[3] = fmodf(sign * delta * speedmul + modelrot[3], RAD(360));
        if (glfwGetKey(w, GLFW_KEY_G) == GLFW_PRESS) modelrot[4] = fmodf(sign * delta * speedmul + modelrot[4], RAD(360));
        if (glfwGetKey(w, GLFW_KEY_H) == GLFW_PRESS) modelrot[5] = fmodf(sign * delta * speedmul + modelrot[5], RAD(360));

        // ===========================================
        
        gentransform(mat5, modelpos, modelrot, modelscale);
        glUniform1fv(u_model, 25, mat5);

        mat5f_lookat(mat5, campos, front, right, up, over);
        glUniform1fv(u_view, 25, mat5);

        // ===========================================

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, NULL);
        glfwSwapBuffers(w);
    }

    glfwTerminate();

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

    mat5f_scale(tmp, scale);
    mat5f_mulm2(out, tmp);
}

static void genrotmat(mat5f out, const float euler[6])
{
    mat5f tmp;
    mat5f_rotatexy(out, euler[0]);
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
}

static char getshadercomplog(GLuint shader, char **log)
{
    int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    
    if (len > 0)
    {
        char *buff = malloc(len);
        if (!buff) return 1;
        glGetShaderInfoLog(shader, len, NULL, buff);
        *log = buff;
    }
    else *log = NULL;

    return 0;
}