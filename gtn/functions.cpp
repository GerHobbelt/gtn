/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <algorithm>
#include <queue>
#include <set>

#include "gtn/cpu/functions.h"
#include "gtn/cuda/functions.h"

namespace gtn {

#define DISPATCH1(fn) \
Graph fn(const Graph& g) { \
  if (g.isCuda()) { \
    return cuda::fn(g); \
  } else { \
    return cpu::fn(g); \
  } \
}

#define DISPATCH2(fn) \
Graph fn(const Graph& g1, const Graph& g2) { \
  deviceCheck(g1, g2, #fn); \
  if (g1.isCuda()) { \
    return cuda::fn(g1, g2); \
  } else { \
    return cpu::fn(g1, g2); \
  } \
}

#define DISPATCHV(fn) \
Graph fn(const std::vector<Graph>& graphs) { \
  if (graphs.empty()) { \
    return cpu::fn(graphs); \
  } \
  deviceCheck(graphs, #fn); \
  if (graphs[0].isCuda()) { \
    return cuda::fn(graphs); \
  } else { \
    return cpu::fn(graphs); \
  } \
}

void deviceCheck(const std::vector<Graph>& graphs, const std::string& name) {
  bool isCuda = graphs[0].isCuda();
  int device;
  if (isCuda) {
     device = graphs[0].device();
  }
  for (auto& g : graphs) {
    if (g.isCuda() != isCuda || (isCuda && g.device() != device)) {
      throw std::invalid_argument(
        "[" + name + "] Graphs must be on the same device");
    }
  }
}

void deviceCheck(const Graph& g1, const Graph& g2, const std::string& name) {
  deviceCheck({g1, g2}, name);
}

DISPATCH1(negate)
DISPATCH2(add)
DISPATCH2(subtract)

Graph clone(const Graph& g, Projection projection /* = Projection::NONE */) {
  if (g.isCuda()) {
    return cuda::clone(g, projection);
  } else {
    return cpu::clone(g, projection);
  }
}

Graph projectInput(const Graph& g) {
  return clone(g, Projection::INPUT);
}

Graph projectOutput(const Graph& g) {
  return clone(g, Projection::OUTPUT);
}

Graph concat(const Graph& g1, const Graph& g2) {
  return concat({g1, g2});
}

DISPATCHV(concat)
DISPATCH1(closure)
DISPATCHV(union_)
DISPATCH2(intersect)
DISPATCH2(compose)

Graph remove(const Graph& g, int label /* = epsilon */) {
  return remove(g, label, label);
}

Graph remove(const Graph& g, int ilabel, int olabel) {
  if (g.isCuda()) {
    return cuda::remove(g, ilabel, olabel);
  } else {
    return cpu::remove(g, ilabel, olabel);
  }
}

DISPATCH1(forwardScore)
DISPATCH1(viterbiScore)
DISPATCH1(viterbiPath)

} // namespace gtn
