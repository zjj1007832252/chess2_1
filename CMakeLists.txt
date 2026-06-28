cmake_minimum_required(VERSION 3.16)

project(Xiangqi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动查找 Qt（优先使用 PATH 中的 qmake，也可通过 -DCMAKE_PREFIX_PATH 指定）
find_program(_qmake NAMES qmake qmake6 qmake-qt6)
if(_qmake)
    execute_process(COMMAND "${_qmake}" -query QT_INSTALL_PREFIX
                    OUTPUT_VARIABLE _qt_prefix OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_qt_prefix AND EXISTS "${_qt_prefix}")
        list(PREPEND CMAKE_PREFIX_PATH "${_qt_prefix}")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- 源文件 ----------------------------------------------------------------
set(XIANGQI_SOURCES
    main.cpp
    chessboard.cpp
    chesspiece.cpp
    chessgame.cpp
    ai.cpp
    mainwindow.cpp
    boardwidget.cpp
)

set(XIANGQI_HEADERS
    chessboard.h
    chesspiece.h
    chessgame.h
    ai.h
    mainwindow.h
    boardwidget.h
)

set(XIANGQI_RESOURCES
    resources.qrc
)

# --- 目标 ------------------------------------------------------------------
add_executable(xiangqi WIN32
    ${XIANGQI_SOURCES}
    ${XIANGQI_HEADERS}
    ${XIANGQI_RESOURCES}
)

target_link_libraries(xiangqi PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(xiangqi PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Debug 构建时保留控制台窗口，方便查看 qDebug 输出；
# Release 构建为纯 GUI 程序（无控制台）。
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(xiangqi PROPERTIES
        WIN32_EXECUTABLE FALSE
        LINK_FLAGS "-mconsole"
    )
endif()

# --- 运行时自动部署 --------------------------------------------------------
# 构建完成后调用 windeployqt，将所需的 Qt DLL / 插件复制到可执行文件旁，
# 无需在 CMake 里硬编码复制整个 Qt bin 目录。
if(WIN32)
    set(_windeployqt "$<TARGET_FILE:Qt6::windeployqt>")
    add_custom_command(TARGET xiangqi POST_BUILD
        COMMAND "${_windeployqt}"
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:xiangqi>"
        COMMENT "Running windeployqt to deploy Qt runtime"
    )

    # 用编译器自带的 C++ 运行时覆盖 windeployqt 部署的旧版本，保证 ABI 一致
    get_filename_component(_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    foreach(_runtime_dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        add_custom_command(TARGET xiangqi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_mingw_bin_dir}/${_runtime_dll}"
                "$<TARGET_FILE_DIR:xiangqi>/${_runtime_dll}"
            COMMENT "Aligning ${_runtime_dll} with compiler ABI"
        )
    endforeach()
endif()
