

file (GLOB HE_SOURCE 
  "${HE_ROOT_PATH}/src/he/core/*.c"
  "${HE_ROOT_PATH}/src/he/graphics/*.c"
  "${HE_ROOT_PATH}/src/he/graphics/*.cpp"
)
add_library(He_Core STATIC ${HE_SOURCE})
target_include_directories(He_Core PUBLIC
    	${HE_ROOT_PATH}/include
)
