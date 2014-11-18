/*
 * Projet: Reseau de capteur
 *
 * Rôle: Gestion des liaisons series
 * Version: 4.0
 */
#ifndef __UART_H__
#define __UART_H__

#include <stm32f10x_lib.h>

/* 
 * Definition des identifiants de liaison serie et LCD
 */
#define UART_1_ID 1
#define UART_2_ID 2
#define UART_3_ID 3 
#define LCD_ID 4 

/* 
 * Definition pour UARTBufferState 
 */
enum 
{
	EMPTY=0,
	NOT_EMPTY
};

/*
 * Variables exportées (publiques)
 */
extern int UART_RS606TransmissionFinished;

/* Fichiers identifiant les UARTS et l'ecran (pour printf et scanf) */
extern FILE UART_1;
extern FILE UART_2;
extern FILE UART_3;

extern FILE LCD_FILE;
/*
 * Fonctions exportées (publiques)
 */
void UARTInit(void);
int UARTSendChar (int uart, int c);
int UARTGetChar (int uart);
int UARTBufferState (FILE *f);
void UARTDesactivateRXRS606(void);
void UARTActivateRXRS606(void);
void USARTFlushBuffer(void);

#endif /* __UART_H__ */
