/* 
    * Algoritmo para criação de um servidor UDP que aceita requisições de um cliente
    * Desenvolvido por Mateus Medeiros
    * Última atualização em 22/09/2019
*/

/* Importações de bibliotecas necessárias */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nsip.h"

/* Constantes auxiliares */
#define PORT 2102
#define MAXBUF 4000

/* Função para realizar o checksum */
short make_checksum(struct nsip_header *p){
    
    int sum = 0; int i;

    for (i = 0; i < 54; i++)
        sum += *((unsigned char *)p+i);
    
    sum = sum % 65536;

    return sum;
}

/* Função para executar um comando no CMD e guardar o resultado */
void get_output(char *cmd, char *buf){
    
    /* Descritor para o arquivo pipe */
    FILE *fp;

    /* Abrindo o 'comando' para leitura */
    fp = popen(cmd, "r");

    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit;
    }
    
    char tmpbuf[MAXBUF];

    /* ler as linhas da saida do comando e
        armazena-as no buffer temporario */

    while (fgets(tmpbuf, MAXBUF-1, fp) != NULL){

        /* concatena o resultado em buf */
        strncat(buf, tmpbuf, MAXBUF);
    }

    /* fecha o pipe */
    pclose(fp);
}

int main(){

    /* Criando estrutura do cabeçário do protocolo */
    struct nsip_header cabecario;

    /* Variável para guardar o tamanho do socket */
    int sockettamanho;

    /* Variável para criação do socket */
    int sockett;

    /* Estruturas dos sockets cliente/servidor */
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;

    /* Guardando o tamanho do socket */
    sockettamanho = sizeof(struct sockaddr_in);

    /* Criando o socket */
    sockett = socket(AF_INET, SOCK_DGRAM, 0); 

    memset(&(servidor.sin_zero), '\0', sizeof(servidor.sin_zero));
    memset(&(cliente.sin_zero), '\0', sizeof(cliente.sin_zero));

    /* Definindo campos do servidor como endereços IP aceitos, porta utilizada ... */
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(PORT);

    bind(sockett, (struct sockaddr *)&servidor, sizeof(servidor));

    /* Variável auxiliar para controle das conexões */
    int n;

    /* Printando mensagem de iniciação */
    
    printf("\n\n");
    printf("| **************************************************************************** |");
    printf("\n");
    printf("|_________________________ SERVIDOR UDP INICIADO ______________________________|");
    printf("\n");
    printf("|__________ Ouvindo requisições realizadas localmente (eg: 127.0.0.1) _________|");
    printf("\n");
    printf("|___________________ Servidor rodando na porta [2102] _________________________|");
    printf("\n");
    printf("|_______________ Verificando informações na placa de rede [wlp3s0] ____________|");
    printf("\n");
    printf("|                                                                              |\n");
    printf("| **************************************************************************** |\n");
    printf("\n\n\n");


    /* Laço infinito para o servidor está sempre ouvindo novas requisições */
    while(1){

        /* Recebendo uma requisição */
        n = recvfrom(sockett, &cabecario, sizeof(cabecario),  0, ( struct sockaddr *)&cliente, &sockettamanho);

        /* Exibindo dados da requisição recebida */
            
        printf("-- O Servidor acaba de receber uma requisição de %d bytes, segue o cabeçário: --\n", n);
	    printf("\t-> id: %x\n",cabecario.id);
	    printf("\t-> type: %x\n",cabecario.type);
	    printf("\t-> checksum: %x\n",cabecario.checksum);
	    printf("\t-> query: %x\n",cabecario.query);
	    printf("\t-> result: %s\n",cabecario.result);

        printf("\n\n-- Iniciando verificação do CHECKSUM ... --\n\n");

        /* Iniciando verificação do checksum */
        int tmp_sum = cabecario.checksum;
        cabecario.checksum = 0;

        if (tmp_sum == make_checksum(&cabecario)){
            
            printf("\n\nCHECKSUM OK! Iniciando verificação da QUERY requisitada ... \n\n");

            /* Criando buffer para guardar qual comando necessário para se executar de acordo com
                a query que foi enviada na requisição */
            
            char comando[MAXBUF];

            /* Estrutura condicional para verificar qual query foi pedida */

            if(cabecario.query == 0)
                strcpy(comando, "/sbin/ifconfig wlp3s0 | grep ether | cut --delimiter=' ' --fields=10");


            else if (cabecario.query == 1)
                strcpy(comando, "/sbin/ifconfig wlp3s0 | grep RX | grep packets | cut --delimiter=' ' --fields=11");
                

            else if (cabecario.query == 2)
                strcpy(comando, "/sbin/ifconfig wlp3s0 | grep TX | grep packets | cut --delimiter=' ' --fields=11");
                

            else if (cabecario.query == 3)
                strcpy(comando, "/sbin/ifconfig wlp3s0 | grep RX | grep packets | cut --delimiter=' ' --fields=14");
                

            else if (cabecario.query == 4)
                strcpy(comando, "/sbin/ifconfig wlp3s0 | grep TX | grep packets | cut --delimiter=' ' --fields=14");
                

            else if (cabecario.query == 5)
                strcpy(comando, "netstat -atn4 | grep 'OUÇA\\|LISTEN' -c");
                

            else if (cabecario.query == 6)
                strcpy(comando, "netstat -aun4 | grep udp -c");

            else if (cabecario.query == 7)
                strcpy(comando, "netstat -atn4 | grep 'OUÇA\\|LISTEN' | sed 's/ \\+/:/g' | cut -d ':' -f 5 | tr '\n' ',' | rev | cut -c 2- | rev");
                
            else if (cabecario.query == 8)
                strcpy(comando, "netstat -aun4 | grep udp | sed 's/ \\+/:/g' | cut -d ':' -f 5 | tr '\n' ',' | rev | cut -c 2- | rev");


            else printf("\n\n-- Não foi possível identificar a QUERY requisitada. -- \n\n"); 


            /* Alterando o tipo do pacote para resposta */
            cabecario.type = NSIP_REP;

            /* Guardando o resultado da requisição */
            get_output(comando, cabecario.result);

            /* Criando o novo valor do checksum */
            cabecario.checksum = make_checksum(&cabecario);

            /* Enviando novamente a estrutura para o cliente */
            n = sendto(sockett, &cabecario, sizeof(cabecario), 0, (const struct sockaddr *) &cliente, sockettamanho); 

            printf("-- O Servidor acaba responder ao cliente com %d bytes, segue o cabeçário: --\n", n);
            printf("\t-> id: %x\n",cabecario.id);
            printf("\t-> type: %x\n",cabecario.type);
            printf("\t-> checksum: %x\n",cabecario.checksum);
            printf("\t-> query: %x\n",cabecario.query);
            printf("\t-> result: %s\n",cabecario.result);
        
        } else {
            
            printf("\n\nO servidor recebeu uma requisição, mas o pacote foi corrompido. \n\n");

            /* Adicionando o tipo de erro */
            cabecario.type = NSIP_ERR;
            
            /* Enviando para o cliente */
            n = sendto(sockett, &cabecario, sizeof(cabecario), 0, (const struct sockaddr *) &cliente, sockettamanho);

        }

    }

    close(sockett);

    return 0;

}

