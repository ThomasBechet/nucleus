#include "surface.h"

#include "../common/logger.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    GLuint quad_shader;
    GLuint texture;
    GLuint vertices_vbo;
    GLuint uvs_vbo;
    GLuint vao;
} surface_data_t;

static surface_data_t _data;

static const char *quad_vertex = " \
    #version 330 core \n\
    layout(location = 0) in vec2 vertex; \n\
    layout(location = 1) in vec2 uv; \n\
    out vec2 tex_coord; \n\
    void main() { \n\
        tex_coord = uv; \n\
        gl_Position = vec4(vertex, 0, 1); \n\
    }  \
";
static const char *quad_fragment = " \
    #version 330 core \n\
    in vec2 tex_coord; \n\
    uniform sampler2D image; \n\
    out vec4 color; \n\
    void main() { \n\
        color = texture(image, tex_coord); \n\
    }  \
";

static const float vertices[] = {
    -1, -1,
     1, -1,
     1,  1,
     1,  1,
    -1,  1,
    -1, -1
};
static const float uvs[] = {
    0, 0,
    1, 0,
    1, 1,
    1, 1,
    0, 1,
    0, 0
};

static void create_quad_shader(void)
{
    GLuint vertex_shader, fragment_shader;
    GLint success;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &quad_vertex, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        nu_warning(NUGLFW_LOGGER_NAME"Failed to compile quad vertex shader.\n");
        glDeleteShader(vertex_shader);
    }
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &quad_fragment, 0);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        nu_warning(NUGLFW_LOGGER_NAME"Failed to compile quad fragment shader.\n");
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    _data.quad_shader = glCreateProgram();
    glAttachShader(_data.quad_shader, vertex_shader);
    glAttachShader(_data.quad_shader, fragment_shader);

    glLinkProgram(_data.quad_shader);
    glGetProgramiv(_data.quad_shader, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        nu_warning(NUGLFW_LOGGER_NAME"Failed to link quad shader.\n");
        glDeleteProgram(_data.quad_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}
static void destroy_quad_shader(void)
{
    glDeleteProgram(_data.quad_shader);
}

static void create_vao(void)
{
    glGenVertexArrays(1, &_data.vao);
    glBindVertexArray(_data.vao);

    glGenBuffers(1, &_data.vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _data.vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_data.uvs_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _data.uvs_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
}
static void destroy_vao(void)
{
    glDeleteBuffers(1, &_data.vertices_vbo);
    glDeleteBuffers(1, &_data.uvs_vbo);
    glDeleteVertexArrays(1, &_data.vao);
}

static void create_texture(void)
{
    glGenTextures(1, &_data.texture);
    glBindTexture(GL_TEXTURE_2D, _data.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _data.width, _data.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}
static void destroy_texture(void)
{
    glDeleteTextures(1, &_data.texture);
}

nu_result_t nuglfw_surface_create(void)
{
    _data.width = 0;
    _data.height = 0;

    create_quad_shader();
    create_vao();
    create_texture();

    return NU_SUCCESS;
}
nu_result_t nuglfw_surface_destroy(void)
{
    destroy_texture();
    destroy_vao();
    destroy_quad_shader();

    return NU_SUCCESS;
}
nu_result_t nuglfw_surface_draw(
    uint32_t width,
    uint32_t height,
    void *pixels
)
{
    /* check resize */
    if (width != _data.width || height != _data.height) {
        _data.width = width;
        _data.height = height;
        destroy_texture();
        create_texture();
    }

    /* update texture */
    glBindTexture(GL_TEXTURE_2D, _data.texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _data.width, _data.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels);

    /* render texture */
    glUseProgram(_data.quad_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _data.texture);
    glBindVertexArray(_data.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    return NU_SUCCESS;
}