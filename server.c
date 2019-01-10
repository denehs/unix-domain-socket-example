#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ipc.h"


int main() {
	struct sockaddr_un addr;
	struct sockaddr_un from;
	socklen_t fromlen = sizeof(from);
	char buf[8192];
	int ret;
	int len;
	int fd;

	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket()");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_SOCK_FILE);
	unlink(SERVER_SOCK_FILE);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(fd);
		perror("bind()");
		return 1;
	}

	while ((len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) > 0) {
		printf("received %d bytes: %s\n", len, buf);
		strcpy(buf, "transmit good!");
		ret = sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr *)&from, fromlen);
		if (ret < 0) {
			perror("sendto()");
			break;
		}
	}

	close(fd);

	return 0;
}
