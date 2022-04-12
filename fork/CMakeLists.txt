set(SOURCES src/kernel.c src/services.c src/utils.c)
add_library(kernel STATIC ${SOURCES})
target_include_directories(kernel PUBLIC inc/)

add_executable(main main.c)
target_include_directories(main PUBLIC "${PROJECT_FOLDER}/fork/inc/")
target_link_libraries(main kernel)

add_executable(test test.c)
target_include_directories(test PUBLIC "${PROJECT_FOLDER}/fork/inc/")
target_link_libraries(test kernel)
