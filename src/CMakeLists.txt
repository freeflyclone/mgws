set(LIB_NAME "mgws")

set(SRCS ${PROJECT_SOURCE_DIR}/src)
set(INCS ${PROJECT_SOURCE_DIR}/inc)

set(SOURCE_FILES
	${SRCS}/mgws.cpp
	${SRCS}/mongoose.c
	${SRCS}/sqlite3.c
	${SRCS}/session.cpp
	${SRCS}/sessionmgr.cpp
)

set(HEADER_FILES
	${INCS}/mgws.h
	${INCS}/mongoose.h
	${INCS}/sqlite3.h
	${INCS}/sqlite3ext.h
	${INCS}/session.h
	${INCS}/sessionmgr.h
)

add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(${LIB_NAME} PUBLIC MG_TLS=MG_TLS_OPENSSL)
target_include_directories(${LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/inc)
target_link_libraries(${LIB_NAME} PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
target_link_libraries(${LIB_NAME} PRIVATE OpenSSL::SSL)
