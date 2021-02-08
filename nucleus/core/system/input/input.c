#include "input.h"

#include "../../config/interface.h"
#include "../../memory/interface.h"
#include "../../logger/interface.h"

#define NU_LOGGER_INPUT_NAME "[INPUT] "

typedef struct {
    nu_module_handle_t module;
    nu_input_interface_t interface;
} nu_system_input_t;

static nu_system_input_t _system;

nu_result_t nu_system_input_initialize(void)
{
    nu_result_t result;
    result = NU_SUCCESS;

    nu_input_api_t api = nu_config_get().input.api;

    /* load module */
    if (api == NU_INPUT_API_GLFW) { /* use existing glfw window module */
        _system.module = nu_window_get_module_handle(); /* copy module handle */
    } else {
        nu_warning(NU_LOGGER_INPUT_NAME"Unsupported api.\n");
        return NU_FAILURE;
    }

    /* load input interface */
    result = nu_module_load_interface(_system.module, NU_INPUT_INTERFACE_NAME, &_system.interface);
    if (result != NU_SUCCESS) {
        nu_warning(NU_LOGGER_INPUT_NAME"Failed to load interface.\n");
        return result;
    }

    /* initialize input system */
    result = _system.interface.initialize();
    if (result != NU_SUCCESS) {
        nu_warning(NU_LOGGER_INPUT_NAME"Failed to initialize input system.\n");
        return result;
    }

    return NU_SUCCESS;
}
nu_result_t nu_system_input_terminate(void)
{
    /* terminate input system */
    _system.interface.terminate();

    return NU_SUCCESS;
}
nu_result_t nu_system_input_update(void)
{
    _system.interface.update();
    return NU_SUCCESS;
}

nu_module_handle_t nu_input_get_module_handle(void)
{
    return _system.module;
}

nu_result_t nu_input_get_keyboard_state(nu_button_state_t *state, nu_keyboard_t button)
{
    return _system.interface.get_keyboard_state(state, button);
}
nu_result_t nu_input_get_keyboard_text(const char **text, uint32_t *length)
{
    return _system.interface.get_keyboard_text(text, length);
}
nu_result_t nu_input_get_mouse_state(nu_button_state_t *state, nu_mouse_t button)
{
    return _system.interface.get_mouse_state(state, button);
}
nu_result_t nu_input_get_mouse_motion(nu_vec2f_t motion)
{
    return _system.interface.get_mouse_motion(motion);
}
nu_result_t nu_input_get_mouse_scroll(nu_vec2f_t scroll)
{
    return _system.interface.get_mouse_scroll(scroll);
}
nu_result_t nu_input_get_cursor_mode(nu_cursor_mode_t *mode)
{
    return _system.interface.get_cursor_mode(mode);
}
nu_result_t nu_input_set_cursor_mode(nu_cursor_mode_t mode)
{
    return _system.interface.set_cursor_mode(mode);
}