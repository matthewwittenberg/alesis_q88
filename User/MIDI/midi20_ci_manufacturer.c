#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "midi_spec.h"
#include "midi20_ci.h"

const uint8_t MAN_ID_HI = 0x00;
const uint8_t MAN_ID_MD = 0x00;
const uint8_t MAN_ID_LO = 0x0E;
const char* PRODUCT_NAME = "ALESIS Q88 MKII - REVERSE ENGINEERED BY MATT WITTENBERG";
const char* PRODUCT_INSTANCE_ID = "0123456789";
const char* FUNCTION_BLOCK_1_NAME = "KEYBOARD";
const char* FUNCTION_BLOCK_2_NAME = "OUT PORT";
const uint16_t PRODUCT_FAMILY = 0;
const uint16_t PRODUCT_MODEL = 88;
const uint32_t PRODUCT_SOFTWARE_VERSION = 1;

MIDI20_CI_RESULT_T midi20_ci_get_prop_manufacturer(
    const char *pheader_data, uint16_t header_data_length,
    char *pproperty_data, uint16_t *pproperty_data_length,
    uint8_t chunk, uint8_t *ptotal_chunks)
{
    if(strnstr(pheader_data, "ResourceList", header_data_length) != NULL)
    {
        strcpy(pproperty_data,
            "[{\"resource\":\"DeviceInfo\"}]");
        *pproperty_data_length = strlen(pproperty_data);
        *ptotal_chunks = 1;

        return MIDI20_CI_RESULT_SUCCESS;
    }
    else if(strnstr(pheader_data, "DeviceInfo", header_data_length) != NULL)
    {
        strcpy(pproperty_data,
            "{\"manufacturerId\":[15,0,0]," \
            "\"manufacturer\":\"ALESIS\"," \
            "\"familyId\":[0,0]," \
            "\"family\":\"keyboard\"," \
            "\"modelId\":[0,0]," \
            "\"model\":\"Q88 MKII\"," \
            "\"versionId\":[0,0,1,0]," \
            "\"version\":\"1.0\"}");
        *pproperty_data_length = strlen(pproperty_data);
        *ptotal_chunks = 1;

        return MIDI20_CI_RESULT_SUCCESS;
    }

    return MIDI20_CI_RESULT_UNKNOWN_PROPERTY;
}



