//Logger.h file 
#ifndef LOGGER_H
#define LOGGER_H
//The logger will type-define an enumeration called LOG_LEVEL 
typedef enum {
	DEBUG,
	WARNING,
	ERROR,
	CRITICAL
}LOG_LEVEL;
//The logger will expose the following functions to each process via its header file (Logger.h):
int InitializeLog();
void SetLogLevel(LOG_LEVEL level);
void Log(LOG_LEVEL level, const char* prog, const char* func, int line, const char* message);
void ExitLog();
#endif //LOGGER_H
