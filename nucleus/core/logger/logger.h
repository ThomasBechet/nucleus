#ifndef NU_LOGGER_H
#define NU_LOGGER_H

#include "interface.h"

nu_result_t nu_logger_initialize(void);
nu_result_t nu_logger_terminate(void);
nu_result_t nu_logger_start(void);
nu_result_t nu_logger_stop(void);

#endif