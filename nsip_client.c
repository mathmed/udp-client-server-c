#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nsip.h"

short make_checksum(struct nsip_header *p)
{
    // 32 bits
    int sum = 0;
    int i;

    for (i = 0; i < 54; i++)
    {
        //printf("> %x\n",*((unsigned char *)p+i));
        sum += *((unsigned char *)p+i);
    }
    
    sum = sum % 65536; // 2^16

    //printf("sum: %d\n",sum);
    //printf("sum: %x\n",sum);

    return sum;
}

char get_opt()
{
    char opt;

    printf("Informe a opcao a consultar:\n");
    printf("\tMACADDR 0\n");
    printf("\tRXPACKS 1\n");
    printf("\tTXPACKS 2\n");
    printf("\tRXBYTES 3\n");
    printf("\tTXBYTES 4\n");
    printf("\tTCPPORT 5\n");
    printf("\tUDPPORT 6\n");
    printf("\tTCPLIST 7\n");
    printf("\tUDPLIST 8\n");
    printf("Opcao: ");

    scanf("%c",&opt);

    // para zerar
    return (opt - 48);
}

int main(int argc, char *argv[])
{
    struct nsip_header p;

    // descritor do socket do servidor
    int meusocket;

    // recebe o tamanho da estrutura sockaddr_in
    int sockettamanho;

    // numero de bytes recebidos pelo socket
    int numbytes;

    // informacoes do servidor
    struct sockaddr_in destinatario;

    // armazena checksum recebido
    short tmp_sum;

    if (argc != 3) 
    {
		printf("Erro: uso correto: ./cliente_udp ipservidor porta\n");
		return 1;
	}

    // tamanho da estrutura de endereco do socket usado
    sockettamanho = sizeof(struct sockaddr_in);

    // criando o socket
    meusocket = socket(AF_INET, SOCK_DGRAM, 0);

    destinatario.sin_family = AF_INET;

    // ip do servidor - 127.0.0.1 se estiver rodando na sua mesma maquina
    destinatario.sin_addr.s_addr = inet_addr(argv[1]);

    // porta do servidor
    destinatario.sin_port = htons(atoi(argv[2]));
    
    // zerando o resto da estrutura
    memset(&(destinatario.sin_zero), '\0', sizeof(destinatario.sin_zero));
    
    // montando o pacote para envio
    p.id = 0xab;
    p.type = NSIP_REQ;
    p.checksum = 0;
  
    p.query = get_opt();//UDPLIST;//UDPPORT;//MACADDR;

    memset(p.result, '\0', 48);

    // calcular checksum
    p.checksum = make_checksum(&p);

    numbytes = sendto(meusocket, &p, sizeof(p), 0,
			 (struct sockaddr *)&destinatario, sockettamanho);

    printf("Cliente: enviou %d bytes para %s:%s\n", 
            numbytes, argv[1], argv[2]);

    numbytes = recvfrom(meusocket, &p, sizeof(p) , 0,
		    (struct sockaddr *)&destinatario, &sockettamanho);
    
    printf("Cliente: recebeu %d bytes\n", numbytes);
	    printf("\tid: %x\n",p.id);
	    printf("\ttype: %x\n",p.type);
	    printf("\tchecksum: %x\n",p.checksum);
	    printf("\tquery: %x\n",p.query);
	    printf("\tresult: %s\n",p.result);

    tmp_sum = p.checksum;
    p.checksum = 0;

    if (tmp_sum == make_checksum(&p))
    {
        printf("checksum ok\n");
    }
    else
    {
        printf("checksum errado\n");
    }

	close(meusocket);

    return 0;
}
