// cache22.c
#include "cache22.h"

int32 handle_hello(Client *cli, int8 *folder, int8 *args);

bool server_continuation;
bool child_continuation;

int32 handle_hello(Client *cli, int8 *folder, int8 *args) {
    dprintf(cli->s, "Hello, %s:%d\n", cli->ip, cli->port);
    return 0;
}

CmdHandler handlers[] = {
    {(int8 *)"hello", handle_hello},
};

Callback getcmd(int8 *cmd) {
    int16 n, arraylength;
    if (sizeof(handlers) < 16) return 0;
    arraylength = sizeof(handlers) / 16;
    for (n = 0; n < arraylength; n++) {
        if (!strcmp((char *)cmd, (char *)handlers[n].cmd)) {
            return handlers[n].handler;
        }
    }
    return 0;
}

void zero(int8 *buf, int16 size) {
    int8 *p;
    int16 n;
    for (n = 0, p = buf; n < size; n++, p++) {
        *p = 0;
    }
}

void childloop(Client *cli) {
    int8 buf[256];
    int8 *p, *f;
    int16 n;
    int8 cmd[256], folder[256], args[256];

    zero(buf, 256);
    read(cli->s, (char*)buf, 255);
    n = (int16)strlen((char*)buf);
    if (n > 254) n = 254;

    zero(cmd, 256);
    zero(folder, 256);
    zero(args, 256);

    for (p = buf; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);

    if (!(*p) || (!n)) {
        strncpy((char*)cmd, (char *)buf, 254);
        goto done;
    } else {
        *p = 0; // Null terminate buf at the split
        //  Guarantees null-termination and silences the warning
        snprintf((char*)cmd, sizeof(cmd), "%s", (char*)buf);

        // strncpy((char*)cmd, (char *)buf, 254);

        cmd[255] = '\0';  
    }

    for (p++, f = p; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);

    if (!(*p) || (!n)) {
        strncpy((char*)folder, (char *)f, 254);
        goto done;
    } else {
        *p = 0;
        strncpy((char*)folder, (char *)f, 254);
    }

    p++;
    if (*p) {
        strncpy((char *)args, (char *)p, 254);
        for (f = args; (*f) && (*f != '\n') && (*f != '\r'); f++);
        *f = 0;
    }

done:
    dprintf(cli->s, "\ncmd:\t%s\n", cmd);
    dprintf(cli->s, "folder:\t%s\n", folder);
    dprintf(cli->s, "args:\t%s\n", args);
}

void mainloop(int s) {
    struct sockaddr_in cli;
    int s2;
    uint32_t len = sizeof(cli);
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli, &len);
    if (s2 < 0) return;

    client = (Client *)malloc(sizeof(struct s_client));
    assert(client);
    zero((int8 *)client, sizeof(struct s_client));
    
    client->s = s2;
    client->port = (int16)ntohs(cli.sin_port);
    strncpy(client->ip, inet_ntoa(cli.sin_addr), 15);

    pid = fork();
    if (pid > 0) {
        close(s2);
        free(client);
        return;
    } else if (pid == 0) {
        dprintf(s2, "100 Connected to cache22 server\n...");
        child_continuation = true;
        while (child_continuation) {
            childloop(client);
        }
        close(s2);
        free(client);
        exit(0);
    }
}

int initserver(int16 port) {
    struct sockaddr_in sock;
    int s;

    s = socket(AF_INET, SOCK_STREAM, 0);
    assert(s > 0);

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr("127.0.0.1"); // Or your HOST define

    if (bind(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
        perror("bind failed");
        exit(1);
    }
    listen(s, 20);
    return s;
}

int main(int argc, char *argv[]) {
    int16 port = 8080;
    if (argc > 1) port = (int16)atoi(argv[1]);

    int s = initserver(port);
    server_continuation = true;
    while (server_continuation) {
        mainloop(s);
    }
    close(s);
    return 0;
}
