// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

#include "floating-graph.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "egl.h"
#include "random.h"
#include "scenes.h"

#ifdef NIGHTMARE_USE_GLES1
#include <GLES/gl.h>
#elif defined NIGHTMARE_USE_GLES2
#include <GLES2/gl2.h>
#endif

static bool initialize();
static void update(int64_t delta_ns);
static void draw();
static void deinitialize();
static inline float *get_x_value(size_t line_index, size_t point_index);
static inline float *get_y_value(size_t line_index, size_t point_index);

struct Scene floating_graph_scene = {
    .name = "Floating graph",
    .initialize = initialize,
    .update = update,
    .draw = draw,
    .deinitialize = deinitialize};

// Parameters
static size_t line_count = 20;
static size_t point_count = 15;
static int64_t time_to_scale = 2l * SEC_IN_NS;
static int64_t scale_interval = 2l * SEC_IN_NS;
static float target_scale = 1.5;
static int64_t time_to_rotate = 5l * SEC_IN_NS;
static int64_t point_add_interval = 100l * MS_IN_NS;
static int64_t rotation_interval = 5l * SEC_IN_NS;
static float rotation_angle = PI / 6;

// Runtime values
static int64_t general_timer;
static int64_t point_add_timer;
static size_t data_size;
static float *data;
static size_t current_count;
static float z_rotation;
static float scale;

#ifdef NIGHTMARE_USE_GLES1
static GLuint vbo;
#elif defined NIGHTMARE_USE_GLES2
// initial ang is 0.0
// column-major order
static float z_rotation_matrix[16] = {
    1.0, // cos(ang)
    0.0, // sin(ang)
    0.0, // const
    0.0, // const

    0.0, // -sin(ang)
    1.0, // cos(ang)
    0.0, // const
    0.0, // const

    0.0, // const
    0.0, // const
    1.0, // const
    0.0, // const

    0.0, // const
    0.0, // const
    0.0, // const
    1.0, // const
};

// initial scale is 1.0
// column-major order
static float scale_matrix[16] = {
    1.0, // scale
    0.0, // const
    0.0, // const
    0.0, // const

    0.0, // const
    1.0, // scale
    0.0, // const
    0.0, // const

    0.0, // const
    0.0, // const
    1.0, // const
    0.0, // const

    0.0, // const
    0.0, // const
    0.0, // const
    1.0, // const
};

static inline void update_z_rotation_matrix()
{
    float s = sinf(z_rotation);
    float c = cosf(z_rotation);

    z_rotation_matrix[0] = c;
    z_rotation_matrix[1] = s;
    z_rotation_matrix[4] = -s;
    z_rotation_matrix[5] = c;
}

static inline void update_scale_matrix()
{
    scale_matrix[0] = scale;
    scale_matrix[5] = scale;
}

static GLchar vertex_shader_source[] =
    "attribute vec2 a_coord;"
    "uniform mat4 u_rotation_matrix;"
    "uniform mat4 u_scale_matrix;"
    "void main()"
    "{"
    "mat4 transformation_matrix = u_rotation_matrix * u_scale_matrix;"
    "gl_Position = transformation_matrix * vec4(a_coord, 0.0, 1.0);"
    "}";

static GLchar fragment_shader_source[] =
    "void main()"
    "{"
    "gl_FragColor = vec4(0.91, 0.77, 0.42, 1.0);"
    "}";

static GLuint shader_program;
static GLuint a_coord = 0;
static GLint u_rotation_matrix;
static GLint u_scale_matrix;
#endif

static bool initialize()
{
    // Reset state
    current_count = 0;
    general_timer = 0;
    point_add_timer = 0;
    z_rotation = 0.0;
    scale = 1.0;
    reset_random();

    // Initialize lines data
    data_size = 2 * line_count * point_count * sizeof(float);
    data = malloc(data_size);

    // Initialize x data (does not change the entire scene)
    for (size_t li = 0; li < line_count; li++)
    {
        for (size_t pi = 0; pi < point_count; pi++)
        {
            float *x = get_x_value(li, pi);

            // Calculate x position on screen (normalized: [-1.0, 1.0])
            *x = 2.0 / ((float)(point_count - 1)) * pi - 1.0;
        }
    }

    // Setup graphics
#ifdef NIGHTMARE_USE_GLES1
    glGenBuffers(1, &vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glColor4f(0.91f, 0.77f, 0.42f, 1.0f);
#elif defined NIGHTMARE_USE_GLES2
    bool success = create_program(&shader_program, vertex_shader_source, fragment_shader_source);
    if (!success)
    {
        print_error("Failed to create GL program for floating graph scene\n");
        return false;
    }

    glBindAttribLocation(shader_program, a_coord, "a_coord");

    success = link_program(shader_program);
    if (!success)
    {
        print_error("Failed to link GL program for floating graph scene\n");
        return false;
    }

    glUseProgram(shader_program);

    u_rotation_matrix = glGetUniformLocation(shader_program, "u_rotation_matrix");
    u_scale_matrix = glGetUniformLocation(shader_program, "u_scale_matrix");

    glEnableVertexAttribArray(a_coord);
#endif

    glViewport(0, 0, screen_width, screen_height);
    glClearColor(0.16f, 0.62f, 0.56f, 1.0f);
    glLineWidth(2.0f);

    return true;
}

static void update(int64_t delta_ns)
{
    point_add_timer -= delta_ns;
    general_timer += delta_ns;

    // Update scale
    if (general_timer >= time_to_scale + scale_interval)
    {
        scale = target_scale;
    }
    else if (general_timer >= time_to_scale)
    {
        scale = 1.0 + ((float)(general_timer - time_to_scale) / (float)scale_interval) * (target_scale - 1.0);
    }
    else
    {
        scale = 1.0;
    }

    // Update rotation
    if (general_timer >= time_to_rotate)
    {
        uint64_t rotation_timer = general_timer % rotation_interval;
        if (rotation_timer >= rotation_interval / 4 * 3)
        {
            z_rotation = -(PI / 6) + ((float)(rotation_timer - rotation_interval / 4 * 3)) / (rotation_interval / 4) * rotation_angle;
        }
        else if (rotation_timer >= rotation_interval / 4)
        {
            z_rotation = (PI / 6) - ((float)(rotation_timer - rotation_interval / 4)) / (rotation_interval / 2) * rotation_angle * 2;
        }
        else
        {
            z_rotation = ((float)rotation_timer) / (rotation_interval / 4) * rotation_angle;
        }
    }

#ifdef NIGHTMARE_USE_GLES2
    update_z_rotation_matrix();
    update_scale_matrix();
#endif

    if (point_add_timer <= 0)
    {
        // Reset timer
        point_add_timer += point_add_interval;

        if (current_count < point_count)
        {
            current_count++;
        }
        else
        {
            for (size_t li = 0; li < line_count; li++)
            {
                for (size_t pi = 0; pi < point_count - 1; pi++)
                {
                    float *y = get_y_value(li, pi);
                    float *ny = get_y_value(li, pi + 1);
                    *y = *ny;
                }
            }
        }

        // Add random point
        for (size_t li = 0; li < line_count; li++)
        {
            float *y = get_y_value(li, current_count - 1);
            float d = get_random_float();
            *y = d * 2.0 - 1.0;
        }
    }
}

static void draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef NIGHTMARE_USE_GLES1
    glVertexPointer(2, GL_FLOAT, 0, NULL);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);

    glPushMatrix();
    glScalef(scale, scale, 1.0);
    glRotatef(z_rotation / PI * 180.0, 0.0, 0.0, 1.0);
#elif defined NIGHTMARE_USE_GLES2
    glVertexAttribPointer(a_coord, 2, GL_FLOAT, GL_FALSE, 0, data);

    glUniformMatrix4fv(u_rotation_matrix, 1, GL_FALSE, z_rotation_matrix);
    glUniformMatrix4fv(u_scale_matrix, 1, GL_FALSE, scale_matrix);
#endif

    for (int li = 0; li < line_count; li++)
    {
        glDrawArrays(GL_LINE_STRIP, li * point_count, current_count);
    }

#ifdef NIGHTMARE_USE_GLES1
    glPopMatrix();
#endif
}

static void deinitialize()
{
    free(data);

    // TODO: reset graphics
}

static inline float *get_x_value(size_t line_index, size_t point_index)
{
    return data + line_index * point_count * 2 + point_index * 2;
}

static inline float *get_y_value(size_t line_index, size_t point_index)
{
    return get_x_value(line_index, point_index) + 1;
}
