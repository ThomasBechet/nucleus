#ifndef NUSR_MODULE_H
#define NUSR_MODULE_H

#include "interface.h"

/* module */
NU_API nu_result_t nu_module_get_info(nu_module_info_t *info);

/* interface */
NU_API nu_result_t nu_renderer_get_interface(nu_renderer_interface_t *interface);
NU_API nu_result_t nusr_renderer_get_interface(nusr_renderer_interface_t *interface);

#endif