add_executable(TestCases GoldenTester.c)
target_link_libraries(TestCases PRIVATE blibs)
set_target_properties(TestCases PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)