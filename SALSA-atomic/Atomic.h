/*
 * Copyright (c) 1998-2010 Julien Benoist <julien@benoist.name>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ATOMIC_H
#define _ATOMIC_H
#include <stdio.h>
#include <atomic>

/* double-word - as in machine word - primitive
 * used for the double-compare-and-swap operations */
typedef uint64_t DWORD;

#define FAA atomic_fetch_add
#define FAS atomic_fetch_sub
#define CAS atomic_compare_exchange_strong
#define DWCAS atomic_compare_exchange_strong

template<typename T>
class AtomicWrapper {
 public:
  void store(T desired, std::memory_order order = std::memory_order_seq_cst) {
    _entry.store(desired, order);
  }

  T load(std::memory_order order = std::memory_order_seq_cst) const {
    return _entry.load(order);
  }

  bool compare_exchange_strong(T& expected, T desired,
    std::memory_order order = std::memory_order_seq_cst) {
    return _entry.compare_exchange_strong(expected, desired, order);
  }

 private:
  std::atomic<T> _entry;
};

#endif /* _ATOMIC_H  */
