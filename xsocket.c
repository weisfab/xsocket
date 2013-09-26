#include "xsocket.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
static int lsocket_accept(lua_State *L);
static int set_socket_noblock(sockfd){
    int flag;
    if ((flag = fcntl(sockfd, F_GETFL, 0) <0)){
        return -1;
    }
    
    flag |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flag) < 0){
        return -1;
    }
    return 0;
}
static int lsocket_listen(lua_State *L){
    int sockfd=luaL_checkint(L, 1);
    int r=listen(sockfd,50);
    lua_pushnumber(L, r);
    return 1;
}

static int lsocket_bind(lua_State *L)
{
    int sockfd=luaL_checkint(L, 1);
    const char* add=luaL_checkstring(L, 2);
    int port = luaL_checknumber(L, 3);
    struct sockaddr_in serv_addr;
    memset( &serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(add);
    serv_addr.sin_port = htons(port);
    int r=bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    lua_pushnumber(L, r);
    return 1;
}

static int lsocket_connect(lua_State *L)
{
    struct sockaddr_in sa;
    struct hostent *hp;
    
    int sockfd=luaL_checknumber(L, 1);
    const char *host=luaL_checkstring(L, 2);
    int port = luaL_checkint(L, 3);
    /* Get the host. */
    hp = gethostbyname(host);
    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons(port);
    sockfd = socket(hp->h_addrtype, SOCK_STREAM, 0);
    /* connect */
    int ret = connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));
    lua_pushnumber(L, ret);
    return 1 ;
}


static int lsocket_read(lua_State *L)
{
    
    int sockfd=luaL_checknumber(L, 1);
    int expectlen=lua_tonumber(L, 2);
    int n = 0;
    char buffer[expectlen];
    memset( buffer, 0x0, sizeof(buffer));
    n = (int)read(sockfd,buffer,sizeof(buffer));
    if (n<0) {
        lua_pushnumber(L, 0 );
        lua_pushnil(L);
    }else if(n==0){
        lua_pushnumber(L, -1 );
        lua_pushnil(L);
    }else{
        lua_pushnumber(L, n );
        lua_pushlstring(L, buffer, n );
    }
    return 2;
}

static int lsocket_write(lua_State *L){
    int sockfd=luaL_checknumber(L, 1);
    size_t len;
    const char *data=luaL_checklstring(L, 2, &len);
    int n=(int)write( sockfd, data, len);
    if (n<0) {
        if((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
            lua_pushnumber(L, 0);
        }else{
            lua_pushnumber(L, -1);
        }
    }else if(n==0 ){
        lua_pushnumber(L, 0);
    }else{
        lua_pushnumber(L, n);
    }
    return 1;
}

static int lsocket_close(lua_State *L)
{
    int sockfd=luaL_checknumber(L, 1);
    lua_pushnumber(L, close(sockfd));
    return 1;
}
static int lsocket_sendfile(lua_State *L)
{
//    struct xsocket *s=lua_touserdata(L, 1);
//    const char *filename=luaL_checkstring(L, 2);
//    int sockfd=s->fd;
//    FILE *f=fopen(filename, "r");
//    int filefd=fileno(f);
//    //TODO
//    //sendfile(filefd, sockfd, 0, 0, NULL, 0);
    return 0;
}
static int lsocket_newsocket(lua_State *L){
    int socketfd=socket(AF_INET, SOCK_STREAM, 0);
    int on   = 1;
    setsockopt( socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    set_socket_noblock(socketfd);
    lua_pushinteger(L, socketfd);
    return 1;
}
static int lsocket_accept(lua_State *L)
{
    int sockfd=luaL_checkint(L, 1);
    socklen_t clilen;
    struct sockaddr_in  cli_addr;
    clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    set_socket_noblock(newsockfd);
    lua_pushinteger(L, newsockfd);
    lua_pushstring(L,inet_ntoa(cli_addr.sin_addr));
    return 2;
}


static const luaL_Reg socket_method[] ={
    {"new",		lsocket_newsocket},
    
    {"bind",		lsocket_bind},
    {"listen",		lsocket_listen},
    {"accept",		lsocket_accept},
    {"connect",		lsocket_connect},
    {"read",		lsocket_read},
    {"write",		lsocket_write},
    {"close",       lsocket_close},
    {"sendfile",    lsocket_sendfile},
    {NULL,		NULL}
};
static void signal_function(int signal) {
    fprintf(stderr, "singnal:%d\n",signal);
}
LUALIB_API int luaopen_xsocket (lua_State *L)
{
    signal(SIGPIPE, signal_function);
    luaL_newlib(L, socket_method);
    return 1;
}

