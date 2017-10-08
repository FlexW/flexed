find_package(
  Boost 1.62.0 REQUIRED system filesystem
  )

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTKSOURCEVIEWMM gtksourceviewmm-3.0)

link_directories(
  ${GTKMM_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  )

include_directories(
  ${GTKMM_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  )

SET(GCC_COVERAGE_LINK_FLAGS "-fPIC")
SET(
  CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}"
  )

add_library(
  libflexed SHARED
  flexed_editor.cpp
  flexed_editor.h
  flexed_text_buffer.cpp
  flexed_text_buffer.h
  flexed_paned.cpp
  flexed_paned.h
  flexed_global_text_buffer_container.cpp
  flexed_global_text_buffer_container.h
  flexed_text_buffer_container.cpp
  flexed_text_buffer_container.h
  flexed_mode_loader.cpp
  flexed_mode_loader.h
  orderd_container.h
  keyboard_handler.cpp
  keyboard_handler.h
  keyboard_map.cpp
  keyboard_map.h
  )

target_link_libraries(
  libflexed
  ${GTKMM_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS}
  )
