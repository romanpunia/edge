# Create sources list with main sources
get_filename_component(BASE_PATH "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
file(GLOB_RECURSE SOURCE
    ${BASE_PATH}/src/vitex/*.inl
    ${BASE_PATH}/src/vitex/*.h
    ${BASE_PATH}/src/vitex/*.c
    ${BASE_PATH}/src/vitex/*.cc
    ${BASE_PATH}/src/vitex/*.hpp
    ${BASE_PATH}/src/vitex/*.cpp
    ${BASE_PATH}/src/vitex/*.hxx
    ${BASE_PATH}/src/vitex/*.cxx)

# Append source files of dependencies
set(VI_ANGELSCRIPT ON CACHE BOOL "Enable angelscript built-in library")
set(VI_WEPOLL ON CACHE BOOL "Enable efficient epoll implementation for Windows")
set(VI_PUGIXML ON CACHE BOOL "Enable pugixml built-in library")
set(VI_RAPIDJSON ON CACHE BOOL "Enable rapidjson built-in library")
if (VI_BACKWARDCPP OR TRUE)
    set(BACKWARD_PATHS "")
    set(BACKWARD_USES "")
    set(BACKWARD_LINKS "")
    set(BACKWARD_ACTIVE ON)
    find_path(BACKWARD_HAS_UNWIND unwind.h)
    find_path(BACKWARD_HAS_LIBUNWIND libunwind.h)
    find_path(BACKWARD_HAS_BACKTRACE execinfo.h)
    find_path(BACKWARD_HAS_LIBELF libelf.h PATH_SUFFIXES "elfutils")
    find_path(BACKWARD_HAS_DW libdw.h PATH_SUFFIXES "elfutils")
    find_path(BACKWARD_HAS_DWARF dwarf.h PATH_SUFFIXES "libdwarf-0")
    find_path(BACKWARD_HAS_BFD bfd.h)
    if (WIN32)
    elseif (BACKWARD_HAS_LIBUNWIND)
        set(BACKWARD_PATHS "${BACKWARD_HAS_LIBUNWIND}")
        set(BACKWARD_USES "-DBACKWARD_HAS_LIBUNWIND")
        set(BACKWARD_LINKS "-lunwind")
    elseif (BACKWARD_HAS_UNWIND)
        set(BACKWARD_USES "-DBACKWARD_HAS_UNWIND")
    elseif (BACKWARD_HAS_BACKTRACE)
        set(BACKWARD_USES "-DBACKWARD_HAS_BACKTRACE -DBACKWARD_HAS_BACKTRACE_SYMBOL")
    else()
        set(BACKWARD_ACTIVE OFF)
    endif()
    if (WIN32)
    elseif (BACKWARD_HAS_DW AND BACKWARD_HAS_LIBELF)
        set(BACKWARD_PATHS "${BACKWARD_PATHS};${BACKWARD_HAS_DW}")
        set(BACKWARD_USES "${BACKWARD_USES} -DBACKWARD_HAS_DW")
        set(BACKWARD_LINKS "${BACKWARD_LINKS} -lelf -ldw")
    elseif (BACKWARD_HAS_DWARF AND BACKWARD_HAS_LIBELF)
        set(BACKWARD_PATHS "${BACKWARD_PATHS};${BACKWARD_HAS_DWARF}")
        set(BACKWARD_USES "${BACKWARD_USES} -DBACKWARD_HAS_DWARF")
        set(BACKWARD_LINKS "${BACKWARD_LINKS} -lelf -ldwarf")
    elseif (BACKWARD_HAS_BFD)
        set(BACKWARD_PATHS "${BACKWARD_PATHS};${BACKWARD_HAS_BFD}")
        set(BACKWARD_USES "${BACKWARD_USES} -DBACKWARD_HAS_BFD")
        set(BACKWARD_LINKS "${BACKWARD_LINKS} -lbfd")
    elseif (BACKWARD_HAS_BACKTRACE)
    else()
        set(BACKWARD_ACTIVE OFF)
    endif()
    if (BACKWARD_ACTIVE)
        set(VI_BACKWARDCPP ON CACHE BOOL "Enable backward-cpp built-in library")
        if (VI_BACKWARDCPP)
            list(APPEND SOURCE ${CMAKE_CURRENT_LIST_DIR}/backward-cpp/backward.hpp)
            message(STATUS "Use library @backward-cpp - OK")
        endif()
    else()
        set(VI_BACKWARDCPP OFF CACHE BOOL "backward-cpp is unavailable")
        message("Use library @backward-cpp - bad configuration")
    endif()
    if (NOT VI_BACKWARDCPP)
        unset(BACKWARD_PATHS CACHE)
        unset(BACKWARD_USES CACHE)
        unset(BACKWARD_LINKS CACHE)
    endif()
    unset(BACKWARD_ACTIVE CACHE)
    unset(BACKWARD_HAS_UNWIND CACHE)
    unset(BACKWARD_HAS_LIBUNWIND CACHE)
    unset(BACKWARD_HAS_BACKTRACE CACHE)
    unset(BACKWARD_HAS_LIBELF CACHE)
    unset(BACKWARD_HAS_DW CACHE)
    unset(BACKWARD_HAS_DWARF CACHE)
    unset(BACKWARD_HAS_BFD CACHE)
endif()
if (VI_PUGIXML)
    file(GLOB_RECURSE SOURCE_PUGIXML ${CMAKE_CURRENT_LIST_DIR}/pugixml/src/*.*)
    list(APPEND SOURCE ${SOURCE_PUGIXML})
endif()
if (VI_RAPIDJSON)
    file(GLOB_RECURSE SOURCE_RAPIDJSON
        ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/*.h*)
    list(APPEND SOURCE ${SOURCE_RAPIDJSON})
endif()
if (VI_ANGELSCRIPT)
    file(GLOB_RECURSE SOURCE_ANGELSCRIPT
        ${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/include/angelscript.h
        ${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/*.h
        ${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/*.cpp)
    if (MSVC)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm64")
                list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm64_msvc.asm")
            else()
                list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_x64_msvc_asm.asm")
            endif()
        elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm_msvc.asm")
        endif()
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
        list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm_gcc.S")
        list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm_vita.S")
        list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm_xcode.S")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^aarch64")
        if (APPLE)
            list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm64_xcode.S")
        else()
            list(APPEND SOURCE_ANGELSCRIPT "${CMAKE_CURRENT_LIST_DIR}/angelscript/sdk/angelscript/source/as_callfunc_arm64_gcc.S")
        endif()
    endif()
    list(APPEND SOURCE ${SOURCE_ANGELSCRIPT})
endif()
if (VI_WEPOLL AND WIN32)
	list(APPEND SOURCE
        ${CMAKE_CURRENT_LIST_DIR}/wepoll/wepoll.h
        ${CMAKE_CURRENT_LIST_DIR}/wepoll/wepoll.c)
endif()
if (VI_FCONTEXT OR TRUE)
    set(FCONTEXT_ARCHS arm arm64 loongarch64 mips32 mips64 ppc32 ppc64 riscv64 s390x i386 x86_64 combined)
    if (WIN32)
        set(FCONTEXT_BIN pe)
    elseif (APPLE)
        set(FCONTEXT_BIN mach-o)
    else()
        set(FCONTEXT_BIN elf)
    endif()
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(FCONTEXT_ABI aapcs)
    elseif (WIN32)
        set(FCONTEXT_ABI ms)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(FCONTEXT_ABI o32)
        else()
            set(FCONTEXT_ABI n64)
        endif()
    else()
        set(FCONTEXT_ABI sysv)
    endif()
    if (CMAKE_SYSTEM_PROCESSOR IN_LIST FCONTEXT_ARCHS)
        set(FCONTEXT_ARCH ${CMAKE_SYSTEM_PROCESSOR})
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        if (CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]")
            set(FCONTEXT_ARCH arm)
        elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
            set(FCONTEXT_ARCH mips32)
        else()
            set(FCONTEXT_ARCH i386)
        endif()
    else()
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]")
            set(FCONTEXT_ARCH arm64)
        elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
            set(FCONTEXT_ARCH mips64)
        else()
            set(FCONTEXT_ARCH x86_64)
        endif()
    endif()
    if (MSVC)
        if(FCONTEXT_ARCH STREQUAL arm64 OR FCONTEXT_ARCH STREQUAL arm)
            set(FCONTEXT_ASM armasm)
        else()
            set(FCONTEXT_ASM masm)
        endif()
    else()
        set(FCONTEXT_ASM gas)
    endif()
    if (FCONTEXT_BIN STREQUAL pe)
        set(FCONTEXT_EXT .asm)
    elseif (FCONTEXT_ASM STREQUAL gas)
        set(FCONTEXT_EXT .S)
    else()
        set(FCONTEXT_EXT .asm)
    endif()
    if (FCONTEXT_BIN STREQUAL mach-o)
        set(FCONTEXT_BIN macho)
    endif()
    if (EXISTS ${BASE_PATH}/src/vitex/internal/make_${FCONTEXT_ARCH}_${FCONTEXT_ABI}_${FCONTEXT_BIN}_${FCONTEXT_ASM}${FCONTEXT_EXT})
        set(VI_FCONTEXT ON CACHE BOOL "Enable fcontext built-in library (otherwise coroutines based on ucontext, WinAPI or C++20)")
        if (VI_FCONTEXT)
            set(FCONTEXT_SOURCES
                ${BASE_PATH}/src/vitex/internal/fcontext.h
                ${BASE_PATH}/src/vitex/internal/make_${FCONTEXT_ARCH}_${FCONTEXT_ABI}_${FCONTEXT_BIN}_${FCONTEXT_ASM}${FCONTEXT_EXT}
                ${BASE_PATH}/src/vitex/internal/jump_${FCONTEXT_ARCH}_${FCONTEXT_ABI}_${FCONTEXT_BIN}_${FCONTEXT_ASM}${FCONTEXT_EXT}
                ${BASE_PATH}/src/vitex/internal/ontop_${FCONTEXT_ARCH}_${FCONTEXT_ABI}_${FCONTEXT_BIN}_${FCONTEXT_ASM}${FCONTEXT_EXT})
            list(APPEND SOURCE ${FCONTEXT_SOURCES})
            message(STATUS "Use fcontext @${FCONTEXT_ARCH}_${FCONTEXT_ABI}_${FCONTEXT_BIN}_${FCONTEXT_ASM} - OK")
        else()
            message(STATUS "Use fcontext @ucontext/@winapi - OK")
        endif()
    else()
        set(VI_FCONTEXT OFF CACHE BOOL "FContext is unavailable")
        message(STATUS "Use fcontext @ucontext/@winapi - OK")
    endif()
    unset(FCONTEXT_BIN)
    unset(FCONTEXT_ABI)
    unset(FCONTEXT_ARCHS)
    unset(FCONTEXT_ARCH)
    unset(FCONTEXT_ASM)
    unset(FCONTEXT_EXT)
    unset(FCONTEXT_SUFFIX)
endif()
if (VI_CONCURRENTQUEUE OR TRUE)
    list(APPEND SOURCE
        ${CMAKE_CURRENT_LIST_DIR}/concurrentqueue/concurrentqueue.h
        ${CMAKE_CURRENT_LIST_DIR}/concurrentqueue/lightweightsemaphore.h
        ${CMAKE_CURRENT_LIST_DIR}/concurrentqueue/blockingconcurrentqueue.h)
endif()

# Group all sources for nice IDE preview
foreach(ITEM IN ITEMS ${SOURCE})
    get_filename_component(ITEM_PATH "${ITEM}" PATH)
    string(REPLACE "${BASE_PATH}" "" ITEM_GROUP "${ITEM_PATH}")
    string(REPLACE "/" "\\" ITEM_GROUP "${ITEM_GROUP}")
    source_group("${ITEM_GROUP}" FILES "${ITEM}")
endforeach()