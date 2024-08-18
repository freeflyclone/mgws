set(APP webrtc)

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SOURCE_FILES
	${SRCS}/peer.cpp
	${SRCS}/main.cpp
)

set(HEADER_FILES
	${INCS}/peer.h
)

set(WWW_FILES
	${PROJECT_SOURCE_DIR}/www/webrtc/css/dynamic.css
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/sounds/local-ring.m4a
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/sounds/remote-ring.m4a
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/sounds/telephone-ring.m4a
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/audio.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/index.html
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/index.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/local.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/theme.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/ui.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/websock.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/peer.js
	${PROJECT_SOURCE_DIR}/www/webrtc/mgws/pulse.js
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
