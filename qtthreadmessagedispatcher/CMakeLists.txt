set(CMAKE_CXX_STANDARD 17)

include(add_msvc_precompiled_header)
include(configure_msvc_runtime)

set(PROJECT_NAME "qtthreadmessagedispatcher")

project(${PROJECT_NAME})

aux_source_directory("./" QTTHREADMESSAGEDISPATCHER_SOURCE_LIST)
file(GLOB_RECURSE QTTHREADMESSAGEDISPATCHER_HEADER_LIST "*.h")

qt5_wrap_cpp(QTTHREADMESSAGEDISPATCHER_MOC_HEADER_LIST ${QTTHREADMESSAGEDISPATCHER_HEADER_LIST})
list(APPEND QTTHREADMESSAGEDISPATCHER_SOURCE_LIST ${QTTHREADMESSAGEDISPATCHER_MOC_HEADER_LIST})

list(REMOVE_ITEM QTTHREADMESSAGEDISPATCHER_SOURCE_LIST stdafx.cpp)
add_msvc_precompiled_header("stdafx.h" "stdafx.cpp" QTTHREADMESSAGEDISPATCHER_SOURCE_LIST)

configure_msvc_runtime()

source_group("Precompiled Headers" FILES stdafx.h stdafx.cpp)
source_group("MOC Files" FILES ${QTTHREADMESSAGEDISPATCHER_MOC_HEADER_LIST})

add_definitions(${QT_DEFINITIONS})

add_library(
    ${PROJECT_NAME} STATIC
    ${QTTHREADMESSAGEDISPATCHER_SOURCE_LIST}
    ${QTTHREADMESSAGEDISPATCHER_HEADER_LIST}
)

if(MSVC)
    add_definitions(
        /EHsc
        /MP
        /permissive-
        /std:c++latest
        /Zi
        /W4
        /WX
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
        LINK_FLAGS /DEBUG
    )

    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES WIN32_EXECUTABLE YES
        LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
    )
endif()

message("
    ${PROJECT_NAME}: 
    Sources: ${QTTHREADMESSAGEDISPATCHER_SOURCE_LIST}
    Headers: ${QTTHREADMESSAGEDISPATCHER_HEADER_LIST}
")

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Core
)

if(NOT WIN32)
    include(cotire)
    set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
    cotire(${PROJECT_NAME})
endif()
