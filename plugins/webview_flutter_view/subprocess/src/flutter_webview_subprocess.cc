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

#include "include/cef_app.h"
#include "wrapper/cef_library_loader.h"
#include <capi/cef_app_capi.h>
#include <unistd.h>
#include <iostream>

// Entry point function for sub-processes (i.e. render, plugin, GPU, etc)
int main(int argc, char* argv[]) {
  cef_main_args_t main_args({argc, argv});

#if(WEBVIEW_SUBPROCESS_DEBUG)
  std::cout << "Parent Process id : " << getpid() << std::endl; 
  std::cout << "Child Process with parent id : " << getppid() << std::endl; 
  std::cout << "Subprocess arg count: " << argc << std::endl;
  for(int i = 0; i < argc; i++)
  {
    std::cout << "Subprocess arg " << argc << ": " << argv[i] << std::endl;
  }
#endif

  int exit_code = cef_execute_process(&main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    return exit_code;
  }

  return 0;
}
