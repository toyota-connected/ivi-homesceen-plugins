/*
 * Copyright 2020-2024 Toyota Connected North America
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ecsystems_manager.h"

#include <spdlog/spdlog.h>
#include <asio/post.hpp>
#include <chrono>
#include <thread>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
ECSystemManager* ECSystemManager::m_poInstance = nullptr;
ECSystemManager* ECSystemManager::GetInstance() {
  if (m_poInstance == nullptr) {
    m_poInstance = new ECSystemManager();
  }

  return m_poInstance;
}

////////////////////////////////////////////////////////////////////////////
ECSystemManager::~ECSystemManager() {
  spdlog::debug("ECSystemManager~");
}

////////////////////////////////////////////////////////////////////////////
ECSystemManager::ECSystemManager()
    : io_context_(std::make_unique<asio::io_context>(ASIO_CONCURRENCY_HINT_1)),
      work_(make_work_guard(io_context_->get_executor())),
      strand_(std::make_unique<asio::io_context::strand>(*io_context_)),
      m_eCurrentState(NotInitialized) {
  vSetupThreadingInternals();
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::StartRunLoop() {
  if (m_bIsRunning) {
    return;
  }

  m_bIsRunning = true;
  m_bSpawnedThreadFinished = false;

  // Launch RunLoop in a separate thread
  loopThread_ = std::thread(&ECSystemManager::RunLoop, this);
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::vSetupThreadingInternals() {
  filament_api_thread_ = std::thread([this] {
    // Save this thread's ID as it runs io_context_->run()
    filament_api_thread_id_ = pthread_self();

    // Optionally set the thread name
    pthread_setname_np(filament_api_thread_id_, "ECSystemManagerThreadRunner");

    spdlog::debug("ECSystemManager Filament API thread started: 0x{:x}",
                  filament_api_thread_id_);

    io_context_->run();
  });
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::RunLoop() {
  constexpr std::chrono::milliseconds frameTime(16);  // ~1/60 second

  // Initialize lastFrameTime to the current time
  auto lastFrameTime = std::chrono::steady_clock::now();

  m_eCurrentState = Running;
  while (m_bIsRunning) {
    auto start = std::chrono::steady_clock::now();

    // Calculate the time difference between this frame and the last frame
    std::chrono::duration<float> elapsedTime = start - lastFrameTime;

    if (!isHandlerExecuting.load()) {
      // Use asio::post to schedule work on the main thread (API thread)
      post(*strand_, [elapsedTime = elapsedTime.count(), this] {
        isHandlerExecuting.store(true);
        try {
          ExecuteOnMainThread(
              elapsedTime);  // Pass elapsed time to the main thread
        } catch (...) {
          isHandlerExecuting.store(false);
          throw;  // Rethrow the exception after resetting the flag
        }
        isHandlerExecuting.store(false);
      });
    }

    // Update the time for the next frame
    lastFrameTime = start;

    auto end = std::chrono::steady_clock::now();

    // Sleep for the remaining time in the frame
    if (std::chrono::duration<double> elapsed = end - start;
        elapsed < frameTime) {
      std::this_thread::sleep_for(frameTime - elapsed);
    }
  }
  m_eCurrentState = ShutdownStarted;

  m_bSpawnedThreadFinished = true;
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::StopRunLoop() {
  m_bIsRunning = false;
  if (loopThread_.joinable()) {
    loopThread_.join();
  }

  // Stop the io_context
  io_context_->stop();

  // Reset the work guard
  work_.reset();

  // Join the filament_api_thread_
  if (filament_api_thread_.joinable()) {
    filament_api_thread_.join();
  }
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::ExecuteOnMainThread(const float elapsedTime) {
  vUpdate(elapsedTime);
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::vInitSystems() {
  // Note this is currently expected to be called from within
  // an already asio post, Leaving this commented out so you know
  // that you could change up the routine, but if you do
  // it needs to run on the main thread.
  // asio::post(*ECSystemManager::GetInstance()->GetStrand(), [&] {
  for (const auto& system : m_vecSystems) {
    system->vInitSystem();
  }

  m_eCurrentState = Initialized;

  //});
}

////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ECSystem> ECSystemManager::poGetSystem(
    const size_t systemTypeID,
    const std::string& where) {
  if (const auto callingThread = pthread_self();
      callingThread != filament_api_thread_id_) {
    // Note we should have a 'log once' base functionality in common
    // creating this inline for now.
    if (const auto foundIter = m_mapOffThreadCallers.find(where);
        foundIter == m_mapOffThreadCallers.end()) {
      spdlog::info(
          "From {} "
          "You're calling to get a system from an off thread, undefined "
          "experience!"
          " Use a message to do your work or grab the ecsystemmanager strand "
          "and "
          "do your work.",
          where);

      m_mapOffThreadCallers.insert(std::pair(where, 0));
    }
  }

  std::unique_lock lock(vecSystemsMutex);
  for (const auto& system : m_vecSystems) {
    if (system->GetTypeID() == systemTypeID) {
      return system;
    }
  }
  return nullptr;  // If no matching system found
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::vAddSystem(std::shared_ptr<ECSystem> system) {
  std::unique_lock lock(vecSystemsMutex);
  spdlog::debug("Adding system at address {}",
                static_cast<void*>(system.get()));
  m_vecSystems.push_back(std::move(system));
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::vUpdate(const float deltaTime) {
  // Copy systems under mutex
  std::vector<std::shared_ptr<ECSystem>> systemsCopy;
  {
    std::unique_lock lock(vecSystemsMutex);

    // Copy the systems vector
    systemsCopy = m_vecSystems;
  }  // Mutex is unlocked here

  // Iterate over the copy without holding the mutex
  for (const auto& system : systemsCopy) {
    if (system) {
      system->vProcessMessages();
      system->vUpdate(deltaTime);
    } else {
      spdlog::error("Encountered null system pointer!");
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::DebugPrint() const {
  for (const auto& system : m_vecSystems) {
    spdlog::debug(
        "ECSystemManager:: DebugPrintProcessing system at address {}, "
        "use_count={}",
        static_cast<void*>(system.get()), system.use_count());
  }
}

////////////////////////////////////////////////////////////////////////////
void ECSystemManager::vShutdownSystems() {
  post(*GetInstance()->GetStrand(), [&] {
    // we shutdown in reverse, until we have a 'system dependency tree' type of
    // view, filament system (which is always the first system, needs to be
    // shutdown last as its 'engine' varible is used in destruction for other
    // systems
    for (auto it = m_vecSystems.rbegin(); it != m_vecSystems.rend(); ++it) {
      (*it)->vShutdownSystem();
    }

    m_eCurrentState = Shutdown;
  });
}

}  // namespace plugin_filament_view