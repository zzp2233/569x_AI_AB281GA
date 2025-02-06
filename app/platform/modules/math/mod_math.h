#ifndef MOD_MATH_H
#define MOD_MATH_H

typedef struct
{
    u8* array;     // 存储队列元素的数组指针
    int front;      // 队列头索引
    int rear;       // 队列尾索引
    int capacity;   // 队列容量
} array_queue_t;

extern const u8 crc8_tbl[256];

u8 mod_crc8_maxim(u8 *buf, u8 length);
bool array_is_null(uint8_t * array, uint8_t len);                       //判断数组是否为空
bool array_is_equal(uint8_t * array1, uint8_t * array2, uint8_t len);   //判断数组是否相等
void array_reverse(u8 *arr, u8 size);

/**
 * @brief 根据判断索引位置是否存在有效数组元素
 * @param[in] queue: 队列指针
 * @param[in] idx: 元素索引
 * @return 索引位置的元素值, -1: 索引不存在;
 **/
bool mod_queue_idx_is_valid(array_queue_t *queue, int idx);

void mod_queue_creat(array_queue_t *queue);     // 创建队列
u8 mod_queue_is_empty(array_queue_t *queue);    // 判断队列是否为空
u8 mod_queue_is_full(array_queue_t *queue);     // 判断队列是否已满
u8 mod_queue_size(array_queue_t *queue);        // 获取队列大小
bool mod_inqueue(array_queue_t *queue, u8 item);// 判断队列是否已存在元素
void mod_enqueue(array_queue_t *queue, u8 item);// 入队操作
u8 mod_dequeue(array_queue_t *queue);           // 出队操作
u8 mod_queue_front(array_queue_t *queue);       // 获取队头元素
u8 mod_queue_rear(array_queue_t *queue);        // 获取队尾元素
#endif  //MOD_MATH_H
