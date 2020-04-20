#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  auto success_cb = [&channel, &loop](const std::string &name, uint32_t message_count, uint32_t consumer_count) {
    std::cout << "Queue: " << name << std::endl;
    std::string msg = "Hello, world!";
    channel.publish("", "hello", msg);
    std::cout << "Sent: " << msg << std::endl;
    uv_stop(loop);
  };

  auto finalize_cb = [&connection]() {
    std::cout << "Finalize." << std::endl;
    connection.close();
  };

  channel.declareQueue("hello", AMQP::passive)
    .onSuccess(success_cb)
    .onFinalize(finalize_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
