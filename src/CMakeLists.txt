
add_subdirectory( lib )

IF(QT_QTDBUS_FOUND)
    ADD_DEFINITIONS (-DQT_QTDBUS)
ENDIF(QT_QTDBUS_FOUND)

IF(QWT_FOUND)
    ADD_DEFINITIONS (-DQT_QWT)
ENDIF(QWT_FOUND)

if(RUBY_VERSION)
    ADD_DEFINITIONS (-DRUBY_VERSION=0x${RUBY_VERSION_NUMBER})
ENDIF(RUBY_VERSION)

include_directories( ${SMOKE_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES (${QT_INCLUDES})


set(qtrubyshared_LIB_SRCS 
   Qt.cpp 
   handlers.cpp 
   marshall_types.cpp
   )

set(qtruby_LIB_SRCS qtruby.cpp)

# Use 'MODULE' instead of 'SHARED' so that qtruby will be built as a bundle on Mac OS X
# instead of a dyld. But is this correct (rjd)? It needs testing.
add_library(qtruby4shared SHARED ${qtrubyshared_LIB_SRCS})
add_library(qtruby4 MODULE ${qtruby_LIB_SRCS})

target_link_libraries(  qtruby4shared 
                            ${QT_QTCORE_LIBRARY} 
                            ${QT_QTGUI_LIBRARY} 
                            ${RUBY_LIBRARY} 
                            ${QT_QTNETWORK_LIBRARY}
                            ${SMOKE_BASE_LIBRARY}
                            ${SMOKE_QTCORE_LIBRARY} 
                            ${SMOKE_QTGUI_LIBRARY} 
                            ${SMOKE_QTXML_LIBRARY} 
                            ${SMOKE_QTOPENGL_LIBRARY} 
                            ${SMOKE_QTSQL_LIBRARY} 
                            ${SMOKE_QTNETWORK_LIBRARY} 
                            ${SMOKE_QTDBUS_LIBRARY} 
                            ${SMOKE_QTSVG_LIBRARY} )

target_link_libraries(  qtruby4 
                            ${QT_QTCORE_LIBRARY} 
                            ${QT_QTGUI_LIBRARY} 
                            ${QT_QTDBUS_LIBRARY} 
                            ${RUBY_LIBRARY} 
                            ${QT_QTNETWORK_LIBRARY} 
                            ${SMOKE_QTCORE_LIBRARY} 
                            ${SMOKE_QTGUI_LIBRARY} 
                            ${SMOKE_QTXML_LIBRARY} 
                            ${SMOKE_QTOPENGL_LIBRARY} 
                            ${SMOKE_QTSQL_LIBRARY} 
                            ${SMOKE_QTNETWORK_LIBRARY} 
                            ${SMOKE_QTDBUS_LIBRARY} 
                            ${SMOKE_QTSVG_LIBRARY}
                            qtruby4shared )

# target_link_libraries(qtrubyinternal ${QT_QTCORE_LIBRARY} ${RUBY_LIBRARY} ${QT_QTNETWORK_LIBRARY})

set_target_properties(qtruby4 PROPERTIES PREFIX "")
set_target_properties(qtruby4shared PROPERTIES VERSION 2.0.0 SOVERSION 2)

install(TARGETS qtruby4shared EXPORT QtRuby4Export DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} )
install(TARGETS qtruby4 DESTINATION ${CUSTOM_RUBY_SITE_ARCH_DIR})
install(EXPORT QtRuby4Export DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtruby4/cmake NAMESPACE QTRUBY4_ )

install(FILES qtruby.h marshall.h marshall_basetypes.h marshall_complex.h marshall_macros.h marshall_primitives.h marshall_types.h smokeruby.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/qtruby)

if (Q_WS_MAC)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink \"${CUSTOM_RUBY_SITE_ARCH_DIR}/qtruby4.so\"  \"${CUSTOM_RUBY_SITE_ARCH_DIR}/qtruby4.bundle\" )" )
endif (Q_WS_MAC)
