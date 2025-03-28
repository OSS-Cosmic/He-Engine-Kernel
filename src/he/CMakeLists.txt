

file (GLOB HE_SOURCE 
  "${HE_ROOT_PATH}/src/he/core/*.c"
  "${HE_ROOT_PATH}/src/he/graphics/*.c"
  "${HE_ROOT_PATH}/src/he/graphics/*.cpp"
)
add_library(He_Core STATIC ${HE_SOURCE})
target_link_libraries(He_Core PRIVATE stb)
target_include_directories(He_Core PUBLIC
    	${HE_ROOT_PATH}/include
)

function(add_core_test) 
	cmake_parse_arguments(
	   P_ARGS
	   ""
	   "NAME"
	   "SRCS"
	   ${ARGN}
	)
	add_executable(${P_ARGS_NAME} ${P_ARGS_SRCS} ${CMOCKA_SRC_FILES})
	target_include_directories(${P_ARGS_NAME} PRIVATE ${UTEST_INCLUDE_DIR} ".")
	target_link_libraries(${P_ARGS_NAME} PRIVATE He_Core)
	target_link_libraries(${P_ARGS_NAME} PRIVATE utest)
	set_output_dir(${P_ARGS_NAME} test)
endfunction()

if(BUILD_UNIT_TEST)
    add_core_test(NAME str_test SRCS 
		"./core/test/str_test.c")
    add_core_test(NAME str_utf_test SRCS 
		"./core/test/str_utf_test.c")
    add_core_test(NAME scratch_alloc_test SRCS 
		"./core/test/scratch_alloc_test.c")
    add_core_test(NAME alloc_test SRCS 
		"./core/test/alloc_test.c")
endif()



