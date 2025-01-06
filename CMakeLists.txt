cmake_minimum_required(VERSION 3.30)
project(Exam06 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_executable(Exam06 mini_serv.c)
