#!/usr/bin/env zsh

set -x

export LD_LIBRARY_PATH=/home/feid/dev/amqp_cpp/lib:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=/home/feid/dev/openssl/include:/home/feid/dev/amqp_cpp/include

compile_t1()
{
    g++ t1_send.cpp rmq_handler.h -o t1_send -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t1_receive.cpp rmq_handler.h -o t1_receive -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

compile_t2()
{
    g++ t2_new_task.cpp rmq_handler.h -o t2_new_task -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t2_worker.cpp rmq_handler.h -o t2_worker -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

compile_t3()
{
    g++ t3_emit_log.cpp rmq_handler.h -o t3_emit_log -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t3_receive_logs.cpp rmq_handler.h -o t3_receive_logs -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

compile_t4()
{
    g++ t4_emit_log_direct.cpp rmq_handler.h -o t4_emit_log_direct -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t4_receive_logs_direct.cpp rmq_handler.h -o t4_receive_logs_direct -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

compile_t5()
{
    g++ t5_emit_log_topic.cpp rmq_handler.h -o t5_emit_log_topic -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t5_receive_logs_topic.cpp rmq_handler.h -o t5_receive_logs_topic -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

compile_t6()
{
    g++ t6_rpc_server.cpp rmq_handler.h -o t6_rpc_server -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
    g++ t6_rpc_client.cpp rmq_handler.h -o t6_rpc_client -luv -L/home/feid/dev/amqp_cpp/lib -lamqpcpp -lpthread -ldl
}

if [[ $# -eq 0 ]]; then
    compile_t1
    compile_t2
    compile_t3
    compile_t4
    compile_t5
    compile_t6
elif [[ $# -eq 1 ]]; then
    if [[ $1 == '1' ]]; then
        compile_t1
    elif [[ $1 == '2' ]]; then
        compile_t2
    elif [[ $1 == '3' ]]; then
        compile_t3
    elif [[ $1 == '4' ]]; then
        compile_t4
    elif [[ $1 == '5' ]]; then
        compile_t5
    elif [[ $1 == '6' ]]; then
        compile_t6
    else
        echo "nothing to compile"
    fi
else
    echo "nothing to compile"
fi
