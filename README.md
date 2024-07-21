# http-server

## Brief
A simple HTTP server for learning purpose. It supports basic GET and POST methods, static file serving, thread pooling, LRU caching, and logging.


## Prerequisites
- [spdlog](https://github.com/gabime/spdlog) (for logging)
- [oha](https://github.com/hatoo/oha) (for concurrency testing)


## How to build and run
```sh
mkdir build
cd build
cmake ..
make -j
./http-server
```


## Supported Features
### Basic GET method
**Default (home.html)**
```sh
curl http://localhost
```
- The default URL of GET method is “home.html”


**GET home.html**
```sh
curl http://localhost:8080/home.html
```
- Get server files (example: home.html)


**Echo**
```sh
curl http://localhost:8080/echo
```
- The endpoint of echo in GET method


### Static File Serving for GET method

[Demo video on youtube](https://www.youtube.com/embed/jR403dP_xM4?si=ZpQ780OJhhJx498Z)
> btw, I use Neovim and tmux. You can check out my configuration [here](https://github.com/kfwu1999/dotfiles).


### Concurrency Test
**Running a Concurrency Test**

```sh
oha http://localhost:8080
```
- Perform a concurrency test using [oha](https://github.com/hatoo/oha)

[Demo video on youtube](https://www.youtube.com/watch?v=5g29z_3Lq2c)


### Basic POST Method
**Echo**
```sh
curl -X POST http://localhost:8080/echo
```
- The endpoint of echo in POST method


**Upload data to the server.**

```sh
curl -D - -X POST -H "Content-Type: text/plain" -d "Hello!" http://localhost:8080/upload
```
- create "uploads/" directory (default).
- write contents "Hello!" to "uploads/uploaded_file.txt"


## File overview
- `include/cache.h`, `src/cache.cpp`
    - LRU cache implementation.

- `include/file.h`, `src/file.cpp`
    - file-related utilities.

- `include/log.h`, `src/log.cpp`
    - logging by [spdlog](https://github.com/gabime/spdlog)

- `include/net.h`, `src/net.cpp`
    - Low-level networking code.

- `include/request.h`, `src/request.cpp`
    - HTTP request handling.

- `include/response.h`, `src/response.cpp`
    - HTTP response building.

- `include/server.h`, `src/server.cpp`
    - HTTP server implementation.

- `include/thread_pool.hpp`
    - thread pool implementation (using .hpp for template code).


## Reference
- The cat image of status code is from [https://http.cat/](https://http.cat/).
- The feature inspirations are mostly from [luiscmartinez/C-Web-Server](https://github.com/luiscmartinez/C-Web-Server/tree/master).
- The coding challenge from [CodeCrafters: Build your own HTTP server](https://app.codecrafters.io/courses/http-server) guided the development process.
