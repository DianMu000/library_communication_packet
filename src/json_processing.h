#ifndef __JSON_PROCESSING
#define __JSON_PROCESSING

#include "cJSON.h"

int json_read_from_file(char *file_name, cJSON **ppjson_destination);

int json_free(cJSON *pjson_root);


#endif