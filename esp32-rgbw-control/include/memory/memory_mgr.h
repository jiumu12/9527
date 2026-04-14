#ifndef MEMORY_MGR_H
#define MEMORY_MGR_H

#include <Arduino.h>

// 内存池配置
#define MEMORY_POOL_SIZE 4096 // 内存池大小
#define MAX_MEMORY_BLOCKS 16   // 最大内存块数量

// 内存块结构
typedef struct {
  size_t size;      // 内存块大小
  bool used;        // 是否被使用
  uint8_t* data;    // 内存数据指针
} MemoryBlock;

// 函数声明
void initMemoryManager();
void* allocateMemory(size_t size);
void freeMemory(void* ptr);
void printMemoryUsage();
size_t getFreeMemory();

#endif // MEMORY_MGR_H