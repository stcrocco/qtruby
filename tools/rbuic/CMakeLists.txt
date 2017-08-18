
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ruby )

find_package(Qt5 COMPONENTS Xml REQUIRED)

########### next target ###############

set(rbuic5_bin_SRCS 
   customwidgetsinfo.cpp 
   databaseinfo.cpp 
   driver.cpp 
   treewalker.cpp 
   ui5.cpp 
   uic.cpp 
   validator.cpp 
   ruby/rbextractimages.cpp 
   ruby/rbwritedeclaration.cpp 
   ruby/rbwriteicondata.cpp 
   ruby/rbwriteicondeclaration.cpp 
   ruby/rbwriteiconinitialization.cpp 
   ruby/rbwriteinitialization.cpp 
   main.cpp )

add_executable(rbuic5_bin ${rbuic5_bin_SRCS})
set_target_properties(rbuic5_bin PROPERTIES OUTPUT_NAME rbuic5)
set_target_properties(rbuic5_bin PROPERTIES COMPILE_FLAGS -DQT_UIC_RB_GENERATOR)

target_link_libraries(rbuic5_bin Qt5::Core Qt5::Xml )

install(TARGETS rbuic5_bin DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


########### next target ###############


########### install files ###############
