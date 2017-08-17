
add_subdirectory( lib )
find_package(Qt5 COMPONENTS Network DBus Widgets PrintSupport REQUIRED)

IF(QT_QTDBUS_FOUND)
    ADD_DEFINITIONS (-DQT_QTDBUS)
ENDIF(QT_QTDBUS_FOUND)

IF(QWT_FOUND)
    ADD_DEFINITIONS (-DQT_QWT)
ENDIF(QWT_FOUND)

if(RUBY_VERSION)
    ADD_DEFINITIONS (-DRUBY_VERSION=0x${RUBY_VERSION_NUMBER})
ENDIF(RUBY_VERSION)


list(GET Qt5Core_INCLUDE_DIRS 0 QT5_CORE_INCLUDE_BASE)
set(QMetaObjectBuilder_INCLUDE "${QT5_CORE_INCLUDE_BASE}/QtCore/${Qt5Core_VERSION}/QtCore")

include_directories( ${SMOKE_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} ${QMetaObjectBuilder_INCLUDE} )
INCLUDE_DIRECTORIES (${QT_INCLUDES})

set(qtrubyshared_LIB_SRCS 
   Qt.cpp 
   handlers.cpp 
   marshall_types.cpp
   )

set(qtruby_LIB_SRCS qtruby.cpp)

# Use 'MODULE' instead of 'SHARED' so that qtruby will be built as a bundle on Mac OS X
# instead of a dyld. But is this correct (rjd)? It needs testing.
add_library(qtruby5shared SHARED ${qtrubyshared_LIB_SRCS})
add_library(qtruby5 MODULE ${qtruby_LIB_SRCS})

target_link_libraries(  qtruby5shared 
                            Qt5::Core
                            Qt5::Gui
                            Qt5::Network
                            Qt5::Widgets
                            Qt5::PrintSupport
                            ${RUBY_LIBRARY} 
                            ${SMOKE_BASE_LIBRARY}
                            ${SMOKE_QTCORE_LIBRARY} 
                            ${SMOKE_QTGUI_LIBRARY} 
                            ${SMOKE_QTXML_LIBRARY} 
                            ${SMOKE_QTOPENGL_LIBRARY} 
                            ${SMOKE_QTSQL_LIBRARY} 
                            ${SMOKE_QTNETWORK_LIBRARY} 
                            ${SMOKE_QTDBUS_LIBRARY} 
                            ${SMOKE_QTSVG_LIBRARY} )

target_link_libraries(  qtruby5 
                            Qt5::Core
                            Qt5::Gui
                            Qt5::Network
                            Qt5::DBus
                            Qt5::Widgets
                            ${RUBY_LIBRARY} 
                            ${SMOKE_QTCORE_LIBRARY} 
                            ${SMOKE_QTGUI_LIBRARY} 
                            ${SMOKE_QTXML_LIBRARY} 
                            ${SMOKE_QTOPENGL_LIBRARY} 
                            ${SMOKE_QTSQL_LIBRARY} 
                            ${SMOKE_QTNETWORK_LIBRARY} 
                            ${SMOKE_QTDBUS_LIBRARY} 
                            ${SMOKE_QTSVG_LIBRARY}
                            qtruby5shared )

set_target_properties(qtruby5 PROPERTIES PREFIX "")
set_target_properties(qtruby5shared PROPERTIES VERSION 3.0.0 SOVERSION 3)

install(TARGETS qtruby5shared EXPORT QtRuby5Export DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} )
install(TARGETS qtruby5 DESTINATION ${CUSTOM_RUBY_SITE_ARCH_DIR})
install(EXPORT QtRuby5Export DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtruby5/cmake NAMESPACE QTRUBY5_ )

install(FILES qtruby.h marshall.h marshall_basetypes.h marshall_complex.h marshall_macros.h marshall_primitives.h marshall_types.h smokeruby.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/qtruby)

if (Q_WS_MAC)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink \"${CUSTOM_RUBY_SITE_ARCH_DIR}/qtruby5.so\"  \"${CUSTOM_RUBY_SITE_ARCH_DIR}/qtruby5.bundle\" )" )
endif (Q_WS_MAC)
