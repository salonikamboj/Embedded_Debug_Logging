# Embedded_Debug_Logging


## Project Description
### Overview
The Embedded Debug Logging project addresses the challenges of diagnosing and troubleshooting issues in a product deployed at customer sites. Traditional testing may not catch every defect, and when malfunctions occur in the field, it's often challenging to identify the root cause. This project introduces an embedded logging mechanism, allowing code to generate informative logs that are sent to a central server. This enables efficient debugging, particularly for problems that are hard to replicate in a controlled environment.

### Business Case
In the software development lifecycle, effective logging is crucial for post-deployment issue resolution. The ability to remotely collect detailed logs from customer environments enhances the company's capability to diagnose and address problems swiftly. This proactive approach helps maintain customer confidence and protects the company's reputation.

### Design
Each Process
  * Initializes logging for its process.
  * Sets the filter level severity.
  * Calls log functions to report activity.
  * Shuts down the logger before exiting.
Logger
  * Code resides in one file, included in the Makefile for each process.
  * Communicates with the server via an IP address and port.
  * Collects logs and filters them based on severity.
  * Uses UDP for fast log delivery.
  * Separate thread for reading UDP messages from the server.
Server
  * Binds a socket to its IP address and an available port.
  * Writes messages to the logger via UDP.
  * Options to dump the log file to the screen and overwrite the filter level severity.
  * Graceful shutdown via ctrl-C.
  * Separate thread for reading UDP messages from the logger.

### Implementation
Each Process
  * Includes Logger.h file.
  * Calls InitializeLog(), SetLogLevel(), Log(), and ExitLog().
  * Logs contain severity, filename, function name, line number, and a message.
Logger
  * Defines LOG_LEVEL enumeration for severity.
  * Provides functions: InitializeLog(), SetLogLevel(), Log(), and ExitLog().
  * Initializes a non-blocking UDP socket, sets server address, and starts the receive thread.
  * Implements mutexing for shared resources.
Server 
  * Main function creates a non-blocking UDP socket and binds it.
  * Starts a receive thread and presents a user menu with options.
  * Options include setting log level, dumping log file, and shutting down.
  * Receive thread writes received content to the server log file.

### Testing and Rework
  * Start the server on one machine and the test application on another.
  * Ensure correct IP address and port settings.
  * Observe log file growth by dumping to the screen or inspecting the file.
  * Change filter log severity on the server and observe log file changes.
  * Verify timestamps, log levels, file names, functions, line numbers, and messages for each log entry.

