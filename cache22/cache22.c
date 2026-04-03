// cache22.c
#include"cache22.h"

int32 handle_hello(Client *cli,int8 *folder, int8 *args);

bool server_continuation;
bool child_continuation;

int32 handle_hello(Client *cli,int8 *folder, int8 *args){
    dprintf(cli->s, "Hello, %s:%d\n", cli->ip, cli->port);
    return 0;
}

CmdHandler handlers []= {
    {(int8 *)"hello", handle_hello},
   

};

Callback getcmd(int8 *cmd){
    Callback cb;
    int16 n,arraylength;
    if (sizeof(handlers )< 16 )
        return 0;
    arraylength = sizeof(handlers) / 16;
    cb = 0;
    for (n = 0; n < arraylength; n++)
    {
        if(!strcmp((char *)cmd, (char *)handlers[n].cmd)){
            cb = handlers[n].handler;
            break;
        }
    }
         return cb;
}

void zero(int8 *buf, int16 size)
{
    int8 *p;
    int16 n;
    for (n = 0, p = buf; n < size; n++,p++){
        *p = 0;
    }
    return;
}



void childloop(Client *cli){
    int8 buf[256];
    int8 *p,*f;
    int16 n;
    int8 cmd[256], folder[256], args[256];

    // write(cli->s,"hey\n",4);
    // exit(0);

    zero(buf,256);
    read(cli->s, (char*)buf, 255);
    n = (int16)strlen((char*)buf);
    if (n>254){ n = 254; }

    

    //select /Users/goro
    // create /Users/logins
    // insert /Users/bato hdhjoewuufcwhoABV
    for (p = buf;
         (*p) 
        && (n--)
        && (*p != ' ')
        && (*p != '\n') 
        && (*p != '\r')

        ; p++);

    zero(cmd, 256);
    zero(folder, 256);
    zero(args, 256);

    if (!(*p) || (!n))
    {
        strncpy((char*)cmd,(char *)buf,255);
        goto done;
    }
    else if(*p == ' '){
         *p = 0;
        strncpy((char*)cmd,(char *)buf,255);


    }
    else if (
       // (*p == ' ') || 
    (*p == '\n') || (*p == '\r')){
        *p = 0;
        strncpy((char*)cmd,(char *)buf,255);
        goto done;
    }

    for (p++ , f=p;
         (*p) 
         && (n--)
         && (*p != ' ')
        && (*p != '\n')
        && (*p != '\r')


         ; p++);


    if (!(*p) || (!n))
    {
        strncpy((char*)folder,(char *)f,255);
        goto done;

        }
    else if ((*p == ' ') || (*p == '\n') || (*p == '\r')){
        *p = 0;
        strncpy((char*)folder,(char *)f,255);
    }
    p++;

    if (*p){

        strncpy((char *)args, (char *)p, 255);
        for (p = args; (*p) && (*p != '\n') && (*p);p++)
            ;

            *p = 0;
    }

    done:
        dprintf(cli->s, "\ncmd:\t%s\n", cmd);
        dprintf(cli->s, "folder:\t%s\n", folder);
        dprintf(cli->s, "args:\t%s\n", args);

        return;

}

void mainloop(int s){
    struct sockaddr_in cli;
    int s2;
    int32 len;
    char *ip;
    int16 port;
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli,(unsigned int *)&len);
    if (s2<0){
       // sleep(1);
        return;
    }

    port = (int16)htons((int)cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);
    printf("Connection from  %s:%d\n",ip,port);

    client = (Client *)malloc(sizeof(struct s_client));
    assert(client);

    zero((int8 *)client,sizeof(struct s_client));
    client->s = s2;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid = fork();
    if(pid){
        free(client);

        return;
    }
    else{
        dprintf(s2,"100 Connected to cache22 server\n...");
        child_continuation = true;
        while(child_continuation){
        childloop(client);

        }
        close(s2);
        free(client);
        return ;
    }

    //return ;
}

int initserver(int16 port){
    struct sockaddr_in sock;
    int s;

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(HOST); 
    s = socket(AF_INET, SOCK_STREAM, 0);

    assert(s>0);
    errno = 0;
    if (bind(s, (struct sockaddr *)&sock, sizeof(sock)))
    {
        assert_perror(errno);
        exit(1);
    }
    errno = 0;
    if (listen(s, 20))
    {
        assert_perror(errno);
    }
    
    printf("Server listenning on %s:%d\n",HOST,port);

    return s;
}

int main(int argc, char *argv[]){
    char *sport;
    int16 port;
    int s;

    Callback x;


    x = getcmd((int8 *)"hello");
    printf("main: %p\n", x);

    // x = getcmd((int8 *)"maslfhkjg");
    // printf("main: %p\n", x);
    
    return 0;

    if(argc<2){
        sport = PORT;
    }
    else{
        sport = argv[1];
    }

    port = (int16)atoi(sport);

    s=initserver(port);

    server_continuation = true;
    while(server_continuation){
        mainloop(s);

        
    }

    printf("Shutting down... \n");
    close(s);

    return 0;
 }

 #pragma GCC diagnostic pop