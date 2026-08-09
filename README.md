*This project has been created as part of the 42 curriculum by [claghrab, zfarouk, hsacr].*

# WebServ

## Description
This project is a custom HTTP/1.1 web server written entirely in C++98. Inspired by Nginx, its primary goal is to handle multiple concurrent connections using a single-threaded multiplexing architecture. 

The server is capable of:
* Parsing custom `.conf` configuration files.
* Serving static web assets and handling file uploads.
* Routing `GET`, `POST`, and `DELETE` requests safely.
* Executing CGI scripts (like PHP and Python) to support dynamic content and session management.
* Managing non-blocking socket operations via the `select()` system call.

## Instructions

### Compilation
To compile the server, simply run the provided Makefile from the root directory:
```bash
make
```
### Execution
Start the server by passing a configuration file as the only argument. If no argument is provided, it falls back to a default configuration path:
```bash
./webserv [path/to/config.conf]
```
### Testing
Once running, you can test the server by navigating to the designated host and port in your browser:
```Plaintext
http://localhost:[PORT]
```
Alternatively, you can test specific HTTP methods using curl:
```bash
curl -i -X GET http://localhost:[PORT]/
```

## Resources

#### HTTP
>* [MDN - HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)
>* [An Overview of the HTTP Protocol as covered in RFCs](https://www.inspirisys.com/HTTP_Protocol_as_covered_in_RFCs-An_Overview.pdf)
>* [HTTP Status Codes](https://umbraco.com/knowledge-base/http-status-codes/)
>* [Hypertext Transfer Protocol -- HTTP/1.0](https://datatracker.ietf.org/doc/html/rfc1945)
>* [Hypertext Transfer Protocol -- HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616)
#### Networking

>* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
#### CGI

