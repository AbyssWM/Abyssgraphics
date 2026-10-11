#include <abyssgraphics/resource/resources/AsyncResource.hpp>

#include <condition_variable>

namespace Abyssgraphics {
    struct SAsyncResourceImpl {
        Abyssutils::Memory::CUniquePointer<std::condition_variable> awaitingCv;
        std::mutex                                                 awaitingMtx;
        bool                                                       awaitingEvent = false;
    };
}