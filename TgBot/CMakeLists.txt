cmake_minimum_required(VERSION 3.10)
project(TelegramBot)

set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

add_definitions(-D_WIN32_WINNT=0x0601)

set(TgBot_DIR "C:/dev/vcpkg/installed/x64-windows/share/tgbot-cpp")
find_package(TgBot CONFIG REQUIRED)

set(SOURCES
    main.cpp
    voice_recognition.cpp
    sorting_algorithms.cpp
)

add_executable(TelegramBot ${SOURCES})
target_link_libraries(TelegramBot PRIVATE TgBot::TgBot)