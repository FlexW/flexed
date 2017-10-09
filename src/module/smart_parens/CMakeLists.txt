find_package(PkgConfig)

SET(GCC_COVERAGE_LINK_FLAGS "-fPIC")
SET(
  CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}"
  )

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

add_library(flexed_smart_parens SHARED smart_parens_flexed.cpp)

target_link_libraries(
  flexed_smart_parens
  ${GTKMM_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  libflexed
  )
