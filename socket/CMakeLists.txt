set(CLIENT_SOURCES src/client.c src/iotty.c src/links.c)
add_library(client SHARED ${CLIENT_SOURCES})
target_include_directories(client PUBLIC inc/)

add_executable(browse main.c)
target_include_directories(OS PUBLIC "${PROJECT_FOLDER}/socket/inc/")
target_link_libraries(browse client)
