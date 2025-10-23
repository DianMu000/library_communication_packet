#include "template_buffer_processing.h"
#include <stdlib.h>



buffer_instance_return_t buffer_instance(buffer_t *buffer, interface_t* interface, uint16_t rec_buffer_size){

    if(buffer == NULL)          return ERROR_BUFFER_NULL;
    if(interface == NULL )      return ERROR_INTERFACE_NULL;
    if(rec_buffer_size == 0)    return ERROR_BUFFER_SIZE;   

    
    buffer->interface.pfinit = interface->pfdeinit;
    buffer->interface.pfdeinit = interface->pfdeinit;
    buffer->interface.pfsendbyte = interface->pfsendbyte;
    buffer->interface.pfget_rec_IDLE_num = interface->pfget_rec_IDLE_num;//(使用DMA才产生此函数),不使用DMA不产生
    buffer-> interface.pfDMA_receive = interface->pfDMA_receive;//(使用DMA才产生此函数),不使用DMA不产生
    buffer-> interface.pfread_receive = interface->pfread_receive;//(不使用DMA才产生此函数),使用DMA不产生

    buffer->interface.pinterrupt_global_buffer = &buffer;

    
    buffer->receive_buffer.padd_buffer = NULL;
    buffer->receive_buffer.size = rec_buffer_size;
    buffer->receive_buffer.write_index = 0;
    buffer->receive_buffer.read_index = 0;

    buffer->FIFO_unpacked.head = NULL;
    buffer->FIFO_unpacked.rear = NULL;
    
    buffer->pfinit = init;
    buffer->pfdeinit = deinit;
    buffer->pfunpack_rec_buffer = 
    buffer->pfprocess = 
    buffer->pfget_FIFO_lengh = 
    buffer->pfsend = 

    return SUCCESS;
}

static pfinit_return_t init(buffer_t* self){
    if((self->interface.pfinit()) != 0){
        return ERROR_CALL_INTERFACE_INIT;//调用函数接口失败
    }
    if((self->receive_buffer.padd_buffer = (uint8_t*)malloc(self->receive_buffer.size)) == NULL){
        return ERROR_MALLOC;//申请内存失败
    }
    return SUCCESS;//初始化成功
}

static pfdeinit_return_t deinit(buffer_t* self){
    if((self->interface.pfdeinit()) != 0){
        return ERROR_CALL_INTERFACE_DEINIT;
    }
    if(self->receive_buffer.padd_buffer == NULL){
        return ERROR_MULTIPLE_FREE;
    }
    free(self->receive_buffer.padd_buffer);
    self->receive_buffer.padd_buffer = NULL;
    return SUCCESS;
}




__attribute__((weak)) void overflow_callback(buffer_t *pinterrupt_global_buffer){
 
}


/// @brief 获取缓冲区是否空
/// @param buffer 
/// @return 1为空，0为非空
static uint8_t _get_buffer_flag_null(receive_buffer_t *buffer){
    if(buffer->write_index == buffer->read_index) return 1;
    else return 0; 
} 



/// @brief 获取缓冲区写入number长度的数据之后是否满,DMA使用时使用此函数，不使用时不用
/// @param buffer 
/// @param number写入的数量 
/// @return 1为满，0为非满
static uint8_t _get_buffer_flag_of_IDLE(receive_buffer_t *buffer, uint16_t number){
    uint16_t write_index = buffer->write_index;
    uint16_t read_index = buffer->read_index;
    uint16_t size = buffer->size;
    
    if(write_index >= read_index){
        if(write_index + number >= size - 1 && (int16_t)write_index + number - size >= (int16_t)read_index - 1) return 1;
    }
    else{
        if(write_index + number >= read_index - 1) return 1;
    } 
    return 0;
}



extern  buffer_t *pinterrupt_global_buffer;

/// @brief 中断服务函数回调函数，在中断中调用(使用DMA才产生此函数)，不使用DMA不产生
/// @param  
void IDLE_IT_callback(void){
    uint16_t num = pinterrupt_global_buffer->interface.pfget_rec_IDLE_num(); 
    uint16_t write_index = pinterrupt_global_buffer->receive_buffer.write_index;
    uint16_t read_index = pinterrupt_global_buffer->receive_buffer.read_index;
    uint16_t size = pinterrupt_global_buffer->receive_buffer.size;

    if(_get_buffer_flag_of_IDLE(&(pinterrupt_global_buffer->receive_buffer), num) == 1){//如果满了，调用溢出回调函数，溢出标志位置1
        overflow_callback(pinterrupt_global_buffer);
        pinterrupt_global_buffer->receive_buffer.flag_OF = 1;
    }

    if((write_index + num) <= size){//数据序号自增
            write_index += num;
        }
        else{
            write_index = write_index + num - size;
        }
        
    pinterrupt_global_buffer->interface.pfDMA_receive();//使用DMA接收数据重新开始接受
}

/// @brief 获取缓冲区现在状态是否满，DMA不使用时使用此函数，使用时不用
/// @param buffer 
/// @return 1为满，0为非满
static uint8_t _get_buffer_flag_of_RXNE(receive_buffer_t *buffer){
    uint16_t write_index = pinterrupt_global_buffer->receive_buffer.write_index;
    uint16_t read_index = pinterrupt_global_buffer->receive_buffer.read_index;
    uint16_t size = pinterrupt_global_buffer->receive_buffer.size;

    if(write_index >= read_index){
        if(write_index + 1 == size - 1 && read_index == 0) return 1; //如果写入序号为size-1，读取序号为0，则满
    }
    else {
        if(write_index + 1 == read_index) return 1; //如果写入序号+1等于读取序号，则满
    } return 0; //否则不满
    
}   
        


/// @brief 中断服务函数回调函数，在中断中调用(不使用DMA才产生此函数)，使用DMA不产生
/// @param  
void RXNE_IT_callback(void){ 
    uint16_t write_index = pinterrupt_global_buffer->receive_buffer.write_index;
    uint16_t read_index = pinterrupt_global_buffer->receive_buffer.read_index;
    uint16_t size = pinterrupt_global_buffer->receive_buffer.size;
    uint16_t *padd_buffer = pinterrupt_global_buffer->receive_buffer.padd_buffer;

    if(_get_buffer_flag_of_RXNE(&(pinterrupt_global_buffer->receive_buffer)) == 1){//如果满了，调用溢出回调函数，溢出标志位置1
        overflow_callback(pinterrupt_global_buffer);
        pinterrupt_global_buffer->receive_buffer.flag_OF = 1;
    }

    if(write_index != size - 1){//数据序号自增,接受数据
        padd_buffer[write_index] = pinterrupt_global_buffer->interface.pfread_receive();
        write_index += 1;
    }       
    else{
        padd_buffer[write_index] = pinterrupt_global_buffer->interface.pfread_receive();
        write_index = 0;
    }
}

/// @brief 待处理指令的数量队列
/// @param self 
/// @return 数量
static uint16_t pfget_FIFO_lengh(buffer_t *self){
    return self->FIFO_unpacked.size;
}

/// @brief 解包函数
/// @param self 
/// @return 标志位
static pfunpack_rec_buffer_return_t unpack_rec_buffer(buffer_t* self){
    


    return SUCCESS;
}

/// @brief 处理函数，转到处理函数处理
/// @param self 
/// @return 
static pfprocess_return_t pfprocess(buffer_t*self){
    

    return SUCCESS;
}




/// @brief 发送
/// @param self buffer结构体
/// @param protocol_send 协议发送结构体
/// @param protocol_type 协议类型
/// @param pdata 数据指针
/// @param data_length 数据长度
/// @return pfsend_return_t枚举类型
static pfsend_return_t pfsend(buffer_t *self, uint8_t *protocol_send, protocol_type_t protocol_type, uint8_t *pdata, uint16_t data_length){
    uint8_t position_data = table_protocol_type_to_position_data[protocol_type];
    uint8_t protocol_send_length = table_protocol_type_to_protocol_send_length[protocol_type];

    self->interface.pfsendbyte(protocol_send, position_data);

    self->interface.pfsendbyte(pdata, data_length);
  
    self->interface.pfsendbyte(protocol_send + position_data, protocol_send_length - position_data);
    
    
    return SUCCESS;
}