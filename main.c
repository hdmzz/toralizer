#include "toralize.h"

/**
 * Struct de la proxy REQUEST
 * 		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----+
    	   1    1      2              4           variable       1      ======== nombres de bytes

    ou VN should be 4, CD should be 1 for command (command code)
*/
Req *request(const char *dstip, const int dstport)
{
    Req *req;

    req = malloc(reqsize);
    if (req == NULL) {
        fprintf(stderr, "Error malloc request");
        return NULL;
    };
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    //!req->userid = USERNAME; donnera une erreur l'expression doit etre une valeur modifiable pour regler ce pb ==>>> strncpy
    strncpy(req->userid, USERNAME, 8);//8 car le userid est definie de taille constante 8 dans la structure proxy_request

    return (req);
}

int main(int ac, char **av)
{
    char                *host;
    Req                 *req;
    Res                 *res;
    struct sockaddr_in  sock;
    char                buff[ressize];
    char                tmp[512];
    int                 port;
    int                 success;
    int                 s;

    if (ac < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", av[0]);
        return (-1);
    };

    host = av[1];
    port = atoi(av[2]);
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "Error socket return less than zero\n");
        return (s);
    };
    
    
    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (connect(s, (struct sockaddr *)&sock, sizeof(sock))) {
        fprintf(stderr, "Error connect function: %s\n", strerror(errno));
        return (-1);
    };

    printf("Connected to proxy\n");

    req = request(host, port);
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

    printf("Success connected to the proxy to %s:%d\n", host, port);
    
    memset(tmp, 0, 512);

    snprintf(tmp, 512,
        "TEST / HTTP/1.0\r\n"
        "Host: www.hugodam.cloud\r\n"
        "\r\n"
    );

    //on a ecrit un header maintenant il faut ecrire dans le file descriptor puis remettre a zero le buffer tmp et 

    write(s, tmp, 511);

    memset(tmp, 0, 512);

    if (read(s, tmp, 511) < 0)
    {
        perror("Error read return of response from the serveur");
        return (free(req), close(s), -1);
    };

    printf("Retour de requete: %s\n", tmp);
    
    free(req);
    close(s);

    return (0);
};
