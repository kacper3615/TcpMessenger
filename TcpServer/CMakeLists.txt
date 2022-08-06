cmake_minimum_required(VERSION 3.8)

project(TcpServer)

set(CMAKE_CXX_STANDARD 17)

find_package (Git)
if (GIT_FOUND)
      message("git found: ${GIT_EXECUTABLE} in version ${GIT_VERSION_STRING}")
endif (GIT_FOUND)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB tcpServer_src CONFIGURE_DEPENDS "src/*.hpp" "src/*.cpp" "src/*/*.hpp" "src/*/*.cpp" "src/*/*/*.hpp" "src/*/*/*.cpp")

add_executable(TcpServer ${tcpServer_src})
target_link_libraries(TcpServer Threads::Threads)

