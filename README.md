## Simple HTTP Server with Thread Pooling
This project is a simple HTTP server implementation in C++ that serves HTML web pages and handles HTTP GET requests using a thread pool for efficient concurrent client handling.

# Features
* Handles HTTP GET requests for static HTML files.
* Utilizes a thread pool to handle multiple client connections concurrently.
* Supports basic HTTP 1.0 protocol.
* Provides dynamic HTTP responses for specific URLs.
* Optional support for additional HTTP functionality (not implemented in provided code).

# Worker Thread Pool
The worker thread pool design involves creating a pool of reusable worker threads at the start of the server. Instead of creating a new thread for every client connection, the server now places accepted client file descriptors in a shared queue. Worker threads fetch clients from this queue, process their requests, and return to the queue for the next client. This efficient approach reduces the overhead associated with thread creation.

# Usage
**Compilation**: Compile the server using the provided source files.<br>

g++ -o server simple_server.cpp http_server.cpp -lpthread <br>
**Execution**: Run the server, providing a port number as an argument. <br>

./server  &lt;port&gt; <br>
**Accessing Server**: Open a web browser and navigate to http://localhost:&lt;port&gt;. Replace &lt;port&gt; with the port number you specified when running the server. <br>

**Testing**: You can test the server by accessing different URLs and observing the responses. You can also use command-line tools like wget to interact with the server.

# Configuration
Modify the SIZE_THREAD_POOL constant in simple_server.cpp to adjust the number of worker threads in the pool.
Configure the paths to HTML files and dynamic response logic in the provided code (http_server.cpp).

# Acknowledgments
Special thanks to the instructors and creators of the bootcamp for providing valuable guidance.
