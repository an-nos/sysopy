cmake_minimum_required(VERSION 3.15)
project(sysopy C)

set(CMAKE_C_STANDARD 11)

add_executable(sysopy main.c cw01/zad3b/main.c cw01/zad3b/main_dyn.c cw02/zad1/zad1.c cw03/zad2/main.c cw04/zad1/main.c cw04/zad3/main.c cw04/zad2/exec_test.c cw04/zad4a/sender.c cw04/zad4a/catcher.c cw05/zad1/main.c cw05/zad2/main.c cw05/zad3/consumer.c cw05/zad3/producer.c cw05/zad3/main.c cw06/zad1/server.c cw06/zad1/client.c cw06/zad1/common.h cw06/zad2/client.c cw06/zad2/server.c cw06/zad2/common.h cw07/zad1/common.h cw07/zad1/worker1.c cw07/zad1/main.c cw07/zad1/worker2.c cw07/zad1/worker3.c cw07/zad2/main.c cw07/zad2/worker1.c cw07/zad2/worker2.c cw07/zad2/worker3.c cw07/zad2/common.h cw08/zad1/main.c)