cmake_minimum_required(VERSION 3.16)

project(misere
   DESCRIPTION "A simple, high-performance C++ HTTP application server"
   LANGUAGES CXX
)

# True only when misere is configured directly, not pulled in via
# add_subdirectory() by a consumer. Controls whether misere's own test
# suite is built by default.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(MISERE_IS_TOP_LEVEL TRUE)
else()
   set(MISERE_IS_TOP_LEVEL FALSE)
endif()

option(MISERE_BUILD_TESTS "Build misere's own test suite" ${MISERE_IS_TOP_LEVEL})

# chaudiere's own CHAUDIERE_BUILD_TESTS defaults off here too (it uses
# the same CMAKE_SOURCE_DIR check, and this isn't chaudiere's own
# top-level configure), so this doesn't pull in chaudiere's tests or a
# second copy of poivre.
add_subdirectory(chaudiere)
add_subdirectory(src)

if(MISERE_BUILD_TESTS)
   add_subdirectory(poivre)
   enable_testing()
   add_subdirectory(tests)
endif()
