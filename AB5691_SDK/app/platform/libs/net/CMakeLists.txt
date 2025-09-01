cmake_minimum_required(VERSION 3.15)
project(net C)

SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)

set(NET_VERSION_MAJOR    "1")
set(NET_VERSION_MINOR    "0")
set(NET_VERSION_REVISION "0")
set(NET_VERSION_STRING
    "${NET_VERSION_MAJOR}.${NET_VERSION_MINOR}.${NET_VERSION_REVISION}"
)

set(NET_COMPILER_FLAGS
    -Os
    -Wall
    -march=rv32imacxbs1
    -Wno-unused-function
    -Wno-unused-const-variable
    -Wno-unused-variable
    -msave-restore
    -mjump-tables-in-text
)

set(NET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NET_INCLUDE_DIR ${NET_SOURCE_DIR}/include)

set(LWIP_INCLUDE_DIRS
    "${NET_INCLUDE_DIR}"
    "${NET_INCLUDE_DIR}/common"
    "${NET_INCLUDE_DIR}/os"
    "${NET_INCLUDE_DIR}/lwip"
    "${NET_INCLUDE_DIR}/lwip/ports"
)

# ========== 1. 交叉编译工具链 ==========
# 命令行:  cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm-linux.cmake ..
# 也可把下面两行直接写死，但推荐用命令行方式：
# set(CMAKE_TOOLCHAIN_FILE "D:/users/hao_zhu/svn/1_pan/glass/app/libs/net/riscv.cmake")

# ========== 2. 模块开关 ==========
option(WITH_CJSON     "Build cjson module"     ON)
option(WITH_NOPOLL    "Build nopoll module"    ON)
option(WITH_WEBCLIENT "Build webclient module" ON)
# 后续再加模块时，只要再加一个 option 即可

# ========== 3. 收集 OBJECT 库 ==========
set(NETLIB_OBJS)          # 保存所有要合并的 OBJECT 库

add_subdirectory(modules/common)
list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:common>)

add_subdirectory(modules/lwip)
list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:lwipcontribports>)

add_subdirectory(modules/tinycrypt)
list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:tinycrypt>)

if(WITH_CJSON)
    add_subdirectory(modules/cjson)
    list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:cjson>)
endif()

if(WITH_NOPOLL)
    add_subdirectory(modules/nopoll)
    list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:nopoll>)
endif()

if(WITH_WEBCLIENT)
    add_subdirectory(modules/webclient)
    list(APPEND NETLIB_OBJS $<TARGET_OBJECTS:webclient>)
endif()

# ========== 4. 生成最终静态库 ==========
add_library(net)
target_link_libraries(net PUBLIC lwipcontribports lwipcore lwipallapps tinycrypt)

if(WITH_CJSON)
    target_link_libraries(net PUBLIC cjson)
endif()

if(WITH_NOPOLL)
    target_link_libraries(net PUBLIC nopoll)
endif()

if(WITH_WEBCLIENT)
    target_link_libraries(net PUBLIC webclient)
endif()

add_custom_command(TARGET net
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
            --rename-section .bss=.net_bss
            --rename-section .sbss=.net_sbss
            $<TARGET_FILE:net>
    COMMENT "Renaming .bss to .net_bss in-place"
    VERBATIM
)
