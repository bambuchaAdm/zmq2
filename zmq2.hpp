#ifndef __ZMQ_HELPER_HPP__
#define __ZMQ_HELPER_HPP__
/**
   Copyright (C) 2012 Łukasz "BamBucha" Dubiel <bambucha@v-lo.krakow.pl>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <zmq.h>
#include <exception>
#include <string>
#include <iostream>

namespace zmq
{

  class ZMQException : public std::exception
  {


    int errnum;

  public:

    ZMQException() : errnum (zmq_errno())
    {}

    virtual const char * what() const throw ()
    {
      return zmq_strerror(errnum);
    }

    int num() const
    {
      return errnum;
    }
  };

  class Message
  {
    friend class Socket;
    
    struct MessagePair
    {
      int refs;
      zmq_msg_t message;
    };

    MessagePair * msg;
    
    void close();
  public:

    Message();
    explicit Message(size_t);
    Message(void *,size_t, zmq_free_fn *, void * = 0);
    Message(std::string &);
    Message(const Message &);
    
    ~Message();

    void move(Message&);    
    void copy(Message&);

    const void * data();
    size_t size();

    operator std::string();
    void operator=(const Message &);

    friend std::ostream& operator<<(std::ostream&, Message&);
  };

  std::ostream& operator<<(std::ostream&, Message& );


  enum SocketType{
    PAIR = ZMQ_PAIR,
    PUB = ZMQ_PUB,
    SUB = ZMQ_SUB,
    REQ = ZMQ_REQ,
    REP = ZMQ_REP,
    DEALER = ZMQ_DEALER,
    ROUTER = ZMQ_ROUTER,
    PULL = ZMQ_PULL,
    PUSH = ZMQ_PUSH
  };


  class Context
  {
    friend class Socket;

    void * context;

    void close();

  public:
    
    Context(int);
    ~Context();
  };

  enum TransportOptions {
    NORMAL=0,
    NOBLOCK = ZMQ_NOBLOCK,
    SNDMORE = ZMQ_SNDMORE
  };

  class Socket 
  {
    friend class Message;

    void * socket;
    void close();

  public:
    Socket(Context&,SocketType);

    ~Socket();
    
    void setsockopt(int option, const void * optval, size_t optvallen);
    void getsockopt(int option, void * optval, size_t optvallen); 

    void bind(const char *);
    void connect(const char *);
    void send(Message&,TransportOptions = NORMAL);
    void recv(Message&,TransportOptions = NORMAL);
    Message recv(TransportOptions = NORMAL);
  };
}

#endif /* __ZMQ_HELPER_HPP__ */
