#include <uv.h>
#include <amqpcpp.h>
#include <amqpcpp/libuv.h>
#include <iostream>
#include <cstdlib>
#include <string>

/**
 *  Custom handler
 */
class MsgHandler : public AMQP::LibUvHandler
{
private:
  /**
   *  Method that is called when a connection error occurs
   *  @param  connection
   *  @param  message
   */
  virtual void onError(AMQP::TcpConnection *connection, const char *message) override
  {
      std::cout << "error: " << message << std::endl;
  }

  /**
   *  Method that is called when the TCP connection ends up in a connected state
   *  @param  connection  The TCP connection
   */
  virtual void onConnected(AMQP::TcpConnection *connection) override
  {
      std::cout << "connected" << std::endl;
  }

public:
  /**
   *  Constructor
   *  @param  uv_loop
   */
  MsgHandler(uv_loop_t *loop) : AMQP::LibUvHandler(loop) {}

  /**
   *  Destructor
   */
  virtual ~MsgHandler() = default;
};
