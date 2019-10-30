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

#ifndef __FLUENT_LOGGER_HPP__
#define __FLUENT_LOGGER_HPP__

#include <set>
#include <string>
#include <vector>

namespace fluent {
  class Message;
  class Socket;
  class Emitter;
  class MsgQueue;
  
  class Logger {
  private:
    std::set<Message*> msg_set_;
    std::vector<Emitter*> emitter_;
    std::string errmsg_;
    std::vector<MsgQueue*> queue_;
    std::string tag_prefix_;
    
  public:
    Logger();
    ~Logger();

    void new_forward(const std::string &host, int port=24224);
    void new_forward(const std::string &host, const std::string &port);
    void new_dumpfile(const std::string &fname);
    void new_dumpfile(int fd);
    void new_textfile(const std::string &fname);
    void new_textfile(int fd);
    MsgQueue* new_msgqueue();
    Message* retain_message(const std::string &tag);
    bool emit_msg(Message *msg);
    const std::string& errmsg() const { return this->errmsg_; }
    void set_queue_limit(size_t limit);
    void set_tag_prefix(const std::string &prefix);
    bool is_connected();
  };

}


#endif   // __SRC_FLUENT_LOGGER_H__
