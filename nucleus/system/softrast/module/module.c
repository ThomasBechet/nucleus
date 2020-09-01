#include "module.h"

#include "../softrast.h"
#include "../asset/mesh.h"
#include "../scene/scene.h"

static const uint32_t plugin_count = 0;
static const char *plugins[] = {};

nu_result_t nu_module_get_info(nu_module_info_t *info)
{
    info->id = NUSR_MODULE_ID;
    info->flags = NU_MODULE_FLAG_TYPE_RENDERER;
    info->plugin_count = plugin_count;
    info->plugins = plugins;

    return NU_SUCCESS;
}

nu_result_t nu_renderer_get_interface(nu_renderer_interface_t *interface)
{
    interface->initialize = nusr_initialize;
    interface->terminate  = nusr_terminate;
    interface->render     = nusr_render;

    return NU_SUCCESS;
}
nu_result_t nusr_renderer_get_interface(nusr_renderer_interface_t *interface)
{
    interface->mesh_create  = nusr_mesh_create;
    interface->mesh_destroy = nusr_mesh_destroy;
    
    interface->camera_set_fov    = nusr_scene_camera_set_fov;
    interface->camera_set_eye    = nusr_scene_camera_set_eye;
    interface->camera_set_center = nusr_scene_camera_set_center;

    interface->staticmesh_create        = nusr_scene_staticmesh_create;
    interface->staticmesh_destroy       = nusr_scene_staticmesh_destroy;
    interface->staticmesh_set_transform = nusr_scene_staticmesh_set_transform;
    
    return NU_SUCCESS;
}