# External Dependencies

The C++ gateway currently avoids external dependencies and uses a very small blocking HTTP server based on system sockets.

For a production-quality extension, a header-only library such as `cpp-httplib` can be placed here and wired into `HttpServer`.

