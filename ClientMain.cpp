#include "Client.h"

int main(int argc, char **argv){
    if(argc != 2)
        err_die("argument not enough");
    Client client(argv[1]);
    client.Start();
    return 0;
}
