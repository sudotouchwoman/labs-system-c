set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(THREAD_SOURCES src/worker.c src/manager.c src/grid.c)
add_library(grid SHARED ${THREAD_SOURCES})
target_include_directories(grid PUBLIC inc/)
target_link_libraries(grid Threads::Threads)

add_executable(RC main.c)
target_include_directories(RC PUBLIC "${PROJECT_FOLDER}/thread/inc/")
target_link_libraries(RC grid)
