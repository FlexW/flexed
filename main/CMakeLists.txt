find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTKSOURCEVIEWMM gtksourceviewmm-3.0)

link_directories(
  ${GTKMM_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS}
  )

include_directories(
  ${GTKMM_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS}
  )

add_executable(
  flexed
  main.cpp
  )

target_link_libraries(
  flexed
  ${GTKMM_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  libflexed
  )
