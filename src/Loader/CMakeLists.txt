
set(TARGET_NAME loader)

set(CURRENT_DIR .)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

file(GLOB SRC_FILES ${CURRENT_DIR}/*.cpp)
file(GLOB HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB SRC_FILES_OS ${CURRENT_DIR}/${OS_SPECIFIC_DIR_NAME}/*.cpp)

set(ALL_SRC_HDR_FILES
  ${SRC_FILES}
  ${HEADER_FILES}
  ${SRC_FILES_OS}
)

set(THIRD_PARTY_FILES
  
)

source_group(TREE ${PROJECT_SOURCE_DIR} PREFIX "Source" FILES ${ALL_SRC_HDR_FILES} ${THIRD_PARTY_FILES})

SET_COMMON_COMPILER_FLAGS()
SET_COMMON_SHARED_LINKER_FLAGS()

add_library(${TARGET_NAME} SHARED ${ALL_SRC_HDR_FILES} ${THIRD_PARTY_FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_definitions(${TARGET_NAME}
  PRIVATE ${EUSTIA_COMPILER_DEFINES}
  _USRDLL
)

target_link_libraries(${TARGET_NAME}
  PRIVATE core
  PRIVATE misc
)

if(OS_WINDOWS)
  target_link_libraries(${TARGET_NAME} PRIVATE ntdll)
endif()