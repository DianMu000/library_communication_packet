#include "json_processing.h"
#include "stdio.h"
#include "stdlib.h"
#include "cJSON.h"

/// @brief 读取json文件到str_buf
/// @param file_name 文件名称，应包含json后缀
/// @param pstr_destination 
/// @return 0:成功 1：找不到文件 2：读取失败 3：解析json失败
int json_read_from_file(char *file_name, cJSON **ppjson_destination){
    FILE *pjson_file;

    //读文件
    pjson_file = fopen(file_name, "rb+");
    if(pjson_file == NULL){
        printf("找不到%s文件,请确保此文件在.exe同级目录下\n", file_name);
            fclose(pjson_file);
            return 1;
        }
    else{
        printf("已找到%s...\n", file_name);
    }

    //读文件大小
    fseek(pjson_file, 0, SEEK_END);
    long size_json_file = ftell(pjson_file);
    fseek(pjson_file, 0, SEEK_SET);

    //读取文件到字符串
    char *pbuf_str_json = malloc(size_json_file); 
    int result = fread(pbuf_str_json, size_json_file, 1, pjson_file);
    if(result == 1){
        printf("读取user_config.json成功...\n");
    }
    else{
        printf("读取user_config.json失败...\n");
        free(pbuf_str_json);
        fclose(pjson_file);
        return 2;
    }

    //解析json
    cJSON* pjson_root;
    pjson_root = cJSON_Parse(pbuf_str_json);
    if(pjson_root == NULL){
        printf("解析json失败...\n");
            free(pbuf_str_json);
            fclose(pjson_file);
            return 3;

    }
    else{
        printf("解析json成功...\n");
    }

    free(pbuf_str_json);
    //结束释放文件内存
    *ppjson_destination = pjson_root;
    fclose(pjson_file);

    return 0;
}

int json_free(cJSON *pjson_root){
    cJSON_Delete(pjson_root);
    return 0;
}



