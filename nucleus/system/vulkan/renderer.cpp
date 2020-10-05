#include "renderer.hpp"

#include "context/engine.hpp"

#include <memory>

using namespace nuvk;

typedef struct {
    std::unique_ptr<Engine> engine;
} nuvk_data_t;

static nuvk_data_t _data;

// static nu_result_t load_glfw_interface(void)
// {
//     nu_module_handle_t window_module = nu_system_window_get_module_handle();
//     if (nu_module_get_id(window_module) != NUGLFW_MODULE_ID) {
//         nu_fatal("Software rasterizer requires GLFW module to work.\n");
//         return NU_FAILURE;
//     }

//     nuglfw_window_interface_loader_pfn_t load_interface;
//     if (nu_module_load_function(window_module, NUGLFW_WINDOW_INTERFACE_LOADER_NAME, (nu_pfn_t*)&load_interface) != NU_SUCCESS) {
//         log_warning("Software rasterizer failed to load glfw loader.\n");
//         return NU_FAILURE;
//     }

//     if (load_interface(&_data.glfw_interface) != NU_SUCCESS) {
//         log_warning("Software rasterizer failed to load glfw interface.\n");
//         return NU_FAILURE;
//     }

//     return NU_SUCCESS;
// }

nu_result_t nuvk::initialize(void)
{
    _data.engine = std::make_unique<Engine>();
    return NU_SUCCESS;
}
nu_result_t nuvk::terminate(void)
{
    _data.engine.reset();
    return NU_SUCCESS;
}
nu_result_t nuvk::render(void)
{
    _data.engine->render();
    return NU_SUCCESS;
}

nu_result_t nuvk::mesh_create(nu_renderer_mesh_handle_t *handle, const nu_renderer_mesh_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::mesh_destroy(nu_renderer_mesh_handle_t handle)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::texture_create(nu_renderer_texture_handle_t *handle, const nu_renderer_texture_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::texture_destroy(nu_renderer_texture_handle_t handle)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::material_create(nu_renderer_material_handle_t *handle, const nu_renderer_material_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::material_destroy(nu_renderer_material_handle_t handle)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::font_create(nu_renderer_font_handle_t *handle, const nu_renderer_font_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::font_destroy(nu_renderer_font_handle_t handle)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::font_get_text_size(nu_renderer_font_handle_t handle, const char *text, uint32_t *width, uint32_t *height)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::camera_create(nu_renderer_camera_handle_t *handle, const nu_renderer_camera_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::camera_destroy(nu_renderer_camera_handle_t handle)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::camera_set_fov(nu_renderer_camera_handle_t handle, float fov)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::camera_set_eye(nu_renderer_camera_handle_t handle, const nu_vec3_t eye)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::camera_set_center(nu_renderer_camera_handle_t handle, const nu_vec3_t center)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::camera_set_active(nu_renderer_camera_handle_t handle)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::staticmesh_create(nu_renderer_staticmesh_handle_t *handle, const nu_renderer_staticmesh_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::staticmesh_destroy(nu_renderer_staticmesh_handle_t handle)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::staticmesh_set_transform(nu_renderer_staticmesh_handle_t handle, const nu_mat4_t transform)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::label_create(nu_renderer_label_handle_t *handle, const nu_renderer_label_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::label_destroy(nu_renderer_label_handle_t handle)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::label_set_position(nu_renderer_label_handle_t handle, int32_t x, int32_t y)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::label_set_text(nu_renderer_label_handle_t handle, const char *text)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::rectangle_create(nu_renderer_rectangle_handle_t *handle, const nu_renderer_rectangle_create_info_t *info)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::rectangle_destroy(nu_renderer_rectangle_handle_t handle)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::rectangle_set_rect(nu_renderer_rectangle_handle_t handle, nu_rect_t rect)
{
    return NU_SUCCESS;
}

nu_result_t nuvk::viewport_set_size(uint32_t width, uint32_t height)
{
    return NU_SUCCESS;
}
nu_result_t nuvk::viewport_get_size(uint32_t *width, uint32_t *height)
{
    return NU_SUCCESS;
}