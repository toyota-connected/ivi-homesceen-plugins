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

#pragma once

#include "flutter/shell/platform/embedder/embedder.h"

#include "include/fpdfview.h"

namespace plugin_pdf {

struct LibPdfiumExports {
  LibPdfiumExports() = default;
  explicit LibPdfiumExports(void* lib);

  typedef void (*FPDF_InitLibraryWithConfigFnPtr)(
      const FPDF_LIBRARY_CONFIG* config);
  typedef unsigned long (*FPDF_GetLastErrorFnPtr)();
  typedef FPDF_DOCUMENT (*FPDF_LoadMemDocument64FnPtr)(
      const void* data_buf,
      size_t size,
      FPDF_BYTESTRING password);
  typedef FPDF_DOCUMENT (*FPDF_LoadDocumentFnPtr)(FPDF_STRING file_path,
                                                  FPDF_BYTESTRING password);
  typedef void (*FPDF_DestroyLibraryFnPtr)();
  typedef int (*FPDF_GetPageCountFnPtr)(FPDF_DOCUMENT document);
  typedef FPDF_PAGE (*FPDF_LoadPageFnPtr)(FPDF_DOCUMENT document,
                                          int page_index);
  typedef double (*FPDF_GetPageWidthFnPtr)(FPDF_PAGE page);
  typedef double (*FPDF_GetPageHeightFnPtr)(FPDF_PAGE page);
  typedef FPDF_BITMAP (*FPDFBitmap_CreateFnPtr)(int width,
                                                int height,
                                                int alpha);
  typedef FPDF_BOOL (*FPDFBitmap_FillRectFnPtr)(FPDF_BITMAP bitmap,
                                                int left,
                                                int top,
                                                int width,
                                                int height,
                                                FPDF_DWORD color);
  typedef void (*FPDF_RenderPageBitmapFnPtr)(FPDF_BITMAP bitmap,
                                             FPDF_PAGE page,
                                             int start_x,
                                             int start_y,
                                             int size_x,
                                             int size_y,
                                             int rotate,
                                             int flags);
  typedef void* (*FPDFBitmap_GetBufferFnPtr)(FPDF_BITMAP bitmap);
  typedef int (*FPDFBitmap_GetStrideFnPtr)(FPDF_BITMAP bitmap);
  typedef void (*FPDFBitmap_DestroyFnPtr)(FPDF_BITMAP bitmap);
  typedef void (*FPDF_ClosePageFnPtr)(FPDF_PAGE page);
  typedef void (*FPDF_CloseDocumentFnPtr)(FPDF_DOCUMENT document);

  FPDF_InitLibraryWithConfigFnPtr InitLibraryWithConfig = nullptr;
  FPDF_GetLastErrorFnPtr GetLastError = nullptr;
  FPDF_DestroyLibraryFnPtr DestroyLibrary = nullptr;
  FPDF_LoadMemDocument64FnPtr LoadMemDocument64 = nullptr;
  FPDF_LoadDocumentFnPtr LoadDocument = nullptr;
  FPDF_GetPageCountFnPtr GetPageCount = nullptr;
  FPDF_LoadPageFnPtr LoadPage = nullptr;
  FPDF_GetPageWidthFnPtr GetPageWidth = nullptr;
  FPDF_GetPageHeightFnPtr GetPageHeight = nullptr;
  FPDFBitmap_CreateFnPtr Bitmap_Create = nullptr;
  FPDFBitmap_FillRectFnPtr Bitmap_FillRect = nullptr;
  FPDF_RenderPageBitmapFnPtr RenderPageBitmap = nullptr;
  FPDFBitmap_GetBufferFnPtr Bitmap_GetBuffer = nullptr;
  FPDFBitmap_GetStrideFnPtr Bitmap_GetStride = nullptr;
  FPDFBitmap_DestroyFnPtr Bitmap_Destroy = nullptr;
  FPDF_ClosePageFnPtr ClosePage = nullptr;
  FPDF_CloseDocumentFnPtr CloseDocument = nullptr;
};

class LibPdfium {
 public:
  static bool IsPresent(const char* library_path = nullptr) {
    return loadExports(library_path) != nullptr;
  }

  LibPdfiumExports* operator->() const;

 private:
  static LibPdfiumExports* loadExports(const char* library_path);
};

extern LibPdfium LibPdfium;
}  // namespace plugin_pdf