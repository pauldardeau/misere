# Static by default (respects BUILD_SHARED_LIBS), same convention as
# poivre/chaudiere. Doesn't affect the Makefile-built libmisere.so.
add_library(misere
   AbstractHandler.cpp
   EchoHandler.cpp
   GMTDateTimeHandler.cpp
   HTTP.cpp
   HttpClient.cpp
   HttpConnection.cpp
   HttpException.cpp
   HttpRequest.cpp
   HttpRequestHandler.cpp
   HttpResponse.cpp
   HttpServer.cpp
   HttpSocketServiceHandler.cpp
   HttpTransaction.cpp
   ServerDateTimeHandler.cpp
   ServerObjectsDebugging.cpp
   ServerStatsHandler.cpp
   ServerStatusHandler.cpp
   SocketConnection.cpp
   Url.cpp
)

target_compile_features(misere PUBLIC cxx_std_20)
set_target_properties(misere PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(misere PRIVATE
   $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
)

# Headers live at this directory's root (no include/ subdirectory), so
# this is the one directory a consumer needs on its include path.
target_include_directories(misere
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:include/misere>
)

# PUBLIC: misere's own public headers use chaudiere types throughout
# (KeyValuePairs, Socket, etc.), so a consumer of misere also needs
# chaudiere's include directory and link library.
target_link_libraries(misere PUBLIC chaudiere)

# HttpServer.cpp uses poivre::AutoPointer<> directly (same situation as
# chaudiere's SocketServer.cpp) - implementation detail only, not in any
# misere public header, so PRIVATE: consumers of the compiled misere
# library don't need poivre. A plain include path rather than linking
# the poivre target, since the actual dependency is one header, not any
# of poivre's compiled code.
#
# SYSTEM (not plain PRIVATE): chaudiere ships its own same-named
# BasicException.h/Runnable.h, and several misere .cpp files quote-include
# "BasicException.h" expecting chaudiere's copy (paired with a bare
# `using namespace chaudiere;` for unqualified lookup). SYSTEM makes this
# -isystem instead of -I, which GCC/Clang search strictly after all -I
# dirs - so chaudiere's own PUBLIC include dir (added below) still wins
# for any header name that exists in both, exactly like the Makefile's
# -I../chaudiere/src -I../poivre ordering already relies on.
target_include_directories(misere SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../poivre)

include(GNUInstallDirs)

install(TARGETS misere
   EXPORT misereTargets
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

file(GLOB MISERE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${MISERE_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/misere)

# the misere CLI server executable - target name differs from the
# library target (CMake target names must be unique) but the actual
# output binary is still named "misere", matching the Makefile.
add_executable(misere_cli main.cpp)
set_target_properties(misere_cli PROPERTIES OUTPUT_NAME misere)
target_link_libraries(misere_cli PRIVATE misere)

install(TARGETS misere_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
