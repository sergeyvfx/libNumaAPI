// Copyright (c) 2016, libnumaapi authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Sergey Sharybin (sergey.vfx@gmail.com)

// TODO(sergey):
// - Check whether all platforms compiles here.
// - There is something crazy going on with node type,
//   causes some nasty type narrowing.

#ifdef _WIN32

#include "libnumaapi.h"

#ifndef NOGDI
#  define NOGDI
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef _M_X64
#  include <VersionHelpers.h>
#endif

NUMAPIResult numaApiInitialize(void) {
#ifndef _M_X64
  // No NUMA on 32 bit platforms.
  return LIBNUMAAPI_INAVAILABLE;
#endif
  if (!IsWindows7OrGreater()) {
    // Require Windows 7 or higher.
    LIBNUMAAPI_INAVAILABLE;
  }
  // TODO(sergey): Get entry points to functions.
  return LIBNUMAAPI_SUCCESS;
}

int numaApiGetNumNodes(void) {
  ULONG highest_node_number;
  if (!GetNumaHighestNodeNumber(&highest_node_number)) {
    return 0;
  }
  // TODO(sergey): Resolve such type narrowing.
  // NOTE: This is not necesserily a total amount of nodes in the system.
  return (int)highest_node_number;
}

int numApiIsNodeAvailable(int node) {
  // Trick to detect whether the node is usable or not: check whether
  // there are any processors associated with it.
  //
  // This is needed because numaApiGetNumNodes() is not guaramteed to
  // give total amount of nodes and some nodes might be unavailable.
  LONGLONG processor_mask;
  if (!GetNumaNodeProcessorMask(node, &processor_mask)) {
    return 0;
  }
  if (processor_mask == 0) {
    return 0;
  }
  return 1;
}

void* numaApiAllocateOnNode(size_t size, int node) {
  return VirtualAllocExNuma(GetCurrentProcess(),
                            NULL,
                            size,
                            MEM_RESERVE | MEM_COMMIT,
                            PAGE_READWRITE,
                            node);
}

void* numaApiAllocateLocal(size_t size) {
  UCHAR current_processor = (UCHAR)GetCurrentProcessorNumber();
  UCHAR node;
  if (!GetNumaProcessorNode(current_processor, &node)) {
    return NULL;
  }
  return numaApiAllocateOnNode(size, node);
}

void numaApiFree(void* start, size_t size) {
  if (!VirtualFree(start, size, MEM_RELEASE)) {
    // TODO(sergey): Throw an error!
  }
}

#endif
