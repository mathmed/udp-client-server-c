// Tipos dos pacotes
#define NSIP_REQ 0x0
#define NSIP_REP 0x1
#define NSIP_ERR 0x2

// Tipos das consultas
#define MACADDR 0x0 // Endereço MAC da placa de rede
#define RXPACKS 0x1 // Quantidade de pacotes recebidos
#define TXPACKS 0x2 // Quantidade de pacotes enviados
#define RXBYTES 0x3 // Quantidade de bytes recebidos
#define TXBYTES 0x4 // Quantidade de bytes recebidos
#define TCPPORT 0x5 // Quantidade de portas TCP ouvindo conexões
#define UDPPORT 0x6 // Quantidade de portas UDP ouvindo conexões
#define TCPLIST 0x7 // Lista das portas TCP ouvindo conexões
#define UDPLIST 0x8 // Lista das portas UDP ouvindo conexões

// Network Server Information Protocol header
struct nsip_header
{ 
    unsigned char id;        //  8 bits - Para identificar a consulta
    unsigned char type;      //  8 bits - Tipo do pacote (request/reply)
    unsigned short checksum; // 16 bits - Tipo da consulta, conforme tipos
    unsigned short query;    // 16 bits - Verificacao de erros
    char result[48];         // 48 bytes:
                             //   - Se request: deve ir em branco
                             //   - Se reply: contém o resultado da consulta
};
