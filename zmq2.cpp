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
*/


#include "zmq2.hpp"

#include <iostream>
#include <cstring>
#include <new>

namespace zmq
{

  static void 
  free_new_buffer(void * data,void * hint)
  {
    delete[] (char*)data;
  }

  Message::Message()
  {
    msg = new MessagePair;
    zmq_msg_init(&msg->message);
  }

  Message::Message(size_t size)
  {
    msg = new MessagePair;
    if(!zmq_msg_init_size(&msg->message,size))
    {
      delete msg;
      throw ZMQException();
    }
  }

  Message::Message(void * data, size_t size, zmq_free_fn * ffn, void * hing)
  {
    msg = new MessagePair;
    zmq_msg_init_data(&msg->message,data,size,ffn,hing);
  }

  Message::Message(const Message & old)
  {
    msg = old.msg;
    msg->refs++;
  }

  Message::Message(std::string & arg)
  {
    char * buff = new char[arg.size()+1];
    strcpy(buff,arg.c_str());
    msg = new MessagePair;
    zmq_msg_init_data(&msg->message,buff,arg.size(),free_new_buffer,0);
  }

  Message::~Message()
  {
    try{
      msg->refs--;
      if(msg->refs)
	close();
    }
    catch(ZMQException e){
      std::cerr <<e.what()<<std::endl;
    }
  }

  void Message::close()
  {
    if(!zmq_msg_close(&msg->message))
      throw ZMQException();
  }

  void Message::move(Message & dest)
  {
    zmq_msg_move(&dest.msg->message,&msg->message);
  }

  void Message::copy(Message & dest)
  {
    zmq_msg_copy(&dest.msg->message,&msg->message);
  }

  const void * Message::data()
  {
    return zmq_msg_data(&msg->message);
  }

  size_t Message::size()
  {
    return zmq_msg_size(&msg->message);
  }

  Message::operator std::string()
  {
    return std::string((const char *)data(),size());
  }

  Context::Context(int threads) : context(zmq_init(threads))
  {  }

  void Context::close()
  {
    if(!zmq_term(context))
      throw new ZMQException();
  }

  Context::~Context()
  {
    try{
    this->close();
    }
    catch(ZMQException e){
      std::cerr <<e.what()<<std::endl;
    }
  }

  Socket::Socket(Context & context,SocketType type)
  {
    if(!zmq_socket(context.context,type))
      throw ZMQException();
  }

  void Socket::close()
  {
    if(!zmq_close(socket))
      throw new ZMQException();
  }

  Socket::~Socket()
  {
    try{
      close();
    }
    catch(ZMQException e){
      std::cerr<<e.what()<<std::endl;
    }
  }

  void Socket::bind(const char * address)
  {
    if(!zmq_bind(socket,address))
      throw ZMQException();
  }

  void Socket::connect(const char * address)
  {
    if(!zmq_connect(socket,address))
      throw ZMQException();
  }

  void Socket::send(Message & message, TransportOptions options)
  {
    if(!zmq_send(socket,&message.msg->message,options))
      throw ZMQException();
  }

  void Socket::recv(Message & message, TransportOptions options)
  {
    if(!zmq_recv(socket,&message.msg->message,options))
      throw ZMQException();
  }

  Message Socket::recv(TransportOptions options)
  {
    Message m;
    if(!zmq_recv(socket,&m.msg->message,options))
    {
      throw ZMQException();
    }
    return m;
  } 
}
