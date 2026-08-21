add_executable(test_misere
   MockSocket.cpp
   TestHttpClient.cpp
   TestHTTP.cpp
   TestHttpException.cpp
   TestHttpRequest.cpp
   TestHttpResponse.cpp
   TestHttpServer.cpp
   TestHttpTransaction.cpp
   TestUrl.cpp
   Tests.cpp
)

# chaudiere comes in transitively via misere's own PUBLIC link to it.
target_link_libraries(test_misere PRIVATE misere poivre)

add_test(
   NAME test_misere
   COMMAND test_misere
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
