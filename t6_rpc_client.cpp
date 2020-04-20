#include <chrono>

#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  std::string fib_num = "";

  if (argc == 2) {
    fib_num = argv[1];
  } else {
    fib_num = "5";
  }

  std::string correlation = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch()
  ).count());

  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  auto queue_success_cb = [fib_num, correlation, &channel](const std::string &name, uint32_t message_count, uint32_t consumer_count) {
    AMQP::Envelope env(fib_num.c_str(), fib_num.length());
    env.setCorrelationID(correlation);
    env.setReplyTo(name);
    channel.publish("", "rpc_queue", env);
    std::cout << "Requesting fib(" << fib_num << ")"<< std::endl;
  };

  auto message_cb = [fib_num, correlation, &channel, &loop](const AMQP::Message &msg, uint64_t tag, bool redelivered) {
    if (msg.correlationID() != correlation) {
      return;
    }
    std::string str(msg.body(), msg.bodySize());
    std::cout << "fib(" << fib_num << ") = " << str << std::endl;
    uv_stop(loop);
  };

  channel.declareQueue(AMQP::exclusive).onSuccess(queue_success_cb);
  channel.consume("", AMQP::noack).onReceived(message_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
