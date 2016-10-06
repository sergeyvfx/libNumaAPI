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

#ifdef __linux__

#include "libnumaapi.h"

#ifndef WITH_DYNLOAD
#  include <numa.h>
#else
#  include <dlfcn.h>
#endif

#ifdef WITH_DYNLOAD

typedef void* DynamicLibrary;

#  define dynamicLibraryOpen(path)         dlopen(path, RTLD_NOW)
#  define dynamicLibraryClose(lib)         dlclose(lib)
#  define dynamicLibraryFind(lib, symbol)  dlsym(lib, symbol)

#define _LIBRARY_FIND_CHECKED(lib, name) \
        name = (t##name *)dynamicLibraryFind(lib, #name); \
        assert(name);

#define NUMA_LIBRARY_FIND_CHECKED(name) \
        _LIBRARY_FIND_CHECKED(numa_lib, name)
#define NUMA_LIBRARY_FIND(name) _LIBRARY_FIND(numa_lib, name)

#define _LIBRARY_FIND(lib, name) \
        name = (t##name *)dynamicLibraryFind(lib, #name);

static DynamicLibrary numa_lib;

typedef int tnuma_available(void);
typedef int tnuma_max_node(void);
typedef long tnuma_node_size(int node, long* freep);
typedef void* tnuma_alloc_onnode(size_t size, int node);
typedef void* tnuma_alloc_local(size_t size);
typedef void tnuma_free(void* start, size_t size);

static tnuma_available* numa_available;
static tnuma_max_node* numa_max_node;
static tnuma_node_size* numa_node_size;
static tnuma_alloc_onnode* numa_alloc_onnode;
static tnuma_alloc_local* numa_alloc_local;
static tnuma_free* numa_free;

static DynamicLibrary dynamicLibraryOpenFind(const char** paths) {
  int i = 0;
  while (paths[i] != NULL) {
      DynamicLibrary lib = dynamicLibraryOpen(paths[i]);
      if (lib != NULL) {
        return lib;
      }
      ++i;
  }
  return NULL;
}

static void numaExit(void) {
  if (numa_lib != NULL) {
    dynamicLibraryClose(numa_lib);
    numa_lib = NULL;
  }
}

static NUMAPIResult loadNumaSymbols(void) {
  // Prevnt multiple initializations.
  static int initialized = 0;
  static NUMAPIResult result = LIBNUMAAPI_INAVAILABLE;
  if (initialized) {
    return result;
  }
  initialized = 1;
  // Find appropriate .so library.
  const char* numa_paths[] = {"libnuma.so",
                              "libnuma.so.1",
                              "libnuma.so.2",
                              NULL};
  // Register de-initialization.
  int error = atexit(numaExit);
  if (error) {
    result = LIBNUMAAPI_INAVAILABLE;
    return result;
  }
  // Load library.
  numa_lib = dynamicLibraryOpenFind(numa_paths);
  if (numa_lib == NULL) {
    result = LIBNUMAAPI_INAVAILABLE;
    return result;
  }
  // Load symbols.
  NUMA_LIBRARY_FIND(numa_available);
  NUMA_LIBRARY_FIND(numa_max_node);
  NUMA_LIBRARY_FIND(numa_node_size);
  NUMA_LIBRARY_FIND(numa_alloc_onnode);
  NUMA_LIBRARY_FIND(numa_alloc_local);
  NUMA_LIBRARY_FIND(numa_free);

  result = LIBNUMAAPI_SUCCESS;
  return result;
}
#endif

NUMAPIResult numaApiInitialize(void) {
#ifdef WITH_DYNLOAD
  NUMAPIResult result = loadNumaSymbols();
  if (result != LIBNUMAAPI_SUCCESS) {
    return result;
  }
#endif
  if (numa_available() < 0) {
    return LIBNUMAAPI_INAVAILABLE;
  }
  return LIBNUMAAPI_SUCCESS;
}

int numaApiGetNumNodes(void) {
  return numa_max_node() + 1;
}

int numApiIsNodeAvailable(int node) {
  if (numa_node_size(node, NULL) > 0) {
    return 1;
  }
  return 0;
}

void* numaApiAllocateOnNode(size_t size, int node) {
  return numa_alloc_onnode(size, node);
}

void* numaApiAllocateLocal(size_t size) {
  return numa_alloc_local(size);
}

void numaApiFree(void* start, size_t size) {
  numa_free(start, size);
}

#endif
