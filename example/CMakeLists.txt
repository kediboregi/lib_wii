INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../src/)

ADD_EXECUTABLE(wiicpp-example example.cpp)
TARGET_LINK_LIBRARIES(wiicpp-example wii)
INSTALL(TARGETS wiicpp-example DESTINATION /usr/local/bin)