CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

CMAKE_POLICY(SET CMP0048 NEW)
PROJECT(gltf-viewer-tutorial VERSION 0.0.1)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

OPTION(GLMLV_USE_BOOST_FILESYSTEM "Use boost for filesystem library instead of experimental std lib" OFF)

SET(IMGUI_DIR imgui-1.74)
SET(GLFW_DIR glfw-3.3.1)
SET(GLM_DIR glm-0.9.9.7)
SET(GLAD_DIR glad)
SET(TINYGLTF_DIR tinygltf-bcf2ce586ee8bf2a2a816afa6bfe2f8692ba6ac2)
SET(ARGS_DIR args-6.2.2)

# Add GLFW subdirectory, set some options to OFF by default (the user can still enable them by modifying its CMakeCache.txt)
OPTION(GLFW_BUILD_DOCS OFF)
OPTION(GLFW_BUILD_TESTS OFF)
OPTION(GLFW_BUILD_EXAMPLES OFF)
ADD_SUBDIRECTORY(third-party/${GLFW_DIR})

IF (${CMAKE_VERSION} VERSION_LESS "3.10.0")
    SET(OpenGL_GL_PREFERENCE LEGACY)
ELSE ()
    SET(OpenGL_GL_PREFERENCE GLVND)
ENDIF ()
FIND_PACKAGE(OpenGL REQUIRED)

IF (GLMLV_USE_BOOST_FILESYSTEM)
    FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
ENDIF ()

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE(c2ba-glsl-shaders)
INCLUDE(c2ba-copy-dll-post-build)
INCLUDE(c2ba-assets-directory)

IF (MSVC)
    SET(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/shaders)
    SET(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/assets)
ELSE ()
    SET(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/shaders)
    SET(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/assets)
ENDIF ()

SET(
    IMGUI_SRC_FILES
    third-party/${IMGUI_DIR}/imgui.cpp
    third-party/${IMGUI_DIR}/imgui_demo.cpp
    third-party/${IMGUI_DIR}/imgui_draw.cpp
    third-party/${IMGUI_DIR}/imgui_widgets.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
)

SET(THIRD_PARTY_SRC_FILES ${IMGUI_SRC_FILES} third-party/${GLAD_DIR}/src/glad.c)

SET(LIBRARIES ${OPENGL_LIBRARIES} glfw)

IF (GLMLV_USE_BOOST_FILESYSTEM)
    SET(LIBRARIES ${LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
ELSE ()
    SET(CXXFLAGS ${CXXFLAGS} std=c++14)
    
    IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        SET(CXXFLAGS ${CXXFLAGS} std=c++17)
        SET(LIBRARIES ${LIBRARIES} stdc++fs)
        IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "8.0.0")
            SET(USE_STD_FILESYSTEM 1)
        ENDIF ()
    
    ELSEIF (CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
        IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0")
            SET(LIBRARIES ${LIBRARIES} stdc++fs)
        ELSE ()
            SET(CXXFLAGS ${CXXFLAGS} std=c++17)
            SET(USE_STD_FILESYSTEM 1)
        ENDIF ()
    ENDIF ()
ENDIF ()

SOURCE_GROUP("glsl" REGULAR_EXPRESSION ".*/*.glsl")
SOURCE_GROUP("third-party" REGULAR_EXPRESSION "third-party/*.*")

FILE(GLOB APP_DIRECTORIES "apps/*")

FOREACH (DIR ${APP_DIRECTORIES})
    GET_FILENAME_COMPONENT(APP ${DIR} NAME)
    
    SET(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps/${APP}/src/)
    SET(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps/${APP}/include/)
    SET(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps/${APP}/shaders/)
    SET(ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps/${APP}/assets/)
    
    FILE(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}*.cpp)
    FILE(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}*.hpp)
    FILE(GLOB_RECURSE SHADER_FILES ${SHADER_DIR}*.glsl)
    FILE(GLOB_RECURSE ASSET_FILES ${ASSET_DIR}*)
    
    ADD_EXECUTABLE(${APP} ${SOURCE_FILES} ${INCLUDE_FILES} ${SHADER_FILES} ${ASSET_FILES} ${THIRD_PARTY_SRC_FILES})
    
    IF (USE_STD_FILESYSTEM)
        TARGET_COMPILE_DEFINITIONS(${APP} PUBLIC USE_STD_FILESYSTEM)
    ENDIF ()
    
    IF (GLMLV_USE_BOOST_FILESYSTEM)
        TARGET_INCLUDE_DIRECTORIES(${APP} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
        TARGET_COMPILE_DEFINITIONS(${APP} PUBLIC GLMLV_USE_BOOST_FILESYSTEM)
    ENDIF ()
    
    TARGET_COMPILE_DEFINITIONS(${APP} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD GLM_ENABLE_EXPERIMENTAL)
    
    TARGET_INCLUDE_DIRECTORIES(
        ${APP} SYSTEM
        PRIVATE
        ${INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        third-party/${GLFW_DIR}/include
        third-party/${GLM_DIR}
        third-party/${GLAD_DIR}/include
        third-party/${IMGUI_DIR}
        third-party/${IMGUI_DIR}/examples
        third-party/${TINYGLTF_DIR}/include
        third-party/${ARGS_DIR}
        lib/include
    )
    
    IF (${CMAKE_VERSION} VERSION_LESS "3.8.0")
        SET_PROPERTY(TARGET ${APP} PROPERTY CXX_STANDARD 14)
    ELSE ()
        SET_PROPERTY(TARGET ${APP} PROPERTY CXX_STANDARD 17)
    ENDIF ()
    
    TARGET_LINK_LIBRARIES(${APP} ${LIBRARIES})
    
    INSTALL(TARGETS ${APP} DESTINATION .)

    TARGET_COMPILE_OPTIONS(
        ${APP} PRIVATE
        -g
        -O2
    )
    IF (CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang" AND CMAKE_C_COMPILER_ID MATCHES "[Cc]lang")
        IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "9.0.0")
            TARGET_COMPILE_OPTIONS(
                ${APP} PRIVATE
                -Weverything
                -Wfatal-errors
                -Wno-float-conversion
                -Wno-padded
                -Wno-shadow
                -Wno-shadow-field
                -Wno-unused-macros
                -Wno-old-style-cast
                -Wno-extra-semi-stmt
                -Wno-sign-conversion
                -Wno-unused-variable
                -Wno-double-promotion
                -Wno-unused-parameter
                -Wno-reserved-id-macro
                -Wno-unused-private-field
                -Wno-unused-const-variable
                -Wno-global-constructors
                -Wno-exit-time-destructors
                -Wno-zero-as-null-pointer-constant
                -Wno-c++98-compat
                -Wno-c++98-compat-pedantic
                -Wno-c++98-c++11-c++14-compat
            )
        ENDIF ()
    ENDIF ()
    
    
    C2BA_ADD_SHADER_DIRECTORY(${SHADER_DIR} ${SHADER_OUTPUT_PATH}/${APP})
    C2BA_ADD_ASSETS_DIRECTORY(${ASSET_DIR} ${ASSET_OUTPUT_PATH}/${APP})
    
    INSTALL(DIRECTORY ${DIR}/shaders/ DESTINATION shaders/${APP} FILES_MATCHING PATTERN "*.glsl")
    
    IF (EXISTS ${DIR}/assets/)
        INSTALL(DIRECTORY ${DIR}/assets/ DESTINATION assets/${APP})
    ENDIF ()

ENDFOREACH ()
