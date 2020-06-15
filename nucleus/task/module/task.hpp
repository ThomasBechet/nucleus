#pragma once

#include "../../core/nucleus_core.h"

extern "C"
{
    /* module */
    NU_API nu_result_t nu_module_get_info(nu_module_info_t *info);

    /* loaders */
    NU_API nu_result_t nu_task_interface_loader(nu_task_interface_t *interface);
}