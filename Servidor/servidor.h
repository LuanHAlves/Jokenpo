/* SERVER HEADER */

#define MAX_SIZE_NICK 10
typedef struct {
  int pontosPlayer1;
  int pontosPlayer2;
} PLACAR;

typedef struct{
  char nickname [MAX_SIZE_NICK];
} PLAYER;

void enviaMensagemSwitch(int jogada, int client_socket, int MAX_TAM);
void jokenpo (int jogadaPlayer1, int jogadaPlayer2, int client_sock0, int client_sock1, int MAX_SIZE_C, PLACAR Placar );
