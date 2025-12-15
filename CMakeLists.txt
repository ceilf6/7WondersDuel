cmake_minimum_required(VERSION 4.0)
project(qidaqijiduijue)

set(CMAKE_CXX_STANDARD 20)

add_executable(qidaqijiduijue main.cpp
        Board.h
        Board.cpp
        Card.cpp
        Card.h
        Commons.h
        Game.cpp
        Game.h
        Player.cpp
        Player.h
        SmartAI.cpp
        SmartAI.h)
