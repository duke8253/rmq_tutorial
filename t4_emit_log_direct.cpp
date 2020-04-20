#include "rmq_handler.h"


int main(int argc, char const *argv[])
{
  std::string severity = "";
  std::string msg = "";

  if (argc > 2) {
    severity = argv[1];
    for (int i = 2; i < argc; ++i) {
      if (i != 2) {
        msg += " ";
      }
      msg += argv[i];
    }

  } else {
    severity = "info";
    msg = "Hello, world!";
  }

  auto *loop = uv_default_loop();

  MsgHandler handler(loop);

  AMQP::Address address("localhost", 5672, AMQP::Login("guest", "guest"), "/");
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);

  auto exchange_success_cb = [severity, msg, &channel, &loop]() {
    channel.publish("direct_logs", severity, msg);
    std::cout << "Sent: [" << severity << "] " << msg << std::endl;
    uv_stop(loop);
  };

  channel.declareExchange("direct_logs", AMQP::direct)
    .onSuccess(exchange_success_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
