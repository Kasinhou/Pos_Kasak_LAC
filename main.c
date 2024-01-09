#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

//vstupne data od pouzivatela
typedef struct InputData {
    char rows[4];
    char columns[4];
    char numberOfAnts[4];
    char movement[4];
} INPUT_DATA;

//inicializacia vstupnych dat, priradenie
void input_data_init(INPUT_DATA *input_data, const char *rows, const char *columns,
                     const char *numberOfAnts, const char *movement) {

    strncpy(input_data->rows, rows, sizeof(input_data->rows) - 1);
    input_data->rows[sizeof(input_data->rows) - 1] = '\0';

    strncpy(input_data->columns, columns, sizeof(input_data->columns) - 1);
    input_data->columns[sizeof(input_data->columns) - 1] = '\0';

    strncpy(input_data->numberOfAnts, numberOfAnts, sizeof(input_data->numberOfAnts) - 1);
    input_data->numberOfAnts[sizeof(input_data->numberOfAnts) - 1] = '\0';

    strncpy(input_data->movement, movement, sizeof(input_data->movement) - 1);
    input_data->movement[sizeof(input_data->movement) - 1] = '\0';
}

//serializacia na spravny prenos dat
void serialize_input_data(INPUT_DATA *data, char *serMess) {
    snprintf(serMess, 1024, "%s;%s;%s;%s;", data->rows, data->columns, data->numberOfAnts, data->movement);
}


int main(int argc, char *argv[])
{
    //otazky
    char rows[4];
    char columns[4];
    printf("\n--------------------------------------Langton's ants----------------------------------------\n");
    printf("Enter number which will represent number of rows in the world: ");
    scanf("%s", rows);

    printf("Enter number which will represent number of columns in the world: ");
    scanf("%s", columns);
//---------------------------------------------------------------------------

    char numberOfAnts[4];
    char movement[4];
    printf("How many ants do you want in the world: ");
    scanf("%s", numberOfAnts);
    printf("Movement of ants:\n0 -> Direct\n1 -> Inverse\n"
           "Choose what movement do you want: ");
    scanf("%s", movement);

    printf("You choose a %s movement with %s ants.\n", movement, numberOfAnts);
    //movement = (movement == 1) ? movement : -1;

    //printf("%s, %s, %s, %s", rows, columns, numberOfAnts, movement);

//---------------------------------------------------------------------------

//vytvorenie socketov a ich spracovanie
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[1024];


    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }


    printf("Please enter a message: ");
    bzero(&buffer,1024);

    INPUT_DATA input_data;
    input_data_init(&input_data, rows, columns, numberOfAnts, movement);
    //get_input_data(buffer);
    serialize_input_data(&input_data, buffer);
    //printf("%s je pocet riadkov", input_data.rows);

    //zapisanie na server
    n = write(sockfd, buffer, strlen(buffer));//+1
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }

    //cyklus ohladom prijatia odpovede
    while (true) {
        char buf[2048];
        bzero(buf, 2048);
        read(sockfd, buf, 2047);

        printf("%s\n", buf);
    }
    close(sockfd);

    return 0;
}