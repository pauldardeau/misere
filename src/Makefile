# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

EXE_NAME = misere
SO_NAME = libmisere.so

# HttpServer.h unconditionally depends on armure (TLS support - see
# ../README.md's "TLS / HTTPS" section), and armure has no Makefile of
# its own: it's built via CMake, which also fetches and builds mbedTLS
# automatically. There's no way for this plain Makefile to produce a
# working build any more, so it fails fast here with a clear pointer
# instead of a confusing pile of linker errors.
all :
	@echo "error: the plain Makefile build no longer works." >&2
	@echo "HttpServer.h unconditionally depends on armure (TLS support), which" >&2
	@echo "has no Makefile of its own - it's built via CMake, which also" >&2
	@echo "fetches/builds mbedTLS automatically. See README.md." >&2
	@echo "" >&2
	@echo "Build with CMake instead, from the project root:" >&2
	@echo "  cmake -S . -B build" >&2
	@echo "  cmake --build build" >&2
	@exit 1

clean :
	rm -f *.o
	rm -f $(SO_NAME)
	rm -f $(EXE_NAME)
