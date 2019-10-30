/*-
 * Copyright (c) 2015 Masayoshi Mizutani <mizutani@sfc.wide.ad.jp>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FLUENT_QUEUE_HPP__
#define __FLUENT_QUEUE_HPP__

#include <string>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include "./message.hpp"

namespace fluent {
  class MsgQueue {
  private:
    static const bool DBG;
    Message *msg_head_;
    Message *msg_tail_;
    size_t count_;
    size_t limit_;

  public:
    MsgQueue();
    virtual ~MsgQueue();
    virtual bool push(Message *msg);
    virtual Message *pop();
    virtual Message *bulk_pop();    
    virtual void set_limit(size_t limit);
    // count_ may be critical section, however the function is read only
    // and integer can be read atomically in x86 arch.
    size_t count() const { return this->count_; }; 
    size_t limit() const { return this->limit_; };
  };

  class MsgThreadQueue : public MsgQueue {
  private:
    static const bool DBG;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    bool term_;
    
  public:
    MsgThreadQueue();
    ~MsgThreadQueue();
    bool push(Message *msg);
    Message *bulk_pop();
    void set_limit(size_t limit);
    
    void term();
    bool is_term();    
  };
}


#endif   // __SRC_FLUENT_QUEUE_H__
