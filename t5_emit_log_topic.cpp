#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  std::string routing_key = "";
  std::string msg = "";

  if (argc > 2) {
    routing_key = argv[1];
    for (int i = 2; i < argc; ++i) {
      if (i != 2) {
        msg += " ";
      }
      msg += argv[i];
    }

  } else {
    routing_key = "anonymous.info";
    msg = "Hello, world!";
  }

  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  auto exchange_success_cb = [routing_key, msg, &channel, &loop]() {
    channel.publish("topic_logs", routing_key, msg);
    std::cout << "Sent: " << "[" << routing_key << "] " << msg << std::endl;
    uv_stop(loop);
  };

  channel.declareExchange("topic_logs", AMQP::topic)
    .onSuccess(exchange_success_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
