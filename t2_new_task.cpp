#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  std::string msg = "";
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (i != 1) {
        msg += " ";
      }
      msg += argv[i];
    }
  } else {
    msg = "Hello, world!";
  }

  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  auto success_cb = [msg, &channel, &loop](const std::string &name, uint32_t message_count, uint32_t consumer_count) {
    AMQP::Envelope env(msg.c_str(), msg.length());
    env.setDeliveryMode(2);
    std::cout << "Queue: " << name << std::endl;
    channel.publish("", "task_queue", env);
    std::cout << "Sent: " << msg << std::endl;
    uv_stop(loop);
  };

  auto finalize_cb = [&connection]() {
    std::cout << "Finalize." << std::endl;
    connection.close();
  };

  channel.declareQueue("task_queue", AMQP::durable | AMQP::passive)
    .onSuccess(success_cb)
    .onFinalize(finalize_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
