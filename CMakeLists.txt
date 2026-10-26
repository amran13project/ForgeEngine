cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)
project(ForgeEngine VERSION 3.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
option(FORGE_BUILD_TESTS "Build Forge Engine tests" ON)
set(FORGE_APP_ICON "" CACHE FILEPATH "Optional user-provided Windows .ico file")

set(FORGE_CORE_SOURCES
    src/core/ForgeSystem.cpp
    src/platform/Environment.cpp
    src/project/ProjectManager.cpp
    src/scene/Scene.cpp
    src/assets/AssetDatabase.cpp
    src/physics/Physics.cpp
    src/ai/AI.cpp
    src/scripting/VisualScript.cpp
    src/audio/Audio.cpp
    src/network/NetworkLab.cpp
    src/profiler/Profiler.cpp
    src/build/BuildCenter.cpp
    src/doctor/ProjectDoctor.cpp
    src/recovery/Recovery.cpp
    src/plugins/PluginManager.cpp
    src/testing/TestingCenter.cpp
    src/world/WorldGenerator.cpp
    src/account/AccountService.cpp
    src/account/AccountPolicy.cpp
    src/ai_platform/ForgeAI.cpp
    src/ai_platform/AIOrchestrator.cpp
    src/publish/PublishCenter.cpp
    src/compliance/ComplianceChecker.cpp
    src/localization/LocalizationManager.cpp
    src/accessibility/AccessibilitySettings.cpp
    src/security/SecurityCenter.cpp
    src/release/ReleaseManager.cpp
    src/workspace/WorkspaceProfile.cpp
    src/input/InputSystem.cpp
    src/gameplay/GameplayToolkit.cpp
    src/animation/AnimationSystem.cpp
    src/ui/WidgetTree.cpp
    src/material/Material.cpp
    src/prefab/Prefab.cpp
    src/hotreload/HotReload.cpp
)
set(FORGE_PLATFORM_SOURCES src/platform/NativeWindow.cpp)
set(FORGE_RENDERER_SOURCES src/renderer/SoftwareRenderer.cpp)
add_library(ForgeCore STATIC ${FORGE_CORE_SOURCES})
target_include_directories(ForgeCore PUBLIC src)
if(MSVC)
    target_compile_options(ForgeCore PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(ForgeCore PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(ForgeEngine src/main.cpp ${FORGE_PLATFORM_SOURCES} src/editor/ForgeEditor.cpp ${FORGE_RENDERER_SOURCES})
add_executable(ForgeRuntime src/runtime_main.cpp ${FORGE_PLATFORM_SOURCES} src/runtime/RuntimeApp.cpp ${FORGE_RENDERER_SOURCES})
target_link_libraries(ForgeEngine PRIVATE ForgeCore)
target_link_libraries(ForgeRuntime PRIVATE ForgeCore)

if(FORGE_APP_ICON)
    if(NOT WIN32)
        message(FATAL_ERROR "FORGE_APP_ICON is supported only for Windows builds")
    endif()
    if(NOT EXISTS "${FORGE_APP_ICON}")
        message(FATAL_ERROR "FORGE_APP_ICON does not exist: ${FORGE_APP_ICON}")
    endif()
    enable_language(RC)
    file(TO_CMAKE_PATH "${FORGE_APP_ICON}" FORGE_APP_ICON_RESOURCE)
    configure_file(assets/branding/forge-icon.rc.in
                   "${CMAKE_CURRENT_BINARY_DIR}/forge-icon.rc" @ONLY)
    target_sources(ForgeEngine PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/forge-icon.rc")
    target_sources(ForgeRuntime PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/forge-icon.rc")
    target_compile_definitions(ForgeEngine PRIVATE FORGE_HAS_APP_ICON=1)
    target_compile_definitions(ForgeRuntime PRIVATE FORGE_HAS_APP_ICON=1)
endif()

foreach(target ForgeEngine ForgeRuntime)
    target_include_directories(${target} PRIVATE src)
    target_compile_definitions(${target} PRIVATE FORGE_ENGINE_VERSION="3.2.0" NOMINMAX WIN32_LEAN_AND_MEAN)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

if(WIN32)
    foreach(target ForgeEngine ForgeRuntime)
        target_link_libraries(${target} PRIVATE user32 gdi32 shell32 ole32 ws2_32)
        if(NOT MSVC)
            target_link_options(${target} PRIVATE -mconsole -Wl,--subsystem,console -Wl,--entry=mainCRTStartup)
        endif()
    endforeach()
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(ForgeEngine PRIVATE X11::X11)
    target_link_libraries(ForgeRuntime PRIVATE X11::X11)
endif()

if(FORGE_BUILD_TESTS)
    enable_testing()
    add_executable(ForgeCoreTests tests/test_core.cpp)
    target_include_directories(ForgeCoreTests PRIVATE src)
    target_link_libraries(ForgeCoreTests PRIVATE ForgeCore)
    if(MSVC)
        target_compile_options(ForgeCoreTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ForgeCoreTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ForgeCoreTests COMMAND ForgeCoreTests)
    add_executable(ForgeFullSystemsTests tests/test_full_systems.cpp)
    target_include_directories(ForgeFullSystemsTests PRIVATE src)
    target_link_libraries(ForgeFullSystemsTests PRIVATE ForgeCore)
    if(MSVC)
        target_compile_options(ForgeFullSystemsTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ForgeFullSystemsTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ForgeFullSystemsTests COMMAND ForgeFullSystemsTests)

    add_executable(ForgeProjectTests tests/test_project.cpp)
    target_include_directories(ForgeProjectTests PRIVATE src)
    target_link_libraries(ForgeProjectTests PRIVATE ForgeCore)
    if(MSVC)
        target_compile_options(ForgeProjectTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ForgeProjectTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ForgeProjectTests COMMAND ForgeProjectTests)

    add_executable(ForgeProfessionalQA tests/test_professional_qa.cpp ${FORGE_CORE_SOURCES})
    target_include_directories(ForgeProfessionalQA PRIVATE src)
    if(MSVC)
        target_compile_options(ForgeProfessionalQA PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ForgeProfessionalQA PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ForgeProfessionalQA COMMAND ForgeProfessionalQA)
endif()

install(TARGETS ForgeEngine ForgeRuntime RUNTIME DESTINATION bin)
