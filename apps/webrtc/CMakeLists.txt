set(APP webrtc)

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SOURCE_FILES
	${PROJECT_SOURCE_DIR}/src/mongoose.cpp
	${SRCS}/main.cpp
)

set(HEADER_FILES
	${INCS}/webrtc.h
)

set(WWW_FILES
	${PROJECT_SOURCE_DIR}/www/mgws/index.html
	${PROJECT_SOURCE_DIR}/www/mgws/index.js
)

if(MSVC)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${WWW_FILES})
endif()

add_executable(${APP} ${HEADER_FILES} ${SOURCE_FILES} ${WWW_FILES})

target_include_directories(${APP} PUBLIC ${PROJECT_SOURCE_DIR}/inc ${CMAKE_CURRENT_SOURCE_DIR}/inc)
#target_link_libraries(${APP} PRIVATE mgws)

if(MSVC)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/certs)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "localhost.crt localhost.key 0.0.0.0 4447")
endif()
