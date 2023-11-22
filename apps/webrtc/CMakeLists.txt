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
	${PROJECT_SOURCE_DIR}/www/mgws/flex.css
	${PROJECT_SOURCE_DIR}/www/mgws/flex_mobile.css
)

if(MSVC)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${WWW_FILES})
	set(DEBUG_ARGS
		"${PROJECT_SOURCE_DIR}/www ${PROJECT_SOURCE_DIR}/certs/localhost.crt ${PROJECT_SOURCE_DIR}/certs/localhost.key"
	)
endif()

add_executable(${APP} ${HEADER_FILES} ${SOURCE_FILES} ${WWW_FILES})

target_compile_definitions(${APP} PUBLIC MG_TLS=MG_TLS_OPENSSL)
target_include_directories(${APP} PUBLIC ${PROJECT_SOURCE_DIR}/inc ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(${APP} PUBLIC OpenSSL::SSL)

if(MSVC)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/certs)
	set_target_properties(${APP} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS ${DEBUG_ARGS})
endif()
