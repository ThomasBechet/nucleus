#include "texture.h"

#define MAX_TEXTURE_COUNT 32

typedef struct {
    nusr_texture_t **textures;
    uint32_t next_id;
} nusr_asset_texture_data_t;

static nusr_asset_texture_data_t _data;

static nu_result_t create_texture(uint32_t *id, const nu_renderer_texture_create_info_t *info)
{
    /* error check */
    if (_data.next_id >= MAX_TEXTURE_COUNT) return NU_FAILURE;

    /* create texture */
    _data.textures[_data.next_id] = (nusr_texture_t*)nu_malloc(sizeof(nusr_texture_t));
    _data.textures[_data.next_id]->height = info->height;
    _data.textures[_data.next_id]->width = info->width;

    /* allocate memory */
    _data.textures[_data.next_id]->data = (uint32_t*)nu_malloc(sizeof(uint32_t) * info->width * info->height);
    for (uint32_t p = 0; p < info->width * info->height; p++) {
        uint32_t color = ((uint32_t)(info->data[p * 3 + 0]) << 24) + ((uint32_t)(info->data[p * 3 + 1]) << 16) + ((uint32_t)(info->data[p * 3 + 2]) << 8);
        _data.textures[_data.next_id]->data[p] = color;
    }

    /* save id */
    *id = _data.next_id++;

    return NU_SUCCESS;
}
static nu_result_t destroy_texture(uint32_t id)
{
    if (_data.next_id >= MAX_TEXTURE_COUNT) return NU_FAILURE;
    if (!_data.textures[id]) return NU_FAILURE;

    nu_free(_data.textures[id]->data);
    nu_free(_data.textures[id]);
    _data.textures[id] = NULL;

    return NU_SUCCESS;
}

nu_result_t nusr_texture_initialize(void)
{
    _data.next_id = 0;
    _data.textures = (nusr_texture_t**)nu_malloc(sizeof(nusr_texture_t*) * MAX_TEXTURE_COUNT);
    memset(_data.textures, 0, sizeof(nusr_texture_t*) * MAX_TEXTURE_COUNT);

    return NU_SUCCESS;
}
nu_result_t nusr_texture_terminate(void)
{
    for (uint32_t i = 0; i < _data.next_id; i++) {
        if (_data.textures[i]) {
            destroy_texture(i);
        }
    }

    nu_free(_data.textures);

    return NU_SUCCESS;
}

nu_result_t nusr_texture_create(nu_renderer_texture_handle_t *handle, const nu_renderer_texture_create_info_t *info)
{
    uint32_t id;
    nu_result_t result = create_texture(&id, info);
    if (result == NU_SUCCESS) *((uint32_t*)handle) = id;
    return result;
}
nu_result_t nusr_texture_destroy(nu_renderer_texture_handle_t handle)
{
    uint32_t id = (uint64_t)handle;
    return destroy_texture(id);
}
nu_result_t nusr_texture_get(uint32_t id, nusr_texture_t **p)
{
    if (_data.next_id >= MAX_TEXTURE_COUNT) return NU_FAILURE;
    if (!_data.textures[id]) return NU_FAILURE;

    *p = _data.textures[id];

    return NU_SUCCESS;
}