//Logger.cpp 
#include "Logger.h"
#include <arpa/inet.h>
#include <iostream>
#include <queue>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
using namespace std;
static LOG_LEVEL filter_log_level = DEBUG;
const int PORT = 1153;
const char IP_ADDR[] = "127.0.0.1";
const int BUF_LEN = 4096;
bool is_running = true;
void* recv_func(void* arg);
int fd, ret, len;
struct sockaddr_in servaddr;
struct sockaddr_in logaddr;
char buf[BUF_LEN];
pthread_mutex_t lock_x;
pthread_t tid;

int InitializeLog() {
	//create a non-blocking socket for UDP communications 
	fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
	if (fd < 0) {
		cout << "Cannot create the socket" << endl;
		cout << strerror(errno) << endl;
		return -1;
	}
	//Set the address and port of the log address.
	memset((char*)&logaddr, 0, sizeof(logaddr));
	logaddr.sin_family = AF_INET;
	logaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	logaddr.sin_port = htons(8080);

	ret = bind(fd, (struct sockaddr*)&logaddr, sizeof(logaddr));
	if (ret < 0) {
		cout << "Cannot bind the socket to the local address" << endl;
		cout << strerror(errno) << endl;
		return -1;
	}
	//Set the address and port of the server.
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	ret = inet_pton(AF_INET, IP_ADDR, &servaddr.sin_addr);
	if (ret == 0) {
		cout << "No such address" << endl;
		cout << strerror(errno) << endl;
		close(fd);
		return -1;
	}
	servaddr.sin_port = htons(PORT);
	//Start the receive thread and pass the file descriptor to it.
	ret = pthread_create(&tid, NULL, recv_func, &fd);
	if (ret != 0) {
		cout << "Cannot start thread" << endl;
		cout << strerror(errno) << endl;
		close(fd);
		return -1;
	}
	return 0;

}
void SetLogLevel(LOG_LEVEL level) {
	//set the filter log level and store in a variable global within Logger.cpp.
	filter_log_level = level;
}
void Log(LOG_LEVEL level, const char* prog, const char* func, int line, const char* message) {
	//compare the severity of the log to the filter log severity. The log will be thrown away if its severity is lower than the filter log severity.
	if (level < filter_log_level) {
		return;
	}
	//create a timestamp to be added to the log message.
	time_t now = time(0);
	char* dt = ctime(&now);
	memset(buf, 0, BUF_LEN);
	char levelStr[][16] = { "DEBUG", "WARNING", "ERROR", "CRITICAL" };
	len = sprintf(buf, "%s %s %s:%s:%d %s\n", dt, levelStr[level], prog, func, line, message) + 1;
	buf[len - 1] = '\0';
	//The message will be sent to the server via UDP sendto().
	sendto(fd, buf, len, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
}
void ExitLog() {
	//stop the receive thread via an is_running flag and close the file descriptor.
	is_running = false;
	pthread_join(tid, NULL);
	close(fd);
}

void* recv_func(void* arg)
{
	int fd = *(int*)arg;//accept the file descriptor as an argument.
	char buf[BUF_LEN];
	memset(buf, 0, BUF_LEN);
	struct sockaddr_in recvaddr;
	socklen_t addrlen = sizeof(recvaddr);
	while (is_running) {
		int len = recvfrom(fd, buf, BUF_LEN, MSG_DONTWAIT, (struct sockaddr*)&recvaddr, &addrlen);
		if (len < 0) {
			sleep(1);
		}
		else {
			buf[len] = '\0';
			if (strcmp(buf, "Set Log Level=0") == 0) {
				cout << "setting debug" << endl;
				filter_log_level = DEBUG;
			}
			else if (strcmp(buf, "Set Log Level=1") == 0) {
				cout << "setting warning" << endl;
				filter_log_level = WARNING;
			}
			else if (strcmp(buf, "Set Log Level=2") == 0) {
				cout << "setting error" << endl;
				filter_log_level = ERROR;
			}
			else if (strcmp(buf, "Set Log Level=3") == 0) {
				cout << "setting critical" << endl;
				filter_log_level = CRITICAL;
			}
		}
	}

	pthread_exit(NULL);
}
