#include "memory/memory_mgr.h"

// 内存池
static uint8_t memoryPool[MEMORY_POOL_SIZE];
// 内存块管理
static MemoryBlock memoryBlocks[MAX_MEMORY_BLOCKS];
// 内存块数量
static int memoryBlockCount = 0;

void initMemoryManager() {
  // 初始化内存池
  memset(memoryPool, 0, MEMORY_POOL_SIZE);
  // 初始化内存块
  memset(memoryBlocks, 0, sizeof(memoryBlocks));
  
  // 创建第一个内存块，大小为整个内存池
  memoryBlocks[0].size = MEMORY_POOL_SIZE;
  memoryBlocks[0].used = false;
  memoryBlocks[0].data = memoryPool;
  memoryBlockCount = 1;
  
  Serial.println("Memory manager initialized");
  printMemoryUsage();
}

void* allocateMemory(size_t size) {
  // 寻找合适的内存块
  for (int i = 0; i < memoryBlockCount; i++) {
    if (!memoryBlocks[i].used && memoryBlocks[i].size >= size) {
      // 检查是否需要分割内存块
      if (memoryBlocks[i].size > size + sizeof(MemoryBlock)) {
        // 分割内存块
        if (memoryBlockCount < MAX_MEMORY_BLOCKS) {
          // 创建新的内存块
          memoryBlocks[memoryBlockCount].size = memoryBlocks[i].size - size;
          memoryBlocks[memoryBlockCount].used = false;
          memoryBlocks[memoryBlockCount].data = memoryBlocks[i].data + size;
          memoryBlockCount++;
          
          // 更新原内存块
          memoryBlocks[i].size = size;
        }
      }
      
      // 标记内存块为使用
      memoryBlocks[i].used = true;
      return memoryBlocks[i].data;
    }
  }
  
  // 没有找到合适的内存块，使用动态内存
  Serial.printf("Memory pool full, using dynamic allocation for %u bytes\n", size);
  return malloc(size);
}

void freeMemory(void* ptr) {
  if (ptr == NULL) return;
  
  // 检查是否是内存池中的内存
  for (int i = 0; i < memoryBlockCount; i++) {
    if (memoryBlocks[i].used && memoryBlocks[i].data == ptr) {
      // 标记内存块为未使用
      memoryBlocks[i].used = false;
      
      // 尝试合并相邻的未使用内存块
      for (int j = 0; j < memoryBlockCount; j++) {
        if (j != i && !memoryBlocks[j].used) {
          // 检查是否相邻
          if (memoryBlocks[i].data + memoryBlocks[i].size == memoryBlocks[j].data) {
            // 合并内存块
            memoryBlocks[i].size += memoryBlocks[j].size;
            
            // 移除被合并的内存块
            for (int k = j; k < memoryBlockCount - 1; k++) {
              memoryBlocks[k] = memoryBlocks[k + 1];
            }
            memoryBlockCount--;
            j--; // 重新检查当前位置
          } else if (memoryBlocks[j].data + memoryBlocks[j].size == memoryBlocks[i].data) {
            // 合并内存块
            memoryBlocks[j].size += memoryBlocks[i].size;
            memoryBlocks[j].data = memoryBlocks[i].data;
            
            // 移除被合并的内存块
            for (int k = i; k < memoryBlockCount - 1; k++) {
              memoryBlocks[k] = memoryBlocks[k + 1];
            }
            memoryBlockCount--;
            i = j; // 更新当前内存块索引
          }
        }
      }
      
      return;
    }
  }
  
  // 不是内存池中的内存，使用free释放
  free(ptr);
}

void printMemoryUsage() {
  size_t usedMemory = 0;
  size_t freeMemory = 0;
  
  for (int i = 0; i < memoryBlockCount; i++) {
    if (memoryBlocks[i].used) {
      usedMemory += memoryBlocks[i].size;
    } else {
      freeMemory += memoryBlocks[i].size;
    }
  }
  
  Serial.printf("Memory usage: %u/%u bytes used, %u bytes free\n", 
                usedMemory, MEMORY_POOL_SIZE, freeMemory);
  Serial.printf("Heap memory: %u bytes free\n", ESP.getFreeHeap());
}

size_t getFreeMemory() {
  size_t freeMemory = 0;
  
  for (int i = 0; i < memoryBlockCount; i++) {
    if (!memoryBlocks[i].used) {
      freeMemory += memoryBlocks[i].size;
    }
  }
  
  return freeMemory + ESP.getFreeHeap();
}