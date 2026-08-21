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
threading model, socket handling, log level, keep-alive, built-in handlers,
the server identification string, and dynamically-loaded handler modules.

### Choosing `threading`

- **`pthreads`** (default) and **`c++11`** (`std::thread`) are functionally
  equivalent - benchmarked head-to-head under identical load, the numbers
  were indistinguishable from run-to-run noise. That's expected: chaudière's
  `StdThread`/`PthreadsThread` are both thin wrappers over the same OS
  thread primitives (`std::thread` is itself implemented on top of pthreads
  on Linux). Pick whichever you prefer; there's no performance reason to
  choose one over the other.
- **`gcd_libdispatch`** has no backing implementation in chaudière yet - the
  server logs a warning and falls back to `pthreads` if configured.
- **`none`** disables threading entirely (one request at a time, no pool).
  Useful for debugging, not for production.

### Choosing `sockets`

- **`socket_server`** (default) accepts connections and hands each one to a
  thread-pool worker.
- **`kernel_events`** uses epoll (Linux) or kqueue (BSD/macOS) to multiplex
  many connections through a small number of threads, only handing a
  connection to a worker once it actually has data to read.

  With `keep_alive = false`, this is where `kernel_events` is meant to
  shine: an idle connection costs an epoll registration, not a dedicated
  thread. With `keep_alive = true`, though, a worker that picks up a
  persistent connection holds it - via a blocking read - for the
  connection's *entire* lifetime, exactly like `socket_server` does; the fd
  isn't handed back to epoll until the whole keep-alive session ends, so
  epoll's actual advantage (many idle connections, few threads) never gets
  exercised by continuously-busy clients. Benchmarked with `keep_alive =
  true` under saturated load, `socket_server` matched or slightly beat
  `kernel_events` - `kernel_events` pays epoll's per-request bookkeeping
  cost (`epoll_ctl` add/remove, busy-fd tracking) without the offsetting
  benefit in that scenario. We haven't benchmarked the workload
  `kernel_events` is actually designed for (many concurrent connections
  that are mostly idle between requests), so this isn't "kernel_events is
  never worth it" - just that it isn't a free win once keep-alive is on.

  **Default to `socket_server`** unless you have a specific large-scale,
  mostly-idle-connections workload that justifies `kernel_events`.

### Sizing `thread_pool_size` when `keep_alive = true`

Without keep-alive, a worker thread is freed back to the pool almost
immediately after each request, so `thread_pool_size` mostly just bounds
*request* concurrency. With keep-alive, a worker is held for as long as its
connection stays alive - so `thread_pool_size` needs to cover your expected
*peak concurrent connections*, not just peak request rate. In testing,
undersizing the pool (e.g. 40 concurrent persistent clients against a pool
of 16) reliably destabilized the process under sustained load; sizing the
pool to match or exceed expected concurrent connections resolved it
cleanly. If you can't bound concurrent connections in advance, prefer
leaving `keep_alive = false`, or keep `keep_alive_max_requests`/
`keep_alive_timeout` low enough to keep connection turnover high.

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
