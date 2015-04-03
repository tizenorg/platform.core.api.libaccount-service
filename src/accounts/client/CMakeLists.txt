
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
#SET(LIBDIR "\${prefix}/lib")
#SET(INCLUDEDIR "\${prefix}/include ")
SET(VERSION_MAJOR 0)
SET(VERSION "${VERSION_MAJOR}.3.0")

SET(SRCS
	account.c
	../common/account_ipc_marshal.c
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/accounts/common)

INCLUDE(FindPkgConfig)
pkg_check_modules(clientpkgs REQUIRED
		dbus-1
		dlog
		gobject-2.0
		glib-2.0
		gio-2.0
		gio-unix-2.0
		capi-base-common
		vconf
)

FOREACH(flag ${clientpkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF ("${ARCH}" STREQUAL "arm")
        ADD_DEFINITIONS("-DTARGET_ARM")
        MESSAGE("[TARGET] Compile")
ELSE  ("${ARCH}" STREQUAL "arm")
        ADD_DEFINITIONS("-DTARGET_I386")
        MESSAGE("[i386] Compile")
ENDIF ("${ARCH}" STREQUAL "arm")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall -Werror")
SET(CMAKE_LDFLAGS "-Wl,-zdefs")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DFACTORYFS=\"$ENV{FACTORYFS}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY
	OUTPUT account-mgr-stub.c
	COMMAND gdbus-codegen --interface-prefix org.tizen.
	--generate-c-code account-mgr-stub ../account_mgr.xml
	COMMENT "Generating Account Client GDBus .c/.h")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS} account-mgr-stub.c)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${VERSION})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${clientpkgs_LDFLAGS})

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

#INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib/accounts-svc)
#INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)
#INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR}/accounts-svc)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${LIBDIR}/pkgconfig)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/account.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/account-error.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/account-types.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/account_internal.h DESTINATION include)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/account_product.h DESTINATION include)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/src/accounts/client/account-mgr-stub.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/src/accounts/common/account_ipc_marshal.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/src/accounts/common/account-private.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/src/accounts/common/dbg.h DESTINATION include)
