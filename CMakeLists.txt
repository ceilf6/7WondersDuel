cmake_minimum_required(VERSION 4.0)
project(qidaqijiduijue)

set(CMAKE_CXX_STANDARD 20)

add_executable(qidaqijiduijue main.cpp
        Board.h
        Board.cpp
        Card.cpp
        Card.h
        CardData.h
        CardFactory.h
        CardFactory.cpp
        BaseGameCardFactory.h
        BaseGameCardFactory.cpp
        AgoraCardFactory.h
        AgoraCardFactory.cpp
        PantheonCardFactory.h
        PantheonCardFactory.cpp
        GameConfig.h
        GameConfig.cpp
        GameBuilder.h
        GameBuilder.cpp
        AIStrategy.h
        AIStrategy.cpp
        RandomStrategy.h
        RandomStrategy.cpp
        GreedyStrategy.h
        GreedyStrategy.cpp
        CardDecorator.h
        CardVisitor.h
        ExtensionAdapter.h
        Commons.h
        Game.cpp
        Game.h
        Player.cpp
        Player.h
        SmartAI.cpp
        SmartAI.h)
