#include <systemd/sd-daemon.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *socket_path = "/run/srv.sk";
//char *socket_path = "./socket";
//char *socket_path = "\0hidden";

int main(int argc, char *argv[])
{
    char buf[100];
    int n,fd;

    n = sd_listen_fds(0);
    if (n > 1)
    {
        perror("Too many file descriptors received.\n");
        exit(1);
    }
    else if (n == 1)
    {
        fd = SD_LISTEN_FDS_START + 0;
    }
    else
    {
        struct sockaddr_un addr;

        if (argc > 1) socket_path=argv[1];

        if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("socket error");
            exit(-1);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("connect error");
            exit(-1);
        }

    }

    int rc;

    while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (write(fd, buf, rc) != rc) {
            if (rc > 0) fprintf(stderr,"partial write");
            else {
                perror("write error");
                exit(-1);
            }
        }
    }

    return 0;
}
