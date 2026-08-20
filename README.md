[![Build and test](https://github.com/pauldardeau/misere/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/pauldardeau/misere/actions/workflows/c-cpp.yml)

Misère
======
**Misère** is a simple, high-performance C++ HTTP application server. It's
not a general-purpose web server (no static file serving, no caching) - you
write handlers, wire them to paths, and Misère takes care of sockets,
threading, and HTTP parsing.

License
-------
BSD

Dependencies
------------
- **Chaudière** - C++ core functionality (sockets, threading, configuration,
  logging), included as a git submodule (`chaudiere/`). Required to build
  both `src/` and `tests/`.
- **Poivre** - C++ unit test framework, included as a git submodule
  (`poivre/`). Only needed to build and run `tests/`; the library itself
  (`src/`) has no dependency on it.

Clone with `--recurse-submodules`, or after cloning:

```bash
git submodule update --init --recursive
```

Building
--------
```bash
make -C chaudiere/src  # builds libchaudiere.so
make -C src            # builds libmisere.so and the misere executable
make -C tests          # builds test_misere (needs the poivre submodule)
```

Objectives/Purpose
-------------------
1. Coding is fun!
2. Develop a 'from scratch' high-performance C++ server
3. HTTP seemed like a reasonable 'top-level' protocol for the server
4. This is NOT meant to be a general purpose web server (i.e., no support
   for serving up files, caching, etc.)

Groups of Functionality
------------------------

### Server

- **`HttpServer`** - the server itself. Construct it from a config file
  path (reads `misere.ini` - see below) or a bare port number, register
  handlers with `addPathHandler(path, handler)`, then call `run()`, which
  accepts connections and dispatches each one to a `HttpRequestHandler`,
  either synchronously or via a thread pool depending on configuration.
- **`HttpRequestHandler`** - parses one request off a socket, routes it to
  the registered handler for its path, and writes back the response.
  Malformed/truncated requests (a client that connects and disconnects
  early, a port scanner) are caught and logged rather than propagating.

### Handlers

Write your own by extending **`AbstractHandler`** (which implements
`HttpHandler`'s `init()`/`isAvailable()` for you) and overriding
`serviceRequest(const HttpRequest&, HttpResponse&)`. A handful of built-in
handlers ship with Misère and can be turned on via `allow_builtin_handlers`
in `misere.ini`:

| Path | Handler | Description |
| --- | --- | --- |
| `/Echo` | `EchoHandler` | echoes the request (method, path, headers) back |
| `/GMTDateTime` | `GMTDateTimeHandler` | current time in GMT |
| `/ServerDateTime` | `ServerDateTimeHandler` | current server local time |
| `/ServerStatus` | `ServerStatusHandler` | current server status |
| `/ServerStats` | `ServerStatsHandler` | server statistics |
| `/ServerObjectsDebugging` | `ServerObjectsDebugging` | helps find memory leaks in the server itself |

Handlers can also be loaded from a shared library at runtime via the
`[handlers]`/module sections of `misere.ini`, for deploying handlers
without recompiling the server.

### Request & Response

- **`HttpTransaction`** - shared base for headers, body, and protocol
  version, used by both of the below.
- **`HttpRequest`** - a parsed incoming request (method, path, headers,
  arguments) on the server side, or an outgoing request you build and send
  on the client side.
- **`HttpResponse`** - the response your handler populates
  (`setStatusCode()`, `setBody()`, headers) on the server side, or a
  parsed response on the client side.

### Client

- **`HttpClient`** - for making outbound HTTP calls: `get()`, `head()`,
  `put()`, `post()`, `do_delete()` against an `HttpRequest`, plus a
  self-contained `post(address, port, url, postData, contentType, headers)`
  that doesn't require building an `HttpRequest` at all.
- **`HttpException`** - thrown by response parsing for a 4xx/5xx status,
  carrying the status code and reason phrase.

### Supporting

- **`Url`** - parses a `protocol://host[:port]/path` string into its parts.
- **`HTTP`** - protocol/method/header-name constants, plus
  `responseLineForStatusCode(int)` for mapping a numeric status code to its
  full "`404 Not Found`"-style response line.

Quick Start
-----------
Both examples below were compiled and run against the real library while
writing this README.

**Server** - a minimal custom handler:

```cpp
#include "AbstractHandler.h"
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ByteBuffer.h"

using namespace misere;

class HelloHandler : public AbstractHandler {
public:
   void serviceRequest(const HttpRequest& request, HttpResponse& response) override {
      response.setBody(new chaudiere::ByteBuffer("<html><body>Hello, misere!</body></html>"));
   }
};

int main() {
   HttpServer server("misere.ini");
   server.addPathHandler("/hello", new HelloHandler);
   return server.run();  // blocks, serving requests
}
```

**Client** - fetch it back:

```cpp
#include "HttpClient.h"
#include "HttpResponse.h"
#include "KeyValuePairs.h"

using namespace misere;

HttpClient client;
chaudiere::KeyValuePairs noAddlHeaders;

HttpResponse* response =
   client.post("127.0.0.1", 9500, "/hello", "", "text/plain", noAddlHeaders);

// response->getStatusCode() == 200
// response->getBody() contains "<html><body>Hello, misere!</body></html>"

delete response;
```

Configuration File
-------------------
Misère uses an `.ini` file for configuration - simple, and simple is good.
`src/misere.ini` is a fully-commented example covering the listening port,
threading model (`pthreads`, `c++11`, `gcd_libdispatch`, or `none`),
socket-vs-kernel-events handling, log level, built-in handlers, the server
identification string, and dynamically-loaded handler modules.

Running
-------
```bash
./misere misere.ini
```

Testing
-------
```bash
make -C chaudiere/src
make -C src
make -C tests
LD_LIBRARY_PATH=src:chaudiere/src tests/test_misere
```

Load Testing
------------
Using siege: `siege -b -r 10 -c 100 http://127.0.0.1:13001/Echo`

Apache bench (ab): `ab -t 60 -n 1000 -c 400 http://127.0.0.1:13001/Echo`

Platforms/Compilers
--------------------
This project is implemented with C++20. It is tested on Linux (Ubuntu,
Debian, Mint), FreeBSD, and OpenBSD.

Meaning of Misère
------------------
What does 'Misère' mean?  It's a French word that means "trouble" or "misery".
Fitting for a C++ project name, no?  All the English words that I thought
about using were already being used in 1 or more projects on github, so I
tried to find something that was unique.  I grew up in South Louisiana and
got to hear a fair bit of Cajun French spoken when I was growing up.  It
was not uncommon for a mischievous kid to be told something like "You're
giving me the misères!"
