/* CLIENTE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <gtk/gtk.h>

#define SERVER_PORT 1234
#define IP "127.0.0.1"//"172.20.10.3" //"192.168.5.4" //"127.0.0.1"
#define MAX_SIZE_C 10 // num max de caractere do nick name do jogador
#define SERVER_REPLY_MSG 20
#define RESP_OP "n"
#define T "t"
#define GANHOU "Ganhou a Rodada"
#define PERDEU "Perdeu a Rodada"

typedef struct{
  char nick_player[MAX_SIZE_C];
  char nick_adv[SERVER_REPLY_MSG];
  int pontosVc, pontosAdv;

} PLAYER;

int main(int argc , char *argv[])
{
    int sock;
    int flag=1, count=1;

    char op[1];
    char FLAG[2];
    char resultado_parcial[SERVER_REPLY_MSG];
    struct sockaddr_in server;
    char jogada[MAX_SIZE_C] , server_reply[SERVER_REPLY_MSG];
    char JogadaVc[SERVER_REPLY_MSG], JogadaAdv[SERVER_REPLY_MSG];

    PLAYER player;

    player.pontosVc=0, player.pontosAdv=0;

    system("clear");
    printf("Deseja conectar-se ao servidor [s/n]:");
    gets(op);
    if(strcmp(op, RESP_OP)==0){
      printf("\nCliente não foi conectado.\n");
      return 1;
    }

    //Cria socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Não foi possivel criar socket");
    }
    //puts("Socket Criado");

    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    //conecta ao servidor
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Conexão Falhou!");
        return 1;
    }
    puts("Conectado ao servidor\n");

    printf("Digite seu nickname (MAX %d CARACTERES):", MAX_SIZE_C);
    gets(player.nick_player);
    send(sock, player.nick_player, MAX_SIZE_C, 0);

    system("clear");
    puts("Aguardando outro Jogador...");// Aguardando outro Jogador

    if(recv(sock , player.nick_adv , SERVER_REPLY_MSG , 0) < 0){ // recebe o nome do adversario
        puts("recv1 falhou");
    }
    system("clear");
    //continua se comunicando com o servidor
    while(flag)//flag=1
    {
        puts("\n********************************** Inicio do Jogo *********************************");
        printf("\n\t\t\t\t\t\t\t  PLACAR\n");
        printf("___________________________________\n\n");
        printf(" 1: PEDRA   2: PAPEL   3:  TESOURA"); printf("\t\t|\t  [%d]  %s \n", player.pontosVc, player.nick_player);
        printf("___________________________________"); printf("\t\t|\t  [%d]  %s \n", player.pontosAdv, player.nick_adv);


        printf("\nEntre com sua jogada: ");
          gets(jogada);

        printf("aguardando a jogada de %s...\n", player.nick_adv);

        if(send(sock , jogada , strlen(jogada), 0) < 0){
            puts("send falhou!");
            return 1;
        }

        if(recv(sock , JogadaVc , SERVER_REPLY_MSG, 0) < 0){ //recebe a propria jogada.
            puts("recv1 falhou");
            break;
        }

        if(recv(sock , JogadaAdv , SERVER_REPLY_MSG, 0) < 0){ // Recebe jogada do adversario.
            puts("recv1 falhou");
            break;
        }

        /*resposta do resutado da rodada. se ganhou ou perdeu*/
        if(recv(sock , resultado_parcial , SERVER_REPLY_MSG , 0) < 0){
            puts("recv2 falhou");
            return 1;
        }
        /*incrementa placar */
        if(strcmp(resultado_parcial, GANHOU)==0){
            player.pontosVc++;
        }else if(strcmp(resultado_parcial, PERDEU)==0){
            player.pontosAdv++;
        }
        system("clear");
        printf(" %dª RODADA\n\n", count);
        printf(" Você Jogou: %s \n %s Jogou: %s", JogadaVc, player.nick_adv, JogadaAdv);
        printf("\t\t Você %s\n", resultado_parcial);

        recv(sock, FLAG, 2, 0);
        if(strcmp(FLAG, T)==0){
          flag = 0;
        }
        count++;
    }//while(flag)

    recv(sock , server_reply , 50 , 0);
    system("clear");
    printf("\n RESULTADO DA ULTIMA RODADA\n\n", count);
    printf(" Você Jogou: %s \n %s Jogou: %s\n", JogadaVc, player.nick_adv, JogadaAdv);

    printf("\n\n************************************ PLACAR FINAL *********************************\n\n\n" );

    printf("\t\t\t\t Você: %d  |  %s: %d\n\n", player.pontosVc, player.nick_adv, player.pontosAdv);
    printf(" %s", server_reply);
    printf("\n\n\n\n\n");


    close(sock);
    return 0;
}
