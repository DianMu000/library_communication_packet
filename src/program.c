#include "CJSON.h"
#include "json_processing.h"
#include "stdlib.h"
#include <stdio.h>
#include "cJSON.h"

void program_end(void);

void main(void){
    //读文件并解析
    cJSON *pjson_conf_root;
    if(json_read_from_file("user_config.json", &pjson_conf_root) != 0){
        program_end();
    }

    //获得数据段数量
    int json_obj_count = json_get_count((const cJSON *)pjson_conf_root);
    if(json_obj_count == 0){
        json_free(pjson_conf_root);
        program_end();
    };
    //循环解析元素
    
    

    json_free(pjson_conf_root);
    // 模板生成代码
    
    
    
    
     

}



void program_end(void){
    printf("输入任意键...");
    getchar();
    exit(1);
}

