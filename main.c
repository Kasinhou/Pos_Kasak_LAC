#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct InputData {
    char rows[4];
    char columns[4];
    char numberOfAnts[4];
    char movement[4];
} INPUT_DATA;

void input_data_init(INPUT_DATA *input_data, const char *rows, const char *columns,
                     const char *numberOfAnts, const char *movement) {
    /*input_data->rows = rows;
    input_data->columns = columns;
    input_data->numberOfAnts = numberOfAnts;
    input_data->movement = movement;*/
    /*sprintf(input_data->rows, (const char *) sizeof(input_data->rows), "%s", rows);
    sprintf(input_data->columns, (const char *) sizeof(input_data->columns), "%s", columns);
    sprintf(input_data->numberOfAnts, (const char *) sizeof(input_data->numberOfAnts), "%s", numberOfAnts);
    sprintf(input_data->movement, (const char *) sizeof(input_data->movement), "%s", movement);*/
    strncpy(input_data->rows, rows, sizeof(input_data->rows) - 1);
    input_data->rows[sizeof(input_data->rows) - 1] = '\0';

    strncpy(input_data->columns, columns, sizeof(input_data->columns) - 1);
    input_data->columns[sizeof(input_data->columns) - 1] = '\0';

    strncpy(input_data->numberOfAnts, numberOfAnts, sizeof(input_data->numberOfAnts) - 1);
    input_data->numberOfAnts[sizeof(input_data->numberOfAnts) - 1] = '\0';

    strncpy(input_data->movement, movement, sizeof(input_data->movement) - 1);
    input_data->movement[sizeof(input_data->movement) - 1] = '\0';
}

void serialize_input_data(INPUT_DATA *data, char *serMess) {
    snprintf(serMess, 1024, "%s;%s;%s;%s;", data->rows, data->columns, data->numberOfAnts, data->movement);
}

//treba toolchain dat hore aj na serveri
int main(int argc, char *argv[])
{
    //vytvorenie sveta so zadanymi rozmermi (definovat biele cierne mravcov - #O.)
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
    //generovanie nahodnych ciernych poli
    //rucne definovat cierne polia

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
    printf("%s je pocet riadkov", input_data.rows);
    /*buffer[0] = 'd';
    buffer[1] = 'h';
    buffer[2] = 'o';
    buffer[3] = 'j';*/
    //int result = snprintf(buffer, strlen(buffer), "%c;%c;%c;%c;",
       //                   rows, columns, numberOfAnts, movement);

    /*if (result >= 0 && (size_t)result < strlen(buffer)) {
        printf("\nSuccessfully serialized\n");
    } else {

        printf("\nFailed to serialize\n");
    }*/
    //fgets(buffer, 1023, stdin);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }

    bzero(buffer,1024);
    n = read(sockfd, buffer, 1023);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    printf("%s\n",buffer);
    close(sockfd);

    return 0;
}