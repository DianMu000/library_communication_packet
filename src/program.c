#include "CJSON.h"
#include "json_processing.h"
#include "stdlib.h"
#include <stdio.h>
#include "cJSON.h"
int main(void){

    cJSON *pjson_conf_root;
    json_read_from_file("user_config.json", &pjson_conf_root);
    json_free(pjson_conf_root);



    printf("输入任意键...");
    getchar();
    return 0;
}
