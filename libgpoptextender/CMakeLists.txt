# Copyright (c) 2015, Pivotal Software, Inc. All Rights Reserved.

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

add_library(gpoptextender ${srcs})


target_include_directories(gpoptextender
  SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/libgpdbcost/include"
    "${PROJECT_SOURCE_DIR}/libgpopt/include"
    "${PROJECT_SOURCE_DIR}/libnaucrates/include"
    "${PROJECT_BINARY_DIR}/libgpos/include"
    "${CMAKE_SOURCE_DIR}/integration/CExpressionToBOSS/include"
    "${CMAKE_SOURCE_DIR}/integration/BOSSToCExpression/include"
    ${CMAKE_SOURCE_DIR}/BOSS/Source
)


target_link_libraries(gpoptextender
                      PRIVATE
                      ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
                      gpos
                      ${XERCES_LIBRARIES}
                      ${GPORCA_SOLARIS_EXTRA_LIBS})

set_target_properties(gpoptextender PROPERTIES
                      CXX_STANDARD        17
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS      OFF
                      SOVERSION ${GPORCA_ABI_VERSION}
                      VERSION ${GPORCA_VERSION_STRING})

# Prevent linker errors from undefined symbols on OSX.
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(gpoptextender PROPERTIES
                        LINK_FLAGS "-undefined dynamic_lookup")
endif()


install(TARGETS gpoptextender DESTINATION lib)
install(DIRECTORY include/gpoptextender DESTINATION include)
