#ifndef __ZMQ_HELPER_HPP__
#define __ZMQ_HELPER_HPP__

#include <zmq.h>
#include <exception>
#include <string>

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

    zmq_msg_t message;

    Message(const Message &);
    void operator=(const Message &);

    void close();

  public:
    
    Message();
    Message(size_t);
    Message(void *,size_t, zmq_free_fn *, void * = 0);
    Message(std::string &)
    ~Message();

    void move(Message&);    
    void copy(Message&);

    void * data();
    size_t size();

    operator std::string();

  };

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
    Message& recv(TransportOptions = NORMAL);
  };
}

#endif /* __ZMQ_HELPER_HPP__ */
