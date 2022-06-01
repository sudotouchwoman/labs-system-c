set(CLIENT_SOURCES src/client.c src/urls.c)
add_library(client SHARED ${CLIENT_SOURCES})
target_include_directories(client PUBLIC inc/)

add_executable(browser main.c)
target_include_directories(OS PUBLIC "${PROJECT_FOLDER}/socket/inc/")
target_link_libraries(browser client)
