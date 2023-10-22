#include "common.h"

#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

int print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stdout, format, args);
    va_end(args);

    return result;
}

int print_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stderr, format, args);
    va_end(args);

    return result;
}

// https://embeddedartistry.com/blog/2018/07/12/simple-fixed-point-conversion-in-c/
float from_fixed(int32_t value)
{
    return ((float)value / (float)(1 << 16));
}

int32_t to_fixed16(float value)
{
    return (int32_t)(round(value * (1 << 16)));
}

// https://stackoverflow.com/a/64896093
int64_t difftimespec_ns(const struct timespec after, const struct timespec before)
{
    return ((int64_t)after.tv_sec - (int64_t)before.tv_sec) * (int64_t)1000000000 + ((int64_t)after.tv_nsec - (int64_t)before.tv_nsec);
}

/*
 * Shader compilation
 */

#ifdef NIGHTMARE_USE_GLES2
#include <GLES2/gl2.h>

bool create_program(GLuint *program, const GLchar *vertex_source, const GLchar *fragment_source)
{
    // Load vertex shader
    GLuint vertex_shader = load_shader(vertex_source, GL_VERTEX_SHADER);

    if (!vertex_shader)
    {
        print_error("Failed to compile vertex shader\n");
        return false;
    }

    // Load fragment shader
    GLuint fragment_shader = load_shader(fragment_source, GL_FRAGMENT_SHADER);

    if (!fragment_shader)
    {
        print_error("Failed to compile fragment shader\n");
        return false;
    }

    GLuint _program = glCreateProgram();
    if (!program)
    {
        print_error("Failed to create program\n");
        return false;
    }

    glAttachShader(_program, vertex_shader);
    glAttachShader(_program, fragment_shader);

    (*program) = _program;

    return true;
}

bool link_program(GLuint program)
{

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint info_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_length);

        if (info_length > 1)
        {
            char *info_log = malloc(sizeof(char) * info_length);
            glGetProgramInfoLog(program, info_length, NULL, info_log);
            print_error("Error linking program:\n%s\n", info_log);
            free(info_log);
        }

        glDeleteProgram(program);

        return false;
    }

    return true;
}

GLuint load_shader(const GLchar *shader_source, GLenum type)
{
    GLuint shader = glCreateShader(type);

    if (shader == 0)
    {
        print_error("Failed to create shader object\n");
        return 0;
    }

    glShaderSource(shader, 1, &shader_source, NULL);

    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        print_error("Compilation of shader failed\n");

        GLint info_length = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);

        if (info_length > 1)
        {
            char *info_log = malloc(sizeof(char) * info_length);
            glGetShaderInfoLog(shader, info_length, NULL, info_log);
            print_error("Error compiling shader:\n%s\n", info_log);
            free(info_log);
        }

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}
#endif