set(APP dashboard)

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SOURCE_FILES
	${SRCS}/main.cpp
)

set(HEADER_FILES
)

set(WWW_FILES
	${PROJECT_SOURCE_DIR}/www/dashboard/index.html
)

if(MSVC)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${WWW_FILES})
	set(DEBUG_ARGS
		"${PROJECT_SOURCE_DIR}/www ${PROJECT_SOURCE_DIR}/certs/localhost.crt ${PROJECT_SOURCE_DIR}/certs/localhost.key"
	)
endif()

add_executable(${APP} ${HEADER_FILES} ${SOURCE_FILES} ${WWW_FILES})

target_include_directories(${APP} PUBLIC ${PROJECT_SOURCE_DIR}/inc ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(${APP} PRIVATE mgws)

if(MSVC)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/certs)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS ${DEBUG_ARGS})
endif()
