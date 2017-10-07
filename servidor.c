/* SERVIDOR */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "servidor.h"

#define SERVER_PORT 1234
#define MAX_CLIENT 2
#define MAX_SIZE_C 20
#define MAX_SIZE_NICK 10
#define TRUE 1

int main(int argc, char *argv[])
{
    int sockets, client_sock[MAX_CLIENT], c, read_size, i, erro;
    struct sockaddr_in server, client;
    char CHARjogadaPlayer1[MAX_SIZE_C], CHARjogadaPlayer2[MAX_SIZE_C];
    int jogadaPlayer1, jogadaPlayer2;
    int rodadas=1, vencedorPartida=-1;

    PLACAR Placar;
    PLAYER player[MAX_CLIENT];



      fd_set ativo_fd_set, le_fd_set;

      system("clear");
      /* cria o socket */
      sockets = socket(AF_INET, SOCK_STREAM, 0);   // SOCK_STREAM: TCP
      if (sockets == -1){
          printf("Não foi possivel criar socket");
          return 1;
      }//puts("Socket Criado");

      /* Preenche os campos da estrutura tipo struct sockaddr_in */
      server.sin_family = AF_INET; /* Selecona protocolo IPv4 */
      server.sin_addr.s_addr = INADDR_ANY; /* Aceita conexões vindas de qualquer IP */
      server.sin_port = htons(SERVER_PORT);	/* Numero da porta de serviço	*/

      /* bind: Liga um socket a uma porta de servico */
      if(bind(sockets,(struct sockaddr *)&server, sizeof(server)) < 0){
          perror("Erro: bind Falhou!");
          return 1;
      }//puts("bind done");

      puts(" SERVIDOR ONLINE \n");

      listen(sockets, 2);

      /* Aceita a conexao */
      puts(" Aguardando Jogadores...");
      c = sizeof(struct sockaddr_in);

      FD_ZERO(&ativo_fd_set);
      FD_SET(sockets, &ativo_fd_set);

      while (1)
      {
          /* A funcao select espera que algo aconteca nos sockets */
          le_fd_set=ativo_fd_set;
          erro=select(FD_SETSIZE, &le_fd_set, NULL, NULL, NULL);
          if(erro < 0){
              puts("select falhou!");
          }

          if(FD_ISSET(sockets, &le_fd_set))
          {
              /*inicializando o placar*/
              Placar.pontosPlayer1 = 0;
              Placar.pontosPlayer2 = 0;

              /*Cadastro do player 1*/
              client_sock[0] = accept(sockets, (struct sockaddr *)&client, (socklen_t*)&c);
              if (client_sock[0] < 0){
                  perror("accept 0 Falhou!");
                  return 1;
              }
              if(recv(client_sock[0], player[0].nickname, MAX_SIZE_NICK, 0) < 0){
                  perror("Falha no cadastro 1");
                  return 1;
              }
              printf(" %s foi conectado\n\n", player[0].nickname);

              /*cadastro do player 2*/
              client_sock[1] = accept(sockets, (struct sockaddr *)&client, (socklen_t*)&c);
              if (client_sock[1] < 0){
                 perror("accept 1 Falhou!");
                 return 1;
              }
              if(recv(client_sock[1], player[1].nickname, MAX_SIZE_NICK, 0) < 0){
                  perror("Falha no cadastro 2");  return 1;
              }
              printf(" %s foi conectado\n", player[1].nickname);

              system("clear"); // limpar tela do terminal.
              puts("\n JOGADORES ONLINE\n");
              for (i = 0; i < MAX_CLIENT; i++){
                  printf(" %dº player %d: %s\n", (i+1), (i+1), player[i].nickname);
              }
              /*envia o nome dos adversarios para os clientes*/
              send(client_sock[0], player[1].nickname, MAX_SIZE_C, 0);
              send(client_sock[1], player[0].nickname, MAX_SIZE_C, 0);

              /**Recebe a menssagem do cliente**/
              read_size = 2; // força a entrada no while(read_size > 0)
              while(read_size > 0)
              {
                  /* recebe a jogada do cliente_1 */
                  read_size = recv(client_sock[0], CHARjogadaPlayer1, MAX_SIZE_C, 0);
                  /* envia de volta a jogada ao cliente_1 */
                  jogadaPlayer1 = atoi(CHARjogadaPlayer1);
                  enviaMensagemSwitch(jogadaPlayer1, client_sock[0], MAX_SIZE_C);

                  //printf("%d\n", jogadaPlayer1);
                  switch (jogadaPlayer1){
                      case 1: puts(" Player 1 PEDRA"); break;
                      case 2: puts(" Player 1 PAPEL"); break;
                      case 3: puts(" Player 1 TESOURA"); break;
                  }
                  if(read_size == 0){
                      puts(" Cliente 1 Desconectado");
                  }
                  if(read_size == -1){
                      perror("recv1 falhou");
                  }

                  /* recebe a jogada do cliente_2 */
                  read_size = recv(client_sock[1], CHARjogadaPlayer2, MAX_SIZE_C, 0);
                  /* envia de volta a jogada ao cliente_2*/
                  jogadaPlayer2 = atoi(CHARjogadaPlayer2);
                  enviaMensagemSwitch(jogadaPlayer2, client_sock[1], MAX_SIZE_C);

                  //printf("%d\n", jogadaPlayer2);
                  switch (jogadaPlayer2){
                      case 1: puts(" Player 2 PEDRA"); break;
                      case 2: puts(" Player 2 PAPEL"); break;
                      case 3: puts(" Player 2 TESOURA"); break;
                  }
                  if(read_size == 0){
                      puts(" Cliente 2 Desconectado");
                  }
                  if(read_size == -1){
                      perror("recv2 falhou");
                  }
                  /*envia a jogada do adversario aos clientes*/
                  enviaMensagemSwitch(jogadaPlayer1, client_sock[1], MAX_SIZE_C);
                  enviaMensagemSwitch(jogadaPlayer2, client_sock[0], MAX_SIZE_C);


                  /*Parte do jogo*/
                  jogadaPlayer1 = atoi(CHARjogadaPlayer1);
                  jogadaPlayer2 = atoi(CHARjogadaPlayer2);
                  if (jogadaPlayer1 == jogadaPlayer2){
                      printf(" Resultado: EMPATE\n");
                      send(client_sock[0], "EMPATOU", MAX_SIZE_C, 0);
                      send(client_sock[1], "EMPATEOU", MAX_SIZE_C, 0);
                  }else{
                        /*  pedra                 tesoura                 papel              pedra                 tesoura            papel*/
                      if(jogadaPlayer1== 1 && jogadaPlayer2 ==3 || jogadaPlayer1== 2 && jogadaPlayer2 ==1 || jogadaPlayer1== 3 && jogadaPlayer2 ==2){
                          Placar.pontosPlayer1++; // incrementa placar Player 1

                          send(client_sock[0], "Ganhou a Rodada", MAX_SIZE_C, 0);
                          send(client_sock[1], "Perdeu a Rodada", MAX_SIZE_C, 0);
                      }else{
                          Placar.pontosPlayer2++; // incrementa placar player 2

                          send(client_sock[0], "Perdeu a Rodada", MAX_SIZE_C, 0);
                          send(client_sock[1], "Ganhou a Rodada", MAX_SIZE_C, 0);
                      }
                  }

                  printf("\n*******PLACAR PARCIAL*******\n");
                  printf("%s: %d  |  %s: %d\n\n", player[0].nickname, Placar.pontosPlayer1, player[1].nickname, Placar.pontosPlayer2);

                  /*Se placar igual a 3 notificar cliente*/
                  if(Placar.pontosPlayer1==3 || Placar.pontosPlayer2==3){
                      // t: true -> pontuação atingiu os 3 pontos
                      send(client_sock[0], "t", 2, 0);
                      send(client_sock[1], "t", 2, 0);
                  }else{
                      //f: false -> pontuação não atingiu os 3 pontos
                      send(client_sock[0], "f", 2, 0);
                      send(client_sock[1], "f", 2, 0);
                  }
                  if(Placar.pontosPlayer1==3 || Placar.pontosPlayer2==3){
                      system("clear");
                      printf("\n************************************ PLACAR FINAL *********************************\n\n");
                      printf("\t\t\t\t %s: %d  |  %s: %d\n\n", player[0].nickname, Placar.pontosPlayer1, player[1].nickname, Placar.pontosPlayer2);

                      if(Placar.pontosPlayer1 > Placar.pontosPlayer2) {
                          printf("\n %s GANHOU \n", player[0].nickname);
                          send(client_sock[0], "VOCÊ GANHOU PARABÉNS!! :) ", 40, 0);
                          send(client_sock[1], "VOCÊ PERDEU MAIS SORTE NA PROXIMA!! :(", 50, 0);
                      }else{
                          printf("\n %s GANHOU \n", player[1].nickname);
                          send(client_sock[1], "VOCÊ GANHOU PARABÉNS!! :) ", 40, 0);
                          send(client_sock[0], "VOCÊ PERDEU MAIS SORTE NA PROXIMA!! :(", 50, 0);
                      }
                      //encerra a comunicação
                      close(client_sock[0]);
                      close(client_sock[1]);
                      puts("--------------------------------------- FIM ---------------------------------------\n\n:)\n\n");
                      read_size = -1;
                      puts("Aguardando no jogadores...");
                  }
              }//while(read_size > 0)
          }//if(FD_ISSET(sockets, &le_fd_set))
      }//while(1)

    //encerra a comunicação
    close(client_sock[0]);
    close(client_sock[1]);

    return 0;
}

void enviaMensagemSwitch(int jogada, int client_sock, int MAX_TAM)
{
  switch (jogada){
      case 1: send(client_sock, "PEDRA", MAX_TAM, 0);
          break;
      case 2: send(client_sock, "PAPEL", MAX_TAM, 0);
          break;
      case 3: send(client_sock, "TESOURA", MAX_TAM, 0);
          break;
  }
}
