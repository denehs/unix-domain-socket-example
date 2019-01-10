#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "ipc.h"

int main() {
	int fd;
	struct sockaddr_un addr;
	int ret;
	char buff[8192];
	struct sockaddr_un from;
	int ok = 1;
	int len;
	socklen_t fromlen = sizeof(from);

	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		ok = 0;
	}

	if (ok) {
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, SERVER_SOCK_FILE);
		unlink(SERVER_SOCK_FILE);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			ok = 0;
		}
	}

	while ((len = recvfrom(fd, buff, 8192, 0, (struct sockaddr *)&from, &fromlen)) > 0) {
		printf ("recvfrom: %s\n", buff);
		strcpy (buff, "transmit good!");
		ret = sendto(fd, buff, strlen(buff)+1, 0, (struct sockaddr *)&from, fromlen);
		if (ret < 0) {
			perror("sendto");
			break;
		}
	}
	

	if (fd >= 0) {
		close(fd);
	}

	return 0;
}
