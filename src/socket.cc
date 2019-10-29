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

#ifdef _MSC_VER
#include <winsock2.h>
#include "windows.h"
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "./fluent/socket.hpp"
#include "./debug.h"


namespace fluent {
  Socket::Socket(const std::string &host, const std::string &port) :
    host_(host), port_(port), is_connected_(false) {
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
  }
  Socket::~Socket() {
#ifdef _MSC_VER
	  closesocket(this->sock_);
#else
    ::close(this->sock_);
#endif
  }
  bool Socket::connect() {
    const bool DBG = false;
    bool rc = true;
    debug(DBG, "host=%s, port=%s", this->host_.c_str(), this->port_.c_str());

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    int r;
    if (0 != (r = ::getaddrinfo(this->host_.c_str(), this->port_.c_str(),
                                &hints, &result))) {
      this->errmsg_.assign(gai_strerror(r));
      return false;
      // throw Exception("getaddrinfo error: " + errmsg);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
      this->sock_ = ::socket(rp->ai_family, rp->ai_socktype,
                             rp->ai_protocol);
      if (this->sock_ == -1) {
        continue;
      }

      if (::connect(this->sock_, rp->ai_addr, rp->ai_addrlen) == 0) {
        char buf[INET6_ADDRSTRLEN];
        struct sockaddr_in *addr_in = (struct sockaddr_in *) rp->ai_addr;
        ::inet_ntop(rp->ai_family, &addr_in->sin_addr.s_addr, buf,
                    rp->ai_addrlen);

        this->is_connected_ = true;
        debug(DBG, "connected to %s", buf);
        break;
      }
    }

    if (rp == NULL) {
      // throw Exception("no available address for " + this->host_);
      rc = false;
    }

    freeaddrinfo(result);
    return rc;
  }
  bool Socket::send(void *data, size_t len) {
#ifdef _WIN32
    if (SOCKET_ERROR == ::send(this->sock_, (char*)data, len, 0)) {
      LPTSTR err = nullptr;
      auto error = WSAGetLastError();
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
              nullptr, error, 0, (LPTSTR)&err, 0, nullptr);
      this->errmsg_.assign(err);
#else
    if (0 > ::write(this->sock_, data, len)) {
      this->errmsg_.assign(strerror(errno));
#endif // _WIN32
      debug(false, "err: %s", this->errmsg_.c_str());
      this->is_connected_ = false;
#ifdef _MSC_VER
	  closesocket(this->sock_);
#else
	  ::close(this->sock_);
#endif
      return false;
    }
    return true;
  }

}
