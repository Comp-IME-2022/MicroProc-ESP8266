HANDLE handle_serial = NULL;
COMMTIMEOUTS timeoutespera;
DCB DCBrs232win;


BOOL inicializa_serial(char *porta, BYTE ByteSize, BYTE StopBits, BYTE Parity, int BaudRate, int timeout){
    timeoutespera.ReadIntervalTimeout = timeout;
    timeoutespera.ReadTotalTimeoutMultiplier = timeout;
    timeoutespera.ReadTotalTimeoutConstant = 0;
    timeoutespera.WriteTotalTimeoutMultiplier = timeout;
    timeoutespera.WriteTotalTimeoutConstant = 0;
    CloseHandle(handle_serial);
    handle_serial = INVALID_HANDLE_VALUE;
    handle_serial = CreateFile(porta, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(handle_serial == INVALID_HANDLE_VALUE){
        printf("porta errada otario\n");
        return(FALSE);
    }

    SetCommMask(handle_serial, EV_RXCHAR|EV_TXEMPTY|EV_ERR);
    SetCommTimeouts(handle_serial, &timeoutespera);
    GetCommState(handle_serial, &DCBrs232win);
    DCBrs232win.BaudRate;
    DCBrs232win.ByteSize = ByteSize;
    DCBrs232win.Parity = Parity;
    DCBrs232win.StopBits = StopBits;

    if(SetCommState(handle_serial, &DCBrs232win)){
        return(TRUE);
    }else{
        return(FALSE);
    }
}

void libera_serial(void){
    CloseHandle(handle_serial);
}

BOOL envia_caracter_serial(char *caracter){
    int checatransmissao;
    BOOL status;
    status = WriteFile(handle_serial, caracter, 1, (LPDWORD)(&checatransmissao), NULL);
    if((status == TRUE) && (checatransmissao == 1)){
        return(TRUE);
    }else{
        return(FALSE);
    }
}

BOOL recebe_caracter_serial(char *caracter){
    int checatransmissao;
    //printf("\ncheca transmissao: %d\n", checatransmissao);
    BOOL status;
    status = ReadFile(handle_serial, caracter, 1, (LPDWORD)(&checatransmissao) , NULL);
    if((status == TRUE) && (checatransmissao == 1)){
            //printf("\ndeu bom\n");
        return(TRUE);
    }else{
        return(FALSE);
    }
}

BOOL envia_caracter_serial_TXDE(char *caracter){
    int mascara_event;
    BOOL status;
    WaitCommEvent(handle_serial, (LPDWORD)(&mascara_event), NULL);
    if(mascara_event&EV_TXEMPTY){
        status = envia_caracter_serial(caracter);
        return(status);
    }else{
        return(FALSE);
    }
}

BOOL recebe_caracter_serial_RXDA(char *caracter){
    int mascara_event;
    BOOL status;
    WaitCommEvent(handle_serial, (LPDWORD)(&mascara_event), NULL);
    if(mascara_event&EV_RXCHAR){
        status = recebe_caracter_serial(caracter);
        return(status);
    }else{
        return(FALSE);
    }
}
