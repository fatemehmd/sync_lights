#include "Singleton.h"

#include <atomic>
#include <mutex>
#include "freertos/queue.h"
#include "data_types.h"

namespace backpack {

Singleton::Singleton()
{
     ParamsQueue = xQueueCreate( 10, sizeof(struct backpack::LightParams2) );
}

QueueHandle_t Singleton::GetParamsQueue() {
    return ParamsQueue;
}

std::atomic<Singleton*> m_instance;
std::mutex m_mutex;

Singleton* Singleton::GetInstance() {
    Singleton* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        tmp = m_instance.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new Singleton;
            std::atomic_thread_fence(std::memory_order_release);
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}

}  // namespace backpack