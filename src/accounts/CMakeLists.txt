ADD_SUBDIRECTORY(client)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/accounts-svc.pc.in ${CMAKE_SOURCE_DIR}/accounts-svc.pc @ONLY)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/accounts-svc.pc DESTINATION ${LIBDIR}/pkgconfig)
