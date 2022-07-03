#ifndef SINGLETON_H
#define SINGLETON_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace backpack {
class Singleton
{
public:

    static Singleton* GetInstance();
    QueueHandle_t GetParamsQueue();
private:
    Singleton();
    ~Singleton() {};
    QueueHandle_t ParamsQueue = NULL;

};

}  // namespace backpack
#endif