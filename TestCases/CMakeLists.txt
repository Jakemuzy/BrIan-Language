# Get list of test directories
file(GLOB TEST_DIRS "*")

foreach(test_dir ${TEST_DIRS})
    if(IS_DIRECTORY ${test_dir})
        get_filename_component(test_module ${test_dir} NAME)
        
        # Find all .c files in this test module
        file(GLOB test_sources "${test_dir}/*.c")
        
        foreach(test_file ${test_sources})
            get_filename_component(test_name ${test_file} NAME_WE)
            
            # Create executable from test file + all library sources
            add_executable(${test_name} ${test_file} ${LIB_SOURCES})

            # llvm crap
            llvm_map_components_to_libnames(llvm_libs support core irreader)
            target_link_libraries(${test_name} PRIVATE ${llvm_libs})
            target_link_libraries(${test_name} PRIVATE stdc++ gcc_s)

            set_property(TARGET ${test_name} PROPERTY C_STANDARD 99)
            set_property(TARGET ${test_name} PROPERTY C_STANDARD_REQUIRED ON)

            # Register test
            add_test(NAME ${test_module}/${test_name} COMMAND ${test_name})
            
            message(STATUS "Adding test: ${test_name}")
        endforeach()
    endif()
endforeach()

enable_testing()