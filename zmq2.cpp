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


#include "helper.hpp"

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
    zmq_msg_init(&message);
  }

  Message::Message(size_t size)
  {
    if(!zmq_msg_init_size(&message,size))
      throw ZMQException();
  }

  Message::Message(void * data, size_t size, zmq_free_fn * ffn, void * hing)
  {
    zmq_msg_init_data(&message,data,size,ffn,hing);
  }

  Message::Message(std::string & arg)
  {
    char * buff = new char[arg.size()+1];
    strcpy(buff,arg.c_str());
    zmq_msg_init_data(&message,buff,arg.size(),free_new_buffer,0);
  }

  Message::~Message()
  {
    try{
      close();
    }
    catch(ZMQException e){
      std::cerr <<e.what()<<std::endl;
    }
  }

  void Message::close()
  {
    if(!zmq_msg_close(&message))
      throw ZMQException();
  }

  void Message::move(Message & dest)
  {
    zmq_msg_move(&dest.message,&message);
  }

  void Message::copy(Message & dest)
  {
    zmq_msg_copy(&dest.message,&message);
  }

  void * Message::data()
  {
    return zmq_msg_data(&message);
  }

  size_t Message::size()
  {
    return zmq_msg_size(&message);
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
    if(zmq_bind(socket,address))
      throw ZMQException();
  }

  void Socket::connect(const char * address)
  {
    if(zmq_connect(socket,address))
      throw ZMQException();
  }

  void Socket::send(Message & message, TransportOptions options)
  {
    if(!zmq_send(socket,&message.message,options))
      throw ZMQException();
  }

  void Socket::recv(Message & message, TransportOptions options)
  {
    if(!zmq_recv(socket,&message.message,options))
      throw ZMQException();
  }

  Message & Socket::recv(TransportOptions options)
  {
    Message message;
    if(!zmq_recv(socket,&message.message,options))
      throw ZMQException();
  }

  
}
