cmake_minimum_required( VERSION 3.5 )
project( MyFirstAIBot C CXX )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Filters
include( Filters.cmake )

SET( AIBOT_BUILDFILES
    CleanAll.bat
    CMakeLists.txt
    Filters.cmake
    GenSubmit.bat
    GenSubmit.py
    PlayLocalMatch.bat
    PlayLocalMatch.py
    rebuild_vis_sln_x64.bat
)

SET( AIBOT_HEADERS
    Headers/PathFinding/PathFinder.h

    Headers/Core/Coordinates.h
    Headers/Core/Object.h
    Headers/Core/LevelData.h
    Headers/Core/Agent.h
    Headers/Core/AgentState.h

    Headers/Utils/Utils.h
    Headers/Utils/VerboseAssertion.h
    Headers/Utils/CoordUtils.h

    Headers/Systems/ObjectSystem.h
    Headers/Systems/TileSystem.h
    Headers/Systems/Locator.h
    Headers/Systems/AgentSystem.h
    Headers/Systems/ScoreSystem.h

    Headers/Framework/AIBot.h
    Headers/Framework/BotLogicIF.h
    Headers/Framework/ConfigData.h
    Headers/Framework/Globals.h
    Headers/Framework/InitData.h
    Headers/Framework/Logger.h
    Headers/Framework/MyBotLogic.h
    Headers/Framework/TurnData.h

    Headers/DllStuff/Interop_API.h
    Headers/DllStuff/stdafx.h
    Headers/DllStuff/targetver.h
)

SET( AIBOT_SRC
    Sources/PathFinding/PathFinder.cpp

    Sources/Core/LevelData.cpp
    Sources/Core/AgentState.cpp

    Sources/Systems/ObjectSystem.cpp
    Sources/Systems/TileSystem.cpp
    Sources/Systems/AgentSystem.cpp
    Sources/Systems/ScoreSystem.cpp

    Sources/Framework/AIBot.cpp
    Sources/Framework/Logger.cpp
    Sources/Framework/MyBotLogic.cpp

    Sources/DllStuff/dllmain.cpp
    Sources/DllStuff/Interop_API.cpp
    Sources/DllStuff/stdafx.cpp
)

SET(jobfiles "${AIBOT_HEADERS};${AIBOT_SRC};${AIBOT_BUILDFILES}")
file(WRITE jobfiles.txt "${jobfiles}")

add_library(myfirstaibot SHARED ${AIBOT_SRC} ${AIBOT_HEADERS})

target_include_directories(myfirstaibot
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers
)

CREATE_VISUAL_STUDIO_FILTERS(AIBOT)