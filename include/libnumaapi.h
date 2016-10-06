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

#ifndef __LIBNUMAAPI_H__
#define __LIBNUMAAPI_H__

#include <stddef.h>

typedef enum {
  LIBNUMAAPI_SUCCESS     = 0,
  // Generic error, no real details are available,
  LIBNUMAAPI_ERROR       = 1,
  // NUMA is not available on this platform.
  LIBNUMAAPI_INAVAILABLE = 2,
} NUMAPIResult;

// Initialize NUMA API.
//
// This is first call which should be called before any other NUMA functions
// can be used.
NUMAPIResult numaApiInitialize(void);

// Get number of available nodes.
//
// This is in fact an index of last node plus one and it's not guaranteed all
// nodes up to this one are available.
int numaApiGetNumNodes(void);

// Returns non-zero if the given node is available for compute.
int numApiIsNodeAvailable(int node);

// Allocate memory on a given node,
void* numaApiAllocateOnNode(size_t size, int node);

// Allocate memory in the local memory.
void* numaApiAllocateLocal(size_t size);

// Frees size bytes of memory starting at start.
void numaApiFree(void* start, size_t size);

#endif
