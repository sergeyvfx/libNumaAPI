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

#include <stdio.h>
#include <stdlib.h>

#include "numaapi.h"

int main(int argc, char** argv) {
  (void) argc;  // Ignored.
  (void) argv;  // Ignored.
  if (numaAPI_Initialize() != NUMAAPI_SUCCESS) {
    fprintf(stderr, "NUMA API initialization failure.\n");
    return EXIT_FAILURE;
  }
  const int num_nodes = numaAPI_GetNumNodes();
  printf("NUMA API successfully initialized.\n");
  printf("Number of nodes: %d\n", num_nodes);
  printf("Number of processors on current nodes: %d\n",
         numaAPI_GetNumCurrentNodesProcessors());
  int node;
  for (node = 0; node < num_nodes; ++node) {
    printf("Node %d:\n", node);
    printf("  Available: %s\n",
           numaAPI_IsNodeAvailable(node) ? "True" : "False");
    const int num_processors = numaAPI_GetNumNodeProcessors(node);
    printf("  Number of processors: %d\n", num_processors);
    int* test_memory = numaAPI_AllocateOnNode(sizeof(int), node);
    printf("  Test memory allocated: %p\n", test_memory);
    numaAPI_Free(test_memory, sizeof(test_memory));
  }
  return EXIT_SUCCESS;
}
