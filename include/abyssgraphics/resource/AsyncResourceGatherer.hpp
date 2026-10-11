#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include "../cairo/CairoSurface.hpp"
#include "./resources/AsyncResource.hpp"
#include <abyssutils/memory/Atomic.hpp>

namespace Abyssgraphics {
    class CAsyncResourceGatherer {
      public:
        CAsyncResourceGatherer();
        ~CAsyncResourceGatherer();

        void enqueue(Abyssutils::Memory::CAtomicSharedPointer<IAsyncResource> resource);

        // Synchronously await the resource being available
        void await(Abyssutils::Memory::CAtomicSharedPointer<IAsyncResource> resource);

      private:
        std::thread m_gatherThread;

        struct {
            std::mutex              requestMutex;
            std::condition_variable requestsCV;

            bool                    exit           = false;
            bool                    needsToProcess = false;
        } m_asyncLoopState;

        std::vector<Abyssutils::Memory::CAtomicSharedPointer<IAsyncResource>> m_targetsToLoad;
        std::mutex                                                           m_targetsToLoadMutex;

        //
        void asyncAssetSpinLock();
        void wakeUpMainThread();
    };
}
