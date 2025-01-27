/*
 * Copyright 2025 Toyota Connected North America
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

#include "libpdfium.h"

#include <iostream>

#include <dlfcn.h>
#include <shared_library/shared_library.h>

#include "shared_library.h"

namespace plugin_pdf {

LibPdfiumExports::LibPdfiumExports(void* lib) {
  if (lib != nullptr) {
    PluginGetFuncAddress(lib, "FPDF_InitLibraryWithConfig",
                         &InitLibraryWithConfig);
    PluginGetFuncAddress(lib, "FPDF_DestroyLibrary", &DestroyLibrary);
    PluginGetFuncAddress(lib, "FPDF_GetLastError", &GetLastError);
    PluginGetFuncAddress(lib, "FPDF_LoadDocument", &LoadDocument);
    PluginGetFuncAddress(lib, "FPDF_LoadMemDocument64", &LoadMemDocument64);
    PluginGetFuncAddress(lib, "FPDF_CloseDocument", &CloseDocument);

    PluginGetFuncAddress(lib, "FPDF_LoadPage", &LoadPage);
    PluginGetFuncAddress(lib, "FPDF_ClosePage", &ClosePage);
    PluginGetFuncAddress(lib, "FPDF_GetPageCount", &GetPageCount);
    PluginGetFuncAddress(lib, "FPDF_GetPageWidth", &GetPageWidth);
    PluginGetFuncAddress(lib, "FPDF_GetPageHeight", &GetPageHeight);

    PluginGetFuncAddress(lib, "FPDFBitmap_Create", &Bitmap_Create);
    PluginGetFuncAddress(lib, "FPDFBitmap_Destroy", &Bitmap_Destroy);
    PluginGetFuncAddress(lib, "FPDFBitmap_FillRect", &Bitmap_FillRect);
    PluginGetFuncAddress(lib, "FPDFBitmap_GetBuffer", &Bitmap_GetBuffer);
    PluginGetFuncAddress(lib, "FPDFBitmap_GetStride", &Bitmap_GetStride);

    PluginGetFuncAddress(lib, "FPDF_RenderPageBitmap", &RenderPageBitmap);
  }
}

LibPdfiumExports* LibPdfium::operator->() const {
  return loadExports(nullptr);
}

LibPdfiumExports* LibPdfium::loadExports(const char* library_path = nullptr) {
  static LibPdfiumExports exports = [&] {
    void* lib = dlopen(library_path ? library_path : "libpdfium.so",
                       RTLD_NOW | RTLD_GLOBAL);

    return LibPdfiumExports(lib);
  }();

  return exports.InitLibraryWithConfig ? &exports : nullptr;
}

class LibPdfium LibPdfium;
}  // namespace plugin_pdf