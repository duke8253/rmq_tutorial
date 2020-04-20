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

  auto exchange_success_cb = [msg, &channel, &loop]() {
    channel.publish("logs", "", msg);
    std::cout << "Sent: " << msg << std::endl;
    uv_stop(loop);
  };

  channel.declareExchange("logs", AMQP::fanout)
    .onSuccess(exchange_success_cb);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}
