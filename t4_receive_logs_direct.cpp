#include <chrono>
#include <thread>

#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " [info] [warning] [error]" << std::endl;
    return 1;
  }

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
    std::cout << "message received: " << "[" << msg.routingkey() << "] " << str << std::endl;
  };

  auto queue_success_cb = [&](const std::string &name, uint32_t message_count, uint32_t consumer_count)
  {
    for (int i = 1; i < argc; ++i) {
      channel.bindQueue("direct_logs", name, argv[i]);
    }
    channel.consume(name, AMQP::noack)
      .onReceived(message_cb)
      .onSuccess(start_cb)
      .onError(error_cb);
  };

  auto exchange_success_cb = [&]() {
    std::cout << "Exchange created." << std::endl;
    channel.declareQueue(AMQP::exclusive)
      .onSuccess(queue_success_cb);
  };

  channel.declareExchange("direct_logs", AMQP::direct)
    .onSuccess(exchange_success_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  connection.close();

  return 0;
}
