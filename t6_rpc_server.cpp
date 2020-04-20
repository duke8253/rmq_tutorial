#include <chrono>
#include <thread>

#include "rmq_handler.h"

int fib(int n)
{
  if (n < 0) {
    return -1;
  } else if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}


int main(int argc, char const *argv[])
{
  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  // callback function that is called when the consume operation starts
  auto start_cb = [](const std::string &tag) {
    std::cout << "consume operation started" << std::endl;
  };

  // callback function that is called when the consume operation failed
  auto error_cb = [](const char *msg) {
    std::cout << "consume operation failed" << std::endl;
  };

  // callback operation when a message was received
  auto message_cb = [&channel](const AMQP::Message &msg, uint64_t tag, bool redelivered) {
    std::string str(msg.body(), msg.bodySize());
    std::cout << "message received: fib(" << str << ")" << std::endl;

    std::string reply_str = std::to_string(fib(std::stoi(str)));
    AMQP::Envelope env(reply_str.c_str(), reply_str.length());
    env.setCorrelationID(msg.correlationID());

    channel.publish("", msg.replyTo(), env);
    channel.ack(tag);
  };


  channel.declareQueue("rpc_queue");

  channel.consume("")
      .onReceived(message_cb)
      .onSuccess(start_cb)
      .onError(error_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  connection.close();

  return 0;
}
