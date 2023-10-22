#pragma once

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

int print(const char *format, ...);
int print_error(const char *format, ...);
int64_t difftimespec_ns(const struct timespec after, const struct timespec before);
float from_fixed(int32_t value);
int32_t to_fixed16(float value);
#define MS_IN_NS (uint64_t)1000000l
#define SEC_IN_NS (uint64_t)1000000000l
#define PI 3.14159265359

#ifdef NIGHTMARE_USE_GLES2
#include <GLES2/gl2.h>

bool create_program(GLuint *program, const GLchar *vertex_source, const GLchar *fragment_source);
bool link_program(GLuint program);
GLuint load_shader(const GLchar *shader_source, GLenum type);
#endif