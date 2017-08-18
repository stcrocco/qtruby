find_package(Qt5 COMPONENTS Xml REQUIRED)

SET(rbrcc_SRCS rcc.cpp main.cpp )

ADD_EXECUTABLE(rbrcc ${rbrcc_SRCS})
TARGET_LINK_LIBRARIES (rbrcc Qt5::Core Qt5::Xml)

INSTALL(TARGETS rbrcc DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
