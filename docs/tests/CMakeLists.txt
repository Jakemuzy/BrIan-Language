add_executable(tester GoldenTester.c)
target_link_libraries(tester PRIVATE blibs)
set_target_properties(tester PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)