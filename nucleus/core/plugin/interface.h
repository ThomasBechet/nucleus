#ifndef NU_PLUGIN_INTERFACE_H
#define NU_PLUGIN_INTERFACE_H

#include "../common/common.h"

typedef struct {
    nu_result_t (*initialize)(void);
    nu_result_t (*terminate)(void);
    nu_result_t (*fixed_update)(void);
    nu_result_t (*update)(void);
    nu_result_t (*late_update)(void);
} nu_plugin_interface_t;

#endif