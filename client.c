#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ipc.h"


int main(int argc, char **argv) {
	struct sockaddr_un addr;
	struct sockaddr_un from;
	char buf[8192];
	int ret;
	int len;
	int fd;

	if (argc != 2) {
		printf("Usage: %s <message>\n", argv[0]);
		return 1;
	}

	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket()");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, CLIENT_SOCK_FILE);
	unlink(CLIENT_SOCK_FILE);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind()");
		close(fd);
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_SOCK_FILE);
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect()");
		close(fd);
		return 1;
	}

	strcpy(buf, argv[1]);
	if (send(fd, buf, strlen(buf) + 1, 0) == -1) {
		perror("send()");
		close(fd);
		return 1;
	} else {
		printf("sent: %s\n", argv[1]);
	}

	if ((len = recv(fd, buf, sizeof(buf), 0)) < 0) {
		perror("recv()");
	} else {
		printf("received %d bytes: %s\n", len, buf);
	}

	close(fd);

	unlink(CLIENT_SOCK_FILE);

	return 0;
}
