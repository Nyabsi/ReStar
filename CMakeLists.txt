cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(openvr_restar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OpenVR_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/OpenVR)
set(Json_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/json)
set(MinHook_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/MinHook)

find_package(OpenVR REQUIRED)

# Include rest of the CMake bindings
add_subdirectory(3rdparty)

add_subdirectory(${Json_ROOT} EXCLUDE_FROM_ALL)
add_subdirectory(${MinHook_ROOT} EXCLUDE_FROM_ALL)

message(STATUS "ENABLE_VULKAN_VALIDATION = ${ENABLE_VULKAN_VALIDATION}")

add_library(driver_restar SHARED
    "src/Main.cpp"
    "src/TrackedDeviceProvider.cpp"
    "src/StarPatcher.cpp"
)

target_include_directories(driver_restar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(driver_restar PUBLIC cxx_std_20)

target_link_libraries(driver_restar
    PRIVATE
        OpenVR::API
        nlohmann_json::nlohmann_json
        ITE_FW
        StarVR_API
        minhook
        Tobii
)

set(CUSTOM_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")

add_custom_target(restar_resources)

add_custom_command(
    TARGET restar_resources POST_BUILD
    COMMENT "Copying Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CUSTOM_OUTPUT_DIR}
)

add_dependencies(driver_restar restar_resources)

add_custom_command(
    TARGET driver_restar POST_BUILD
    COMMENT "Copying driver"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:driver_restar> ${CUSTOM_OUTPUT_DIR}/bin/win64
    COMMAND_EXPAND_LISTS
)