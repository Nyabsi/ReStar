# Extra CMake bindings for 3rdparty libraries that don't have them.

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

add_library(ITE_FW STATIC IMPORTED GLOBAL)

set_target_properties(ITE_FW PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/ITE_FW/lib/DK3_ITE_FW.lib"
)

target_include_directories(ITE_FW INTERFACE
    ${CMAKE_SOURCE_DIR}/3rdparty/ITE_FW/inc
)

add_library(StarVR_API STATIC IMPORTED GLOBAL)

set_target_properties(StarVR_API PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/StarVR/lib/x64/starvr_api.lib"
)

target_include_directories(StarVR_API INTERFACE
    ${CMAKE_SOURCE_DIR}/3rdparty/StarVR/inc
)

add_library(Tobii STATIC IMPORTED GLOBAL)

set_target_properties(Tobii PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/Tobii/lib/tobii_stream_engine.lib"
)

target_include_directories(Tobii INTERFACE
    ${CMAKE_SOURCE_DIR}/3rdparty/Tobii/include
)
