#include "gui.h"

#include "render.h"

#define MAX_LABEL_COUNT 512

typedef struct {
    uint32_t label_count;
    nusr_label_t *labels;
} nusr_gui_data_t;

static nusr_gui_data_t _data;

nu_result_t nusr_gui_initialize(void)
{
    /* label */
    _data.label_count = 0;
    _data.labels = (nusr_label_t*)nu_malloc(sizeof(nusr_label_t) * MAX_LABEL_COUNT);
    for (uint32_t i = 0; i < MAX_LABEL_COUNT; i++) {
        _data.labels[i].active = false;
    }

    return NU_SUCCESS;    
}
nu_result_t nusr_gui_terminate(void)
{
    nu_free(_data.labels);

    return NU_SUCCESS;
}
nu_result_t nusr_gui_render(nusr_framebuffer_t *color_buffer)
{
    /* draw labels */
    for (uint32_t id = 0; id < _data.label_count; id++) {
        if (_data.labels[id].active) {
            nusr_font_t *font;
            nusr_font_get(_data.labels[id].font, &font);
            nusr_gui_render_label(color_buffer, _data.labels[id].x, _data.labels[id].y, font, _data.labels[id].text);
        }
    }

    return NU_SUCCESS;
}

nu_result_t nusr_gui_label_create(nu_renderer_label_handle_t *handle, const nu_renderer_label_create_info_t *info)
{
    uint32_t found_id = MAX_LABEL_COUNT;
    for (uint32_t i = 0; i < _data.label_count; i++) {
        if (!_data.labels[i].active) {
            found_id = i;
            break;
        }
    }

    if (found_id == MAX_LABEL_COUNT) {
        if (_data.label_count >= MAX_LABEL_COUNT) return NU_FAILURE;
        found_id = _data.label_count;
        _data.label_count++;
    }

    _data.labels[found_id].active = true;
    _data.labels[found_id].x = info->x;
    _data.labels[found_id].y = info->y;
    _data.labels[found_id].font = (uint64_t)info->font;
    strncpy(_data.labels[found_id].text, info->text, NUSR_MAX_LABEL_TEXT_SIZE - 1); 

    *((uint32_t*)handle) = found_id;

    return NU_SUCCESS;
}
nu_result_t nusr_gui_label_destroy(nu_renderer_label_handle_t handle)
{
    uint32_t id = (uint64_t)handle;

    if (!_data.labels[id].active) return NU_FAILURE;

    _data.labels[id].active = false;

    return NU_SUCCESS;
}
nu_result_t nusr_gui_label_set_position(nu_renderer_label_handle_t handle, int32_t x, int32_t y)
{
    uint32_t id = (uint64_t)handle;

    if (!_data.labels[id].active) return NU_FAILURE;

    _data.labels[id].x = x;
    _data.labels[id].y = y;

    return NU_SUCCESS;
}
nu_result_t nusr_gui_label_set_text(nu_renderer_label_handle_t handle, const char *text)
{
    uint32_t id = (uint64_t)handle;

    if (!_data.labels[id].active) return NU_FAILURE;

    strncpy(_data.labels[id].text, text, NUSR_MAX_LABEL_TEXT_SIZE - 1);

    return NU_SUCCESS;
}