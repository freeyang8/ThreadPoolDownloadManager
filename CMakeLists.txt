
cmake_minimum_required(VERSION 3.19)

# 使用 vcpkg 工具链（如果存在）
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "E:/software/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "E:/software/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

project(ThreadPoolDownloadManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# 解决 MSVC 编码问题
if(MSVC)
    add_compile_options("/utf-8")
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(CURL REQUIRED)

qt_standard_project_setup()

qt_add_executable(ThreadPoolDownloadManager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    DownloadManager.h

    DownloadTask.h

    DownloadManager.cpp

    DownloadTask.cpp
    DownloadStatus.h

)

target_link_libraries(ThreadPoolDownloadManager
    PRIVATE
        Qt::Core
        Qt::Widgets
        CURL::libcurl
)

install(TARGETS ThreadPoolDownloadManager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ThreadPoolDownloadManager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
