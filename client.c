#include <stdio.h>
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

	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		ok = 0;
	}

	if (ok) {
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, CLIENT_SOCK_FILE);
		unlink(CLIENT_SOCK_FILE);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			ok = 0;
		}
	}

	if (ok) {
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, SERVER_SOCK_FILE);
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
			perror("connect");
			ok = 0;
		}
	}

	if (ok) {
		strcpy (buff, "iccExchangeAPDU");
		if (send(fd, buff, strlen(buff)+1, 0) == -1) {
			perror("send");
			ok = 0;
		}
		printf ("sent iccExchangeAPDU\n");
	}

	if (ok) {
		if ((len = recv(fd, buff, 8192, 0)) < 0) {
			perror("recv");
			ok = 0;
		}
		printf ("receive %d %s\n", len, buff);
	}

	if (fd >= 0) {
		close(fd);
	}

	unlink (CLIENT_SOCK_FILE);
	return 0;
}
