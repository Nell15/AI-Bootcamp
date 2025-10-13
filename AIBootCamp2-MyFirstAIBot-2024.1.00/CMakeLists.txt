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
    Headers/Algorithm/PathFinding/PathFinder.h
    Headers/Algorithm/Core/Coordinates.h
    Headers/Algorithm/Core/Object.h
    Headers/Algorithm/Core/LevelData.h
    Headers/Algorithm/Core/Agent.h
    Headers/Algorithm/Utils/Utils.h
    Headers/Algorithm/Utils/VerboseAssertion.h
    Headers/Algorithm/Utils/CoordUtils.h
    Headers/Algorithm/Core/AgentState.h

    Headers/Systems/ObjectSystem.h
    Headers/Systems/TileSystem.h
    Headers/Systems/Locator.h
    Headers/Systems/AgentSystem.h

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
    Sources/Algorithm/PathFinding/PathFinder.cpp
    Sources/Algorithm/Core/Agent.cpp
    Sources/Algorithm/Core/LevelData.cpp
    Sources/Algorithm/Core/AgentState.cpp

    Sources/Systems/ObjectSystem.cpp
    Sources/Systems/TileSystem.cpp
    Sources/Systems/AgentSystem.cpp

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