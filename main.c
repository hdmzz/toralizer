#include "toralize.h"

/**
 * Struct de la proxy REQUEST
 * 		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----+
    	   1    1      2              4           variable       1      ======== nombres de bytes

    ou VN should be 4, CD should be 1 for command (command code)
*/
Req *request(struct sockaddr_in *sock)
{
    Req *req;

    req = malloc(reqsize);
    if (req == NULL) {
        fprintf(stderr, "Error malloc request");
        return NULL;
    };
    req->vn = 4;
    req->cd = 1;
    req->dstport = sock->sin_port;
    req->dstip = sock->sin_addr.s_addr;
    //!req->userid = USERNAME; donnera une erreur l'expression doit etre une valeur modifiable pour regler ce pb ==>>> strncpy
    strncpy(req->userid, USERNAME, 8);//8 car le userid est definie de taille constante 8 dans la structure proxy_request

    return (req);
}

int connect(int s2, const struct sockaddr *sock2, socklen_t addrlen)
{

    Req                 *req;
    Res                 *res;
    struct sockaddr_in  sock;
    char                buff[ressize];
    int                 success;
    int                 s;
    int                 (*p)(int, const struct sockaddr*, socklen_t);

    (void)addrlen;
    
    p = dlsym(RTLD_NEXT, "connect");//donc p fait reference a la fonction connect original
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "Error socket return less than zero\n");
        return (s);
    };

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (p(s, (struct sockaddr *)&sock, sizeof(sock))) {
        fprintf(stderr, "Error connect function: %s\n", strerror(errno));
        return (-1);
    };

    printf("Connected to proxy\n");

    req = request((struct sockaddr_in *)sock2);
    write(s, req, reqsize);

    memset(buff, 0, ressize);
    if (read(s, buff, ressize) < 0) {
        perror("Read function failed: ");
        free(req);
        close(s);

        return (-1);
    };

    res = (Res *)buff;//ca marche car buff a été déclaré de taille constante "ressize" il faut caster pour dire au compilateur que lon veut interpreter buff comme un Res *
    success = (res->cd == 90);

    if (!success) {
        fprintf(stderr, "Error the requets did not pass through the proxy error code: %d\n", res->cd);
        free(req);
        close(s);

        return (success);
    };

    dup2(s, s2);
    free(req);
    close(s);

    return (0);
};
