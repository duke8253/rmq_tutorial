#include "rmq_handler.h"


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
    std::cout << "message received: " << str << std::endl;
  };

  channel.declareQueue("hello", AMQP::autodelete);
  channel.consume("hello", AMQP::noack)
    .onReceived(message_cb)
    .onSuccess(start_cb)
    .onError(error_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  connection.close();

  return 0;
}
