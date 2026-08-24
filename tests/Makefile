# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

EXE_NAME = test_misere

# Same reasoning as ../src/Makefile: TLS support (armure) is CMake-only,
# so this plain Makefile can no longer produce a working build. Fails
# fast with a clear pointer instead of a confusing pile of linker
# errors (missing TestSocketTransport/TestTlsConnection/
# TestHttpsIntegration object files, then missing armure/mbedTLS
# symbols even once those are added back).
all :
	@echo "error: the plain Makefile build no longer works." >&2
	@echo "TLS support (armure) is CMake-only - see ../README.md." >&2
	@echo "" >&2
	@echo "Build and run tests with CMake instead, from the project root:" >&2
	@echo "  cmake -S . -B build" >&2
	@echo "  cmake --build build" >&2
	@echo "  ctest --test-dir build --output-on-failure" >&2
	@exit 1

clean :
	rm -f *.o
	rm -f $(EXE_NAME)
