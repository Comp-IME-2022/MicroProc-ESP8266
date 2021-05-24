#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uartapi.c"

#define BF_ZS 1000
#define MAX_PATH 100

char caracter;
char buffer[BF_ZS];

void limpar_buffer(){
    for(int i=0; i<BF_ZS; i++){
        buffer[i] = NULL;
    }
}

int ler_arquivo(FILE* fp, int n){
    int i, c;
    limpar_buffer();

    if(n>BF_ZS) n=BF_ZS;

    for(i=0; i<n; i++){
        if((c = getc(fp)) == EOF) break;
        buffer[i] = c;
    }
    return i;
}

void envia_string(char* string){
    char cr = 0x0A;
    int i, tamanho;
    char lf = 0x0D;
    tamanho = strlen(string);
    for (i = 0; i < tamanho; i++)
        while(!(envia_caracter_serial(&(string[i]))));
    while(!(envia_caracter_serial(&lf)));
    while(!(envia_caracter_serial(&cr)));
}

void envia_serial(char* string){
    int num = strlen(string);
    char size[10];
    char mensagem[100] = "AT+CIPSEND=0,";
    BOOL resp_init = FALSE;

    itoa(num, size, 10);

    strcat(mensagem, size);

    envia_string(mensagem);
    Sleep(300);

    envia_string(string);
    Sleep(300);
}

void wait_connection(char* resp) {
    BOOL connection = FALSE;
    while(recebe_caracter_serial(&caracter)){
        if (caracter == 'C'){
            connection = TRUE;
        }
        if (caracter == 'T' && connection){
            envia_serial(resp);
            printf("Cliente conectado\n");
            break;
        }
    }
}

int main()
{
    int c;
    FILE *file;
    char porta_serial[] = "COM3";
    char titulo[] = "IMPORTADOR DE ARQUIVOS - V.1\n";
    char msg_erro[] = "Ocorreu um erro ao abrir o arquivo!";
    char msg_ok[] = "                     ARQUIVO\n==================================================\n";

    int idx=0;
    char path[MAX_PATH] = "";


    if(inicializa_serial(porta_serial, 8, ONESTOPBIT, NOPARITY, 115200, 600000)){
        //Espera conexão do cliente
        printf("%sAguardando conexao do cliente...\n", titulo);
        wait_connection(titulo);

        //Envia interface de interação
        envia_serial("Escreva o nome do arquivo:\n> ");

        //Lê caracter serial até encontrar o delimitador ':'
        while(recebe_caracter_serial(&caracter)){
            if (caracter == ':') break;
        }

        //Lê caminho do arquivo até encontrar o caracter '\n'
        while(recebe_caracter_serial(&caracter)){
            if(caracter == '\n') break;
            path[idx] = caracter;
            idx++;
        }

        printf("Path: %s\n", path);

        //Tenta abrir o arquivo
        if ((file = fopen(path,"r")) == NULL){
           printf("%s\n", msg_erro);
           envia_serial(msg_erro);
        }else{
            //Lê o arquivo e envia para o cliente em lotes
            int tamanho_leitura;

            printf("%s", msg_ok);
            envia_serial(msg_ok);

            do{
                tamanho_leitura = ler_arquivo(file, BF_ZS);
                printf("%s", buffer);
                envia_serial(buffer);
            }while(tamanho_leitura == BF_ZS);


            fclose(file);
        }
    }



    return 0;
}
