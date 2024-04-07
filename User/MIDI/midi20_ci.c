#include <midi20_ci.h>
#include <stdlib.h>
#include <string.h>
#include "midi_spec.h"

extern const uint8_t MAN_ID_HI;
extern const uint8_t MAN_ID_MD;
extern const uint8_t MAN_ID_LO;
extern const char* PRODUCT_NAME;
extern const uint16_t PRODUCT_FAMILY;
extern const uint16_t PRODUCT_MODEL;
extern const uint32_t PRODUCT_SOFTWARE_VERSION;
extern const char* PRODUCT_NAME;
extern const char* PRODUCT_INSTANCE_ID;
extern const char* FUNCTION_BLOCK_1_NAME;
extern const char* FUNCTION_BLOCK_2_NAME;

#define CI_MAX_SYSEX_SIZE 128

#pragma pack(push, 1)
typedef struct
{
    uint8_t sysex_start;
    uint8_t universal_sysex;
    uint8_t device_id;
    uint8_t sub_id1;
    uint8_t sub_id2;
    uint8_t ci_version;
    uint8_t source_muid[4];
    uint8_t destination_muid[4];
} MIDI_CI_HEADER_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t manufacturer[3];
    uint8_t family[2];
    uint8_t family_model[2];
    uint8_t software_revision[4];
    uint8_t capability_category;
    uint8_t max_sysex_size[4];
    uint8_t sysex_end;
} MIDI_CI_DISCOVERY_REQUEST_VERSION_1_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t manufacturer[3];
    uint8_t family[2];
    uint8_t family_model[2];
    uint8_t software_revision[4];
    uint8_t capability_category;
    uint8_t max_sysex_size[4];
    uint8_t output_path_id;
    uint8_t sysex_end;
} MIDI_CI_DISCOVERY_REQUEST_VERSION_2_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t manufacturer[3];
    uint8_t family[2];
    uint8_t family_model[2];
    uint8_t software_revision[4];
    uint8_t capability_category;
    uint8_t max_sysex_size[4];
    uint8_t sysex_end;
} MIDI_CI_DISCOVERY_REPLY_VERSION_1_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t manufacturer[3];
    uint8_t family[2];
    uint8_t family_model[2];
    uint8_t software_revision[4];
    uint8_t capability_category;
    uint8_t max_sysex_size[4];
    uint8_t output_path_id;
    uint8_t function_block;
    uint8_t sysex_end;
} MIDI_CI_DISCOVERY_REPLY_VERSION_2_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t status;
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_ENDPOINT_REQUEST_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t status;
    uint8_t length[2];
    char product_id[16];
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_ENDPOINT_REPLY_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t sysex_end;
} MIDI_CI_NAK_REPLY_VERSION_1_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t original_sub_id2;
    uint8_t nak_code;
    uint8_t nak_data;
    uint8_t nak_details[5];
    uint8_t length[2];
    char message[32];
    uint8_t sysex_end;
} MIDI_CI_NAK_REPLY_VERSION_2_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t original_sub_id2;
    uint8_t ack_code;
    uint8_t ack_data;
    uint8_t ack_details[5];
    uint8_t length[2];
    char message[32];
    uint8_t sysex_end;
} MIDI_CI_ACK_REPLY_VERSION_2_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t requets_supported;
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_1_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t requets_supported;
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_2_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t requets_supported;
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_PROP_EX_CAPS_REPLY_VERSION_1_T;

typedef struct
{
    MIDI_CI_HEADER_T header;
    uint8_t requets_supported;
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t sysex_end;
} MIDI_CI_INQUIRY_PROP_EX_CAPS_REPLY_VERSION_2_T;
#pragma pack(pop)

uint32_t _midi_ci_muid;
ci_process_callback _process_callback = NULL;
char _ci_property_buffer[256];

uint32_t bytes_to_uint32(uint8_t *pbytes)
{
    uint32_t value = 0;

    value |= (pbytes[0] << 0) & 0x0000007F;
    value |= (pbytes[1] << 7) & 0x00003F80;
    value |= (pbytes[2] << 14) & 0x001FC000;
    value |= (pbytes[3] << 21) & 0x0FE00000;

    return value;
}

void uint32_to_bytes(uint32_t value, uint8_t *pbytes)
{
    pbytes[0] = (value >> 0) & 0x7F;
    pbytes[1] = (value >> 7) & 0x7F;
    pbytes[2] = (value >> 14) & 0x7F;
    pbytes[3] = (value >> 21) & 0x7F;
}

uint16_t bytes_to_uint16(uint8_t *pbytes)
{
    uint16_t value = 0;

    value |= (pbytes[0] << 0) & 0x0000007F;
    value |= (pbytes[1] << 7) & 0x00003F80;

    return value;
}

void uint16_to_bytes(uint16_t value, uint8_t *pbytes)
{
    pbytes[0] = (value >> 0) & 0x7F;
    pbytes[1] = (value >> 7) & 0x7F;
}

void midi20_ci_init()
{
    _midi_ci_muid = rand();
}

void midi20_ci_build_header(MIDI_CI_HEADER_T *prequest_header, MIDI_CI_HEADER_T *preply_header, uint8_t sub_id2)
{
    preply_header->sysex_start = MIDI_SYSEX_START;
    preply_header->universal_sysex = MIDI20_UNIVERSAL_SYSEX;
    preply_header->device_id = prequest_header->device_id;
    preply_header->sub_id1 = MIDI20_UNIVERSAL_SYSEX_SUBID1_CI;
    preply_header->sub_id2 = sub_id2;
    preply_header->ci_version = prequest_header->ci_version;
    uint32_to_bytes(_midi_ci_muid, preply_header->source_muid);
    memcpy(preply_header->destination_muid, prequest_header->source_muid, 4);
}

void midi20_ci_process_discovery1(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_DISCOVERY_REQUEST_VERSION_1_T *prequest = (MIDI_CI_DISCOVERY_REQUEST_VERSION_1_T*)pmessage;

    MIDI_CI_DISCOVERY_REPLY_VERSION_1_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(&prequest->header, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_DISCOVERY_REPLY);
    reply.manufacturer[0] = MAN_ID_LO;
    reply.manufacturer[1] = MAN_ID_MD;
    reply.manufacturer[2] = MAN_ID_HI;
    uint16_to_bytes(PRODUCT_FAMILY, reply.family);
    uint16_to_bytes(PRODUCT_MODEL, reply.family_model);
    uint32_to_bytes(PRODUCT_SOFTWARE_VERSION, reply.software_revision);
    reply.capability_category = MIDI20_CI_CATEGORY_PROPERTY_EXCHANGE;
    uint32_to_bytes(CI_MAX_SYSEX_SIZE, reply.max_sysex_size);
    reply.sysex_end = MIDI_SYSEX_END;

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_process_discovery2(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_DISCOVERY_REQUEST_VERSION_2_T *prequest = (MIDI_CI_DISCOVERY_REQUEST_VERSION_2_T*)pmessage;

    MIDI_CI_DISCOVERY_REPLY_VERSION_2_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(&prequest->header, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_DISCOVERY_REPLY);
    reply.manufacturer[0] = MAN_ID_LO;
    reply.manufacturer[1] = MAN_ID_MD;
    reply.manufacturer[2] = MAN_ID_HI;
    uint16_to_bytes(PRODUCT_FAMILY, reply.family);
    uint16_to_bytes(PRODUCT_MODEL, reply.family_model);
    uint32_to_bytes(PRODUCT_SOFTWARE_VERSION, reply.software_revision);
    reply.capability_category = MIDI20_CI_CATEGORY_PROPERTY_EXCHANGE;
    uint32_to_bytes(CI_MAX_SYSEX_SIZE, reply.max_sysex_size);
    reply.output_path_id = prequest->output_path_id;
    reply.function_block = 0;
    reply.sysex_end = MIDI_SYSEX_END;

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_process_inquiry_endpoint(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_INQUIRY_ENDPOINT_REQUEST_T *prequest = (MIDI_CI_INQUIRY_ENDPOINT_REQUEST_T*)pmessage;

    MIDI_CI_INQUIRY_ENDPOINT_REPLY_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(&prequest->header, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_ENDPOINT_REPLY);
    reply.status = 0;
    uint16_to_bytes(16, reply.length);
    strncpy(reply.product_id, PRODUCT_NAME, 15);
    reply.sysex_end = MIDI_SYSEX_END;

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_nak1(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_HEADER_T *pheader = (MIDI_CI_HEADER_T*)pmessage;

    MIDI_CI_NAK_REPLY_VERSION_1_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(pheader, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_NAK);

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_nak2(uint8_t *pmessage, uint32_t length, uint8_t status_code, const char *ptext)
{
    MIDI_CI_HEADER_T *pheader = (MIDI_CI_HEADER_T*)pmessage;

    MIDI_CI_NAK_REPLY_VERSION_2_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(pheader, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_NAK);
    reply.original_sub_id2 = pheader->sub_id2;
    reply.nak_code = status_code;
    uint16_to_bytes(32, reply.length);
    strncpy(reply.message, ptext, 31);

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_ack(uint8_t *pmessage, uint32_t length, uint8_t status_code, const char *ptext)
{
    MIDI_CI_HEADER_T *pheader = (MIDI_CI_HEADER_T*)pmessage;

    MIDI_CI_ACK_REPLY_VERSION_2_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(pheader, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_ACK);
    reply.original_sub_id2 = pheader->sub_id2;
    reply.ack_code = status_code;
    uint16_to_bytes(32, reply.length);
    strncpy(reply.message, ptext, 31);

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_process_inquiry_prop_ex_caps1(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_1_T *prequest = (MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_1_T*)pmessage;

    MIDI_CI_INQUIRY_PROP_EX_CAPS_REPLY_VERSION_1_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(&prequest->header, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_CAPS_REPLY);
    reply.requets_supported = 1;
    reply.sysex_end = MIDI_SYSEX_END;

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_process_inquiry_prop_ex_caps2(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_2_T *prequest = (MIDI_CI_INQUIRY_PROP_EX_CAPS_REQUEST_VERSION_2_T*)pmessage;

    MIDI_CI_INQUIRY_PROP_EX_CAPS_REPLY_VERSION_2_T reply;
    memset(&reply, 0, sizeof(reply));
    midi20_ci_build_header(&prequest->header, &reply.header, MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_CAPS_REPLY);
    reply.requets_supported = 1;
    reply.major_version = 0;
    reply.minor_version = 0;
    reply.sysex_end = MIDI_SYSEX_END;

    if(_process_callback)
        _process_callback((uint8_t*)&reply, sizeof(reply));
}

void midi20_ci_reply_prop_get(MIDI_CI_HEADER_T *prequest_header, uint8_t request_id,
    const char *pheader_data, const char *pproperty_data,
    uint16_t chunk, uint16_t total_chunks)
{
    uint16_t header_length = strlen(pheader_data);
    uint16_t property_length = strlen(pproperty_data);
    uint8_t *preply = (uint8_t*)malloc(sizeof(MIDI_CI_HEADER_T) + header_length + property_length + 10);

    if(preply)
    {
        memset(preply, 0, sizeof(MIDI_CI_HEADER_T) + header_length + property_length + 10);
        midi20_ci_build_header(prequest_header, (MIDI_CI_HEADER_T*)preply, MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_GET_REPLY);
        uint32_t index = sizeof(MIDI_CI_HEADER_T);

        // request id
        preply[index] = request_id;
        index++;

        // header length
        uint16_to_bytes(header_length, &preply[index]);
        index += 2;

        // header data
        memcpy(&preply[index], pheader_data, header_length);
        index += header_length;

        // chunks in message
        uint16_to_bytes(total_chunks, &preply[index]);
        index += 2;

        // chunk
        uint16_to_bytes(chunk, &preply[index]);
        index += 2;

        // property length
        uint16_to_bytes(property_length, &preply[index]);
        index += 2;

        // property data
        memcpy(&preply[index], pproperty_data, property_length);
        index += property_length;

        // sysex end
        preply[index++] = MIDI_SYSEX_END;

        if(_process_callback)
            _process_callback(preply, index);

        free(preply);
    }
}

__attribute__((weak)) MIDI20_CI_RESULT_T midi20_ci_get_prop_manufacturer(
    const char *pheader_data, uint16_t header_data_length,
    char *pproperty_data, uint16_t *pproperty_data_length,
    uint16_t chunk, uint16_t *ptotal_chunks)
{
    return MIDI20_CI_RESULT_UNKNOWN_PROPERTY;
}

void midi20_ci_process_inquiry_prop_ex_get(uint8_t *pmessage, uint32_t length)
{
    MIDI_CI_HEADER_T *pheader = (MIDI_CI_HEADER_T*)pmessage;
    uint32_t index = sizeof(MIDI_CI_HEADER_T);
    uint8_t request_id;
    uint16_t header_data_length;
    char *pheader_data;
    uint16_t chunks_in_message;
    uint16_t chunk_number;

    // request id
    request_id = pmessage[index];
    index += 1;

    // header data length
    header_data_length = bytes_to_uint16(&pmessage[index]);
    index += 2;

    // header data
    pheader_data = (char*)&pmessage[index];
    index += header_data_length;

    // chunks in message
    chunks_in_message = bytes_to_uint16(&pmessage[index]);
    index += 2;

    // chunk
    chunk_number = bytes_to_uint16(&pmessage[index]);
    index += 2;

    if(header_data_length == 0)
    {
        if(pheader->ci_version == 1)
            midi20_ci_nak1(pmessage, length);
        else
            midi20_ci_nak2(pmessage, length, MIDI20_CI_NAK_STATUS_CODE_MESSAGE_MALFORMED, "zero length header data");

        return;
    }

    // TODO: validate chunk

    if(strnstr(pheader_data, "resource", header_data_length) != NULL)
    {
        uint16_t prop_length = 0;
        uint16_t total_chunks = 1;
        uint16_t chunk = 0;
        MIDI20_CI_RESULT_T result;

        while(chunk < total_chunks)
        {
            memset(_ci_property_buffer, 0, sizeof(_ci_property_buffer));
            result = midi20_ci_get_prop_manufacturer(pheader_data, header_data_length, _ci_property_buffer, &prop_length, chunk, &total_chunks);

            if(result == MIDI20_CI_RESULT_SUCCESS)
            {
                midi20_ci_reply_prop_get(pheader, request_id, "{\"status\":200}", _ci_property_buffer, chunk+1, total_chunks);
                chunk++;
            }
            else
            {
                if(pheader->ci_version == 1)
                    midi20_ci_nak1(pmessage, length);
                else
                    midi20_ci_nak2(pmessage, length, MIDI20_CI_NAK_STATUS_CODE_CI_MESSAGE_NOT_SUPPORTED, "unsupported resource");

                break;
            }
        }
    }
}

void midi20_ci_process_inquiry_prop_ex_set(uint8_t *pmessage, uint32_t length)
{
}

void midi20_ci_process(uint8_t *pmessage, uint32_t length, ci_process_callback callback)
{
    MIDI_CI_HEADER_T *pheader = (MIDI_CI_HEADER_T*)pmessage;
    _process_callback = callback;

    // min length requirement
    if(length < 15)
        return;

    // validate start
    if(pheader->sysex_start != MIDI_SYSEX_START)
        return;

    // validate universal sysex
    if(pheader->universal_sysex != MIDI20_UNIVERSAL_SYSEX)
        return;

    // verify this is a ci message
    if(pheader->sub_id1 != MIDI20_UNIVERSAL_SYSEX_SUBID1_CI)
        return;

    // process discovery message
    if(pheader->sub_id2 == MIDI20_UNIVERSAL_SYSEX_SUBID2_DISCOVERY)
    {
        if(pheader->ci_version == 1)
            midi20_ci_process_discovery1(pmessage, length);
        else
            midi20_ci_process_discovery2(pmessage, length);
        return;
    }
    // process inquiry endpoint message
    else if(pheader->sub_id2 == MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_ENDPOINT)
    {
        midi20_ci_process_inquiry_endpoint(pmessage, length);
        return;
    }
    // process inquiry property exchange
    if(pheader->sub_id2 == MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_CAPS)
    {
        if(pheader->ci_version == 1)
            midi20_ci_process_inquiry_prop_ex_caps1(pmessage, length);
        else
            midi20_ci_process_inquiry_prop_ex_caps2(pmessage, length);
        return;
    }
    // process inquiry property exchange get
    else if(pheader->sub_id2 == MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_GET)
    {
        midi20_ci_process_inquiry_prop_ex_get(pmessage, length);
        return;
    }
    // process inquiry property exchange set
    else if(pheader->sub_id2 == MIDI20_UNIVERSAL_SYSEX_SUBID2_INQUIRY_PROP_EX_SET)
    {
        midi20_ci_process_inquiry_prop_ex_set(pmessage, length);
        return;
    }
    // process unhandled message with nak
    else
    {
        if(pheader->ci_version == 1)
            midi20_ci_nak1(pmessage, length);
        else
            midi20_ci_nak2(pmessage, length, MIDI20_CI_NAK_STATUS_CODE_CI_MESSAGE_NOT_SUPPORTED, "message not supported");
        return;
    }
}

uint8_t product_name_byte(uint8_t index)
{
	if(index < strlen(PRODUCT_NAME)) {
		return PRODUCT_NAME[index];
	}

	return 0;
}

uint8_t product_instance_id_byte(uint8_t index)
{
	if(index < strlen(PRODUCT_INSTANCE_ID)) {
		return PRODUCT_INSTANCE_ID[index];
	}

	return 0;
}

uint8_t function_block_name_byte(uint8_t index, uint8_t block)
{
	if(block == 0) {
		if(index < strlen(FUNCTION_BLOCK_1_NAME)) {
			return FUNCTION_BLOCK_1_NAME[index];
		}
	}
	else if(block == 1) {
		if(index < strlen(FUNCTION_BLOCK_2_NAME)) {
			return FUNCTION_BLOCK_2_NAME[index];
		}
	}

	return 0;
}

// pmessage contains just the payload bytes, not the status and packet type (14 bytes * n packets)
void midi20_stream_process(uint16_t status, uint8_t *pmessage, uint32_t length, ci_process_callback callback)
{
	uint8_t reply_message[16];

	// endpoint discovery message
	if(status == 0x00)
	{
		uint8_t filter_bitmap = pmessage[2];
		uint8_t ump_version_major = pmessage[1];
		uint8_t ump_version_minor = pmessage[0];

		// todo: look at the ump versions

		// endpoint notification request
		if(filter_bitmap & 0x01)
		{
			reply_message[0] = ump_version_minor;
			reply_message[1] = ump_version_major;
			reply_message[2] = 0x01;	// status
			reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
			reply_message[4] = 0x00;	// no jr timestamps
			reply_message[5] = 0x03;	// we support midi 2.0
			reply_message[6] = 0x00;	// reserved
			reply_message[7] = 0x82;	// static, 2 function blocks
			reply_message[8] = 0x00;	// reserved
			reply_message[9] = 0x00;	// reserved
			reply_message[10] = 0x00;	// reserved
			reply_message[11] = 0x00;	// reserved
			reply_message[12] = 0x00;	// reserved
			reply_message[13] = 0x00;	// reserved
			reply_message[14] = 0x00;	// reserved
			reply_message[15] = 0x00;	// reserved

			callback(reply_message, sizeof(reply_message));
		}

		// device identity notification request
		if(filter_bitmap & 0x02)
		{
			reply_message[0] = 0x00;	// reserved
			reply_message[1] = 0x00;	// reserved
			reply_message[2] = 0x02;	// status
			reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
			reply_message[4] = MAN_ID_LO;
			reply_message[5] = MAN_ID_MD;
			reply_message[6] = MAN_ID_HI;
			reply_message[7] = 0x00;	// reserved
			reply_message[8] = PRODUCT_MODEL >> 8;
			reply_message[9] = PRODUCT_MODEL;
			reply_message[10] = PRODUCT_FAMILY>> 8;
			reply_message[11] = PRODUCT_FAMILY;
			reply_message[12] = PRODUCT_SOFTWARE_VERSION >> 24;
			reply_message[13] = PRODUCT_SOFTWARE_VERSION >> 16;
			reply_message[14] = PRODUCT_SOFTWARE_VERSION >> 8;
			reply_message[15] = PRODUCT_SOFTWARE_VERSION >> 0;

			callback(reply_message, sizeof(reply_message));
		}

		// product name notification request
		if(filter_bitmap & 0x04)
		{
			int32_t packet_count = strlen(PRODUCT_NAME) / 14;
			int32_t string_index = 0;

			if(strlen(PRODUCT_NAME) % 14) {
				packet_count++;
			}

			for(int32_t i=0; i<packet_count; i++)
			{
				reply_message[0] = product_name_byte(string_index + 1);
				reply_message[1] = product_name_byte(string_index + 0);
				reply_message[2] = 0x03;	// status
				reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
				reply_message[4] = product_name_byte(string_index + 5);
				reply_message[5] = product_name_byte(string_index + 4);
				reply_message[6] = product_name_byte(string_index + 3);
				reply_message[7] = product_name_byte(string_index + 2);
				reply_message[8] = product_name_byte(string_index + 9);
				reply_message[9] = product_name_byte(string_index + 8);
				reply_message[10] = product_name_byte(string_index + 7);
				reply_message[11] = product_name_byte(string_index + 6);
				reply_message[12] = product_name_byte(string_index + 13);
				reply_message[13] = product_name_byte(string_index + 12);
				reply_message[14] = product_name_byte(string_index + 11);
				reply_message[15] = product_name_byte(string_index + 10);

				if(packet_count > 1)
				{
					if(i == 0) {
						reply_message[3] |= MIDI20_STREAM_STATUS_START;
					}
					else if(i == packet_count - 1) {
						reply_message[3] |= MIDI20_STREAM_STATUS_STOP;
					}
					else {
						reply_message[3] |= MIDI20_STREAM_STATUS_CONTINUE;
					}
				}

				callback(reply_message, sizeof(reply_message));

				string_index += 14;
			}
		}

		// product instance id notification request
		if(filter_bitmap & 0x08)
		{
			int32_t packet_count = strlen(PRODUCT_INSTANCE_ID) / 14;
			int32_t string_index = 0;

			if(strlen(PRODUCT_INSTANCE_ID) % 14) {
				packet_count++;
			}

			for(int32_t i=0; i<packet_count; i++)
			{
				reply_message[0] = product_instance_id_byte(string_index + 1);
				reply_message[1] = product_instance_id_byte(string_index + 0);
				reply_message[2] = 0x04;	// status
				reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
				reply_message[4] = product_instance_id_byte(string_index + 5);
				reply_message[5] = product_instance_id_byte(string_index + 4);
				reply_message[6] = product_instance_id_byte(string_index + 3);
				reply_message[7] = product_instance_id_byte(string_index + 2);
				reply_message[8] = product_instance_id_byte(string_index + 9);
				reply_message[9] = product_instance_id_byte(string_index + 8);
				reply_message[10] = product_instance_id_byte(string_index + 7);
				reply_message[11] = product_instance_id_byte(string_index + 6);
				reply_message[12] = product_instance_id_byte(string_index + 13);
				reply_message[13] = product_instance_id_byte(string_index + 12);
				reply_message[14] = product_instance_id_byte(string_index + 11);
				reply_message[15] = product_instance_id_byte(string_index + 10);

				if(packet_count > 1)
				{
					if(i == 0) {
						reply_message[3] |= MIDI20_STREAM_STATUS_START;
					}
					else if(i == packet_count - 1) {
						reply_message[3] |= MIDI20_STREAM_STATUS_STOP;
					}
					else {
						reply_message[3] |= MIDI20_STREAM_STATUS_CONTINUE;
					}
				}

				callback(reply_message, sizeof(reply_message));

				string_index += 14;
			}
		}

		// stream configuration notification request
		if(filter_bitmap & 0x10)
		{
			reply_message[0] = 0x00;	// no jr timestamps
			reply_message[1] = 0x02;	// midi 2.0 protocol
			reply_message[2] = 0x06;	// status
			reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
			reply_message[4] = 0x00;	// reserved
			reply_message[5] = 0x00;	// reserved
			reply_message[6] = 0x00;	// reserved
			reply_message[7] = 0x00;	// reserved
			reply_message[8] = 0x00;	// reserved
			reply_message[9] = 0x00;	// reserved
			reply_message[10] = 0x00;	// reserved
			reply_message[11] = 0x00;	// reserved
			reply_message[12] = 0x00;	// reserved
			reply_message[13] = 0x00;	// reserved
			reply_message[14] = 0x00;	// reserved
			reply_message[15] = 0x00;	// reserved

			callback(reply_message, sizeof(reply_message));
		}
	}

	// stream configuration request
	if(status == 0x05)
	{
		// ignore the request and send back our configuration for now

		reply_message[0] = 0x00;	// no jr timestamps
		reply_message[1] = 0x02;	// midi 2.0 protocol
		reply_message[2] = 0x06;	// status
		reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
		reply_message[4] = 0x00;	// reserved
		reply_message[5] = 0x00;	// reserved
		reply_message[6] = 0x00;	// reserved
		reply_message[7] = 0x00;	// reserved
		reply_message[8] = 0x00;	// reserved
		reply_message[9] = 0x00;	// reserved
		reply_message[10] = 0x00;	// reserved
		reply_message[11] = 0x00;	// reserved
		reply_message[12] = 0x00;	// reserved
		reply_message[13] = 0x00;	// reserved
		reply_message[14] = 0x00;	// reserved
		reply_message[15] = 0x00;	// reserved

		callback(reply_message, sizeof(reply_message));
	}

	// function block discovery request
	if(status == 0x10)
	{
		uint8_t function_block = pmessage[1];
		uint8_t filter = pmessage[0];

		// keyboard block
		if((function_block == 0xFF) || (function_block == 0))
		{
			// info notification
			if(filter & 0x01)
			{
				reply_message[0] = 0x33;	// bidirectional, ui sender & receiver, not midi 1.0
				reply_message[1] = 0x80;	// active functional block 0
				reply_message[2] = 0x11;	// status
				reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
				reply_message[4] = 0x00;	// no sysex8 streams
				reply_message[5] = 0x01;	// format version
				reply_message[6] = 0x01;	// groups spanned
				reply_message[7] = 0x00;	// first group
				reply_message[8] = 0x00;	// reserved
				reply_message[9] = 0x00;	// reserved
				reply_message[10] = 0x00;	// reserved
				reply_message[11] = 0x00;	// reserved
				reply_message[12] = 0x00;	// reserved
				reply_message[13] = 0x00;	// reserved
				reply_message[14] = 0x00;	// reserved
				reply_message[15] = 0x00;	// reserved

				callback(reply_message, sizeof(reply_message));
			}

			if(filter & 0x02)
			{
				int32_t packet_count = strlen(FUNCTION_BLOCK_1_NAME) / 13;
				int32_t string_index = 0;

				if(strlen(FUNCTION_BLOCK_1_NAME) % 13) {
					packet_count++;
				}

				for(int32_t i=0; i<packet_count; i++)
				{
					reply_message[0] = function_block_name_byte(string_index + 0, 0);
					reply_message[1] = 0x00;	// functional block 0
					reply_message[2] = 0x12;	// status
					reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
					reply_message[4] = function_block_name_byte(string_index + 4, 0);
					reply_message[5] = function_block_name_byte(string_index + 3, 0);
					reply_message[6] = function_block_name_byte(string_index + 2, 0);
					reply_message[7] = function_block_name_byte(string_index + 1, 0);
					reply_message[8] = function_block_name_byte(string_index + 8, 0);
					reply_message[9] = function_block_name_byte(string_index + 7, 0);
					reply_message[10] = function_block_name_byte(string_index + 6, 0);
					reply_message[11] = function_block_name_byte(string_index + 5, 0);
					reply_message[12] = function_block_name_byte(string_index + 12, 0);
					reply_message[13] = function_block_name_byte(string_index + 11, 0);
					reply_message[14] = function_block_name_byte(string_index + 10, 0);
					reply_message[15] = function_block_name_byte(string_index + 9, 0);

					if(packet_count > 1)
					{
						if(i == 0) {
							reply_message[3] |= MIDI20_STREAM_STATUS_START;
						}
						else if(i == packet_count - 1) {
							reply_message[3] |= MIDI20_STREAM_STATUS_STOP;
						}
						else {
							reply_message[3] |= MIDI20_STREAM_STATUS_CONTINUE;
						}
					}

					callback(reply_message, sizeof(reply_message));

					string_index += 13;
				}
			}
		}

		// output port block
		if((function_block == 0xFF) || (function_block == 1))
		{
			// info notification
			if(filter & 0x01)
			{
				reply_message[0] = 0x2A;	// tx only, ui sender, midi 1.0 restricted bandwidth
				reply_message[1] = 0x81;	// active functional block 1
				reply_message[2] = 0x11;	// status
				reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
				reply_message[4] = 0x00;	// no sysex8 streams
				reply_message[5] = 0x01;	// format version
				reply_message[6] = 0x01;	// groups spanned
				reply_message[7] = 0x01;	// first group
				reply_message[8] = 0x00;	// reserved
				reply_message[9] = 0x00;	// reserved
				reply_message[10] = 0x00;	// reserved
				reply_message[11] = 0x00;	// reserved
				reply_message[12] = 0x00;	// reserved
				reply_message[13] = 0x00;	// reserved
				reply_message[14] = 0x00;	// reserved
				reply_message[15] = 0x00;	// reserved

				callback(reply_message, sizeof(reply_message));
			}

			if(filter & 0x02)
			{
				int32_t packet_count = strlen(FUNCTION_BLOCK_2_NAME) / 13;
				int32_t string_index = 0;

				if(strlen(FUNCTION_BLOCK_2_NAME) % 13) {
					packet_count++;
				}

				for(int32_t i=0; i<packet_count; i++)
				{
					reply_message[0] = function_block_name_byte(string_index + 0, 1);
					reply_message[1] = 0x01;	// functional block 1
					reply_message[2] = 0x12;	// status
					reply_message[3] = MIDI20_MESSAGE_TYPE_STREAM;
					reply_message[4] = function_block_name_byte(string_index + 4, 1);
					reply_message[5] = function_block_name_byte(string_index + 3, 1);
					reply_message[6] = function_block_name_byte(string_index + 2, 1);
					reply_message[7] = function_block_name_byte(string_index + 1, 1);
					reply_message[8] = function_block_name_byte(string_index + 8, 1);
					reply_message[9] = function_block_name_byte(string_index + 7, 1);
					reply_message[10] = function_block_name_byte(string_index + 6, 1);
					reply_message[11] = function_block_name_byte(string_index + 5, 1);
					reply_message[12] = function_block_name_byte(string_index + 12, 1);
					reply_message[13] = function_block_name_byte(string_index + 11, 1);
					reply_message[14] = function_block_name_byte(string_index + 10, 1);
					reply_message[15] = function_block_name_byte(string_index + 9, 1);

					if(packet_count > 1)
					{
						if(i == 0) {
							reply_message[3] |= MIDI20_STREAM_STATUS_START;
						}
						else if(i == packet_count - 1) {
							reply_message[3] |= MIDI20_STREAM_STATUS_STOP;
						}
						else {
							reply_message[3] |= MIDI20_STREAM_STATUS_CONTINUE;
						}
					}

					callback(reply_message, sizeof(reply_message));

					string_index += 13;
				}
			}
		}
	}
}
