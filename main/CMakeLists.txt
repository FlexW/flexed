find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTKSOURCEVIEWMM gtksourceviewmm-3.0)

SET(CMAKE_CXX_FLAGS
  ${CMAKE_CXX_FLAGS} "-Wno-#warnings"
  )

link_directories(
  ${GTKMM_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS}
  )

include_directories(
  ${GTKMM_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS}
  )

add_executable(
  fxed
  main.cpp
  )

target_link_libraries(
  fxed
  ${GTKMM_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  flexed
  )

install(
  TARGETS fxed
  DESTINATION "/usr/bin"
  )
