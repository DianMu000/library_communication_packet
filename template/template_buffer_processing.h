#ifndef __TEMPLATE_PACKET_PROCESSING_H
#define __TEMPLATE_PACKET_PROCESSING_H

#define type_order uint8_t
#define order_size 1

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed          long int64_t;

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned          long uint64_t;

const uint8_t table_protocol_type_to_position_data[2] = {//包括0
    3, //protocol_motor
    4, //protocol_RFID
};

const uint8_t table_protocol_type_to_protocol_send_length[2] = {//包括0
    7, //protocol_motor
    9, //protocol_RFID
};

typedef enum buffer_instance_return_t{
    SUCCESS = 0,
    ERROR_BUFFER_NULL = 1,
    ERROR_INTERFACE_NULL = 2,
    ERROR_BUFFER_SIZE  = 3,
}buffer_instance_return_t;

typedef enum pfinit_return_t{
    SUCCESS = 0,
    ERROR_CALL_INTERFACE_INIT = 1,
    ERROR_MALLOC = 2,
}pfinit_return_t;

typedef enum pfdeinit_return_t{
    SUCCESS = 0,
    ERROR_CALL_INTERFACE_DEINIT = 1,
    ERROR_MULTIPLE_FREE = 2,
}pfdeinit_return_t;

typedef enum pfunpack_rec_buffer_return_t{
    SUCCESS = 0,

}pfunpack_rec_buffer_return_t;

typedef enum pfprocess_return_t{
    SUCCESS = 0,

}pfprocess_return_t;

typedef enum pfsend_return_t{
    SUCCESS = 0,

}pfsend_return_t;




struct buffer_t;

/// @brief 下层接口注释结构体
typedef struct interface_t{
    uint8_t (*pfinit) (void);
    uint8_t (*pfdeinit) (void);
    uint8_t (*pfsendbyte) (uint8_t *addr_data, uint8_t data_size);
    uint8_t (*pfget_rec_IDLE_num) (void);//(使用DMA才产生此函数),不使用DMA不产生
    uint8_t (*pfDMA_receive) (void);//(使用DMA才产生此函数),不使用DMA不产生
    uint8_t (*pfread_receive) (void);//(不使用DMA才产生此函数),使用DMA不产生


    struct buffer_t *pinterrupt_global_buffer;//全区定义一个指针，用于在中断函数调用
    //在生成前应该选择是否启用DMA
    
}interface_t;

/// @brief 接收缓冲区结构体
typedef struct receive_buffer_t {
    uint8_t *padd_buffer;
    uint8_t read_index;//read_index为读取数据序号从0开始
    uint8_t write_index;//write_index为接受数据序号从0开始
    uint8_t size;
    uint8_t flag_OF//是否满标志位，1为满，0为非满

}receive_buffer_t;

//DMA:获取多少个
//无DMA：接受中断进入时要转运，计数，

/// @brief 协议类型枚举
typedef enum protocol_type_t{
    protocol_motor = 0,
    protocol_RFID = 1,
}protocol_type_t;


/////////////////////////////////////////////////////////////////
/// @brief 发送电机协议结构体
typedef struct __attribute__((packed)) protocol_send_motor_t{
    //头搜索头 长度或命令 数据 校验




}protocol_send_motor_t;

typedef struct __attribute__((packed)) protocol_send_RFID_t{
    //头搜索头 长度或命令 数据 校验  
    uint8_t head;
    uint8_t id; 
    uint16_t length;
    uint8_t order;
    uint8_t check; //校验位
}protocol_send_RFID_t;

/// @brief 解包完成队列链表成员结构体
typedef struct FIFO_unpacked_lt_t{
    type_order order;
    uint8_t *addr_data;
    struct unpacked_linkedlist_t* next_node;

}FIFO_unpacked_lt_t;

/// @brief 解包完成队列链表结构体
typedef struct FIFO_unpacked_t{
    FIFO_unpacked_lt_t *head;
    FIFO_unpacked_lt_t *rear;
    uint8_t size;
}FIFO_unpacked_t;


typedef struct buffer_t{
    interface_t interface;
    receive_buffer_t receive_buffer;
    FIFO_unpacked_t FIFO_unpacked;

    pfinit_return_t (*pfinit) (buffer_t *self);

    pfdeinit_return_t (*pfdeinit) (buffer_t *self);

    pfunpack_rec_buffer_return_t (*pfunpack_rec_buffer) (buffer_t *self);

    pfprocess_return_t (*pfprocess) (buffer_t *self);

    uint16_t (*pfget_FIFO_lengh) (buffer_t *self);
    
    pfsend_return_t (*pfsend) (buffer_t *self, uint8_t *protocol_send); //配置时设置大小端

}buffer_t;






#endif