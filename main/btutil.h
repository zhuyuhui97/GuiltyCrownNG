#ifndef __BTUTIL_H__
#define __BTUTIL_H__

#include "esp_system.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    IDX_CHAR_GCCTRL,
    IDX_CHAR_VAL_GCCTRL,

    HRS_IDX_NB,
};




esp_err_t init_bt();
esp_err_t init_nvs();

#endif