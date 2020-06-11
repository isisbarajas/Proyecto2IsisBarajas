#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_tpm.h"

#define NorteVerde 0u
#define NorteAmarillo 1u
#define NorteRojo 2u
#define NorteVuelta 3u
#define NorteBoton 2u
#define SurVerde 20u
#define SurAmarillo 21u
#define SurRojo 22u
#define SurVuelta 23u
#define SurBoton 29u
#define EsteVerde 5u
#define EsteAmarillo 4u
#define EsteRojo 3u
#define EsteVuelta 0u
#define EsteBoton 7u
#define OesteVerde 4u
#define OesteAmarillo 12u
#define OesteRojo 4u
#define OesteVuelta 2u
#define OesteBoton 1u

typedef struct {
	uint16_t TIMER_MOD;
	uint8_t NV;
	uint8_t NR;
	uint8_t NA;
	uint8_t NVuelta;
	uint8_t SV;
	uint8_t SA;
	uint8_t SR;
	uint8_t SVuelta;
	uint8_t EV;
	uint8_t EA;
	uint8_t ER;
	uint8_t EVuelta;
	uint8_t OV;
	uint8_t OA;
	uint8_t OR;
	uint8_t OVuelta;
}state;

int main(void) {

  	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	BOARD_InitDebugConsole();

    tpm_config_t config;
    TPM_GetDefaultConfig(&config);
	config.prescale= kTPM_Prescale_Divide_128;
	TPM_Init(TPM0, &config);
	TPM_Init(TPM1, &config);
	TPM_Init(TPM2, &config);


	state FSM[34];
	FSM[0]=(state){.NV= 0u, .NA=1u, .NR=0u, .NVuelta=0u, .SV = 0u, .SA = 1u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 1u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 1u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=2560u};	//Parpadeo Amarillo
	FSM[1]=(state ){.NV= 1u, .NA=0u, .NR=0u, .NVuelta=0u, .SV = 1u, .SA = 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1792u};	//Verde NS-SN y  Rojo EO-OE por 7 segundos
	FSM[2]=(state ){.NV= 1u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 1u, .SA = 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA= 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1792u}; 	//Verde sin vuelta NS-SN y Rojo (EO-OE) 7 segundos
	FSM[3]=(state ){.NV= 0u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA = 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u}; 						//Verde NS-SN y Rojo EO-OE
	FSM[4]=(state ){.NV= 0u, .NA=1u, .NR=0u, .NVuelta=0u, .SV = 0u, .SA = 1u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA= 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u}; 	//Amarillo NS-SN y Rojo EO-OE 2 segundos
	FSM[5]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 1u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV = 1u, .OA = 0u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=1792u};		//Rojo NS-SN y  Verde EO-OE 7 segundos
	FSM[6]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 1u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV = 1u, .OA = 0u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=1792u};		//Rojo NS-SN y  Verde sin vuelta EO-OE 7s
	FSM[7]=(state ){.NV= 0u, .NA= 0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA = 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 0u, .OVuelta=0u};						//Rojo NS-SN y Parpadeo Verde EO-OE
	FSM[8]=(state ){.NV= 0u, .NA= 0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 1u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 1u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=512u};		//Rojo NS-SN y Amarillo EO-OE 2s
	FSM[9]=(state ){.NV= 1u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u};						//Verde NS, Parpadeo Verde SN  Y Rojo EO-OE
	FSM[10]=(state ){.NV=1u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA= 1u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u};		//Verde NS  Amarillo SN y Rojo EO-OE 2s
	FSM[11]=(state ){.NV=1u, .NA=0u, .NR=0u, .NVuelta=1u, .SV= 0u, .SA= 0u, .SR= 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1024u};		//Verde y vuelta NS,  Rojo SN y Rojo EO-OE 4s
	FSM[12]=(state ){.NV=0u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA= 0u, .OR = 1u, .OVuelta=0u};						//Parpadeo Verde y vuelta NS,  Rojo SN  y Rojo EO-OE
	FSM[13]=(state ){.NV=0u, .NA=1u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u};		//Amarillo NS,  Rojo SN y Rojo EO-OE 2s
	FSM[14]=(state ){.NV=0u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 1u, .SA= 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u};						//Parpadeo Verde  NS,  Verde SN y Rojo EO-OE
	FSM[15]=(state ){.NV=0u, .NA=1u, .NR=0u, .NVuelta=0u, .SV= 1u, .SA= 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u};		//Amarillo NS,  Verde SN y Rojo EO-OE 2s
	FSM[16]=(state ){.NV=0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 1u, .SA= 0u, .SR = 0u, .SVuelta=1u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1024u};		//Rojo NS, Verde y Vuelta SN y Rojo EO-OE 4s
	FSM[17]=(state ){.NV=0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u};						//Rojo NS,  Verde y Parpadeo Vuelta SN y Rojo (EO-OE)
	FSM[18]=(state ){.NV=0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 1u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u};		//Rojo NS, Amarillo SN y Rojo EO-OE 2s
	FSM[19]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV= 1u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 0u, .OVuelta=0u};						//Rojo NS-SN, Verde EO y Parpadeo Verde OE
	FSM[20]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 1u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV= 0u, .OA = 1u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=512u};		//Rojo NS-SN, Verde EO y  Amarillo OE  2s
	FSM[21]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 1u, .EA = 0u, .ER = 0u, .EVuelta=1u, .OV= 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1024u};		//Rojo NS-SN, Verde y Vuelta EO y Rojo OE 4s
	FSM[22]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR= 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u};						//Rojo NS-SN, Verde y Vuelta Parpadeo EO y Rojo OE
	FSM[23]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR= 1u, .SVuelta=0u, .EV = 0u, .EA = 1u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA= 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=512u};		//Rojo NS-SN, Amarillo EO y Rojo OE 2s
	FSM[24]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 0u, .EVuelta=0u, .OV= 1u, .OA = 0u, .OR = 0u, .OVuelta=0u};						//Rojo NS-SN, Parpadeo Verde EO y Verde OE
	FSM[25]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 1u, .ER = 0u, .EVuelta=0u, .OV = 1u, .OA = 0u, .OR= 0u, .OVuelta=0u, .TIMER_MOD=512u}; 		//Rojo NS-SN, Amarillo EO y Verde OE  2s
	FSM[26]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 1u, .OA = 0u, .OR = 0u, .OVuelta=1u, .TIMER_MOD=1024u}; 	//Rojo NS-SN, Rojo EO y Verde y Vuelta OE 4s
	FSM[27]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA= 0u, .ER= 1u, .EVuelta=0u, .OV= 0u, .OA = 0u, .OR = 0u, .OVuelta=0u};					    	//Rojo NS-SN, Rojo EO y Verde y Vuelta Parpadeo OE
	FSM[28]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV= 0u, .OA = 1u, .OR = 0u, .OVuelta=0u, .TIMER_MOD=512u};		//Rojo NS-SN, Rojo EO y Amarillo OE  2s
	FSM[29]=(state ){.NV= 0u, .NA=0u, .NR=0u, .NVuelta=1u, .SV= 0u, .SA = 0u, .SR = 0u, .SVuelta=1u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u, .TIMER_MOD=1024u};	// Vuelta NS-SN y Rojo EO-OE 4s
	FSM[30]=(state ){.NV= 0u, .NA=0u, .NR=0u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 0u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 1u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 1u, .OVuelta=0u};						//Vuelta NS-SN y  Rojo EO-OE
	FSM[31]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA = 0u, .ER = 0u, .EVuelta=1u, .OV = 0u, .OA = 0u, .OR = 0u, .OVuelta=1u, .TIMER_MOD=1024u};		//Vuelta EO-OE y Rojo NS-SN 4s
	FSM[32]=(state ){.NV= 0u, .NA=0u, .NR=1u, .NVuelta=0u, .SV= 0u, .SA= 0u, .SR = 1u, .SVuelta=0u, .EV = 0u, .EA= 0u, .ER = 0u, .EVuelta=0u, .OV = 0u, .OA = 0u, .OR = 0u, .OVuelta=0u};						//Parpadeo Vuelta EO-OE y Rojo NS-SN

	uint8_t estado_reg=0;
	uint8_t estado=0;
	uint8_t Timer_init=0;
	uint32_t timerBandera;
	uint32_t mascara=1u<<8u;
	uint32_t mascara2=1u<<8u;
	uint32_t mascara_Off2= mascara2;
	uint32_t mascara_Off= mascara;
	uint8_t NSensor;
	uint8_t SSensor;
	uint8_t ESensor;
	uint8_t OSensor;
	uint8_t NSensor_reg=1;
	uint8_t SSensor_reg=1;
	uint8_t ESensor_reg=1;
	uint8_t OSensor_reg=1;
	uint8_t estadoactual=0;
	int contador = 0;

    while(1) {
    	GPIO_WritePinOutput(GPIOB, NorteVerde, FSM[estado].NV);
    	GPIO_WritePinOutput(GPIOB, NorteAmarillo, FSM[estado].NA);
    	GPIO_WritePinOutput(GPIOB, NorteRojo, FSM[estado].NR);
    	GPIO_WritePinOutput(GPIOB, NorteVuelta, FSM[estado].NVuelta);
    	GPIO_WritePinOutput(GPIOE, SurVerde, FSM[estado].SV);
    	GPIO_WritePinOutput(GPIOE, SurAmarillo, FSM[estado].SA);
    	GPIO_WritePinOutput(GPIOE, SurRojo, FSM[estado].SR);
    	GPIO_WritePinOutput(GPIOE, SurVuelta, FSM[estado].SVuelta);
    	GPIO_WritePinOutput(GPIOC, EsteVerde, FSM[estado].EV);
		GPIO_WritePinOutput(GPIOC, EsteAmarillo, FSM[estado].EA);
		GPIO_WritePinOutput(GPIOC, EsteRojo, FSM[estado].ER);
		GPIO_WritePinOutput(GPIOC, EsteVuelta, FSM[estado].EVuelta);
		GPIO_WritePinOutput(GPIOA, OesteVerde, FSM[estado].OV);
		GPIO_WritePinOutput(GPIOA, OesteAmarillo, FSM[estado].OA);
		GPIO_WritePinOutput(GPIOD, OesteRojo, FSM[estado].OR);
		GPIO_WritePinOutput(GPIOA, OesteVuelta, FSM[estado].OVuelta);

    	NSensor=GPIO_ReadPinInput(GPIOC, NorteBoton);
    	SSensor=GPIO_ReadPinInput(GPIOE, SurBoton);
    	ESensor=GPIO_ReadPinInput(GPIOC, EsteBoton);
    	OSensor=GPIO_ReadPinInput(GPIOA, OesteBoton);

    	if(NSensor == 0){
			NSensor_reg=0;
    	}
    	if(SSensor == 0){
    		SSensor_reg=0;
    	}
    	if(ESensor == 0){
    		ESensor_reg=0;
    	}
    	if(OSensor == 0){
    		OSensor_reg=0;
    	}
    	timerBandera=TPM_GetStatusFlags(TPM0);
     	switch(estado){
    	case 0:
    		do{
				TPM_SetTimerPeriod(TPM1, 256u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOB, 1u<<1u);
				GPIO_TogglePinsOutput(GPIOE, 1u<<21u);
				GPIO_TogglePinsOutput(GPIOC, 1u<<4u);
				GPIO_TogglePinsOutput(GPIOA, 1u<<12u);
				while(!(TPM1->STATUS & mascara)){
				}
				if(TPM1->STATUS & mascara){
					TPM1->STATUS &=mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
    		contador=0;
    		estadoactual=estado;
    		estado=1;
    	break;
    	case 1:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[1].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
 			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				if(NSensor_reg == 1 && SSensor_reg==1){
					estado_reg=0;
					NSensor_reg = 1;
					SSensor_reg = 1;
					estado = 2;
				}else if(NSensor_reg == 0 && SSensor_reg==1){
					NSensor_reg = 1;
					SSensor_reg = 1;
					estado = 9;
				}else if(NSensor_reg == 1 && SSensor_reg==0){
					NSensor_reg = 1;
					SSensor_reg = 1;
					estado = 14;
				}else if(NSensor_reg == 0 && SSensor_reg==0){
					estado_reg=1;
					NSensor_reg = 1;
					SSensor_reg = 1;
					estado = 3;
				}
			}
		break;
    	case 2:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[2].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado=3;
			}
		break;

    	case 3:
    		estadoactual=estado;
			do{
				TPM_SetTimerPeriod(TPM1, 64u); //Para el parpadeo sea de un cuarto de segundo
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOB, 1u<<0u);
				GPIO_TogglePinsOutput(GPIOE, 1u<<20u);
				while(!(TPM1->STATUS & mascara)){
				}

				if(TPM1->STATUS & mascara){
					TPM1->STATUS &=mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}

			}while(contador<=9);
			contador=0;
			estado=4;
    	break;

    	case 4:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[4].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				if(estado_reg==0){
					estado=5;
				}else if(estado_reg==1){
					estado=29;
				}
			}
		break;

    	case 5:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[5].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}

			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;

				if(ESensor_reg == 1 && OSensor_reg==1){
					estado_reg=0;
					ESensor_reg = 1;
					OSensor_reg = 1;
					estado = 6;
				}else if(ESensor_reg == 0 && OSensor_reg==1){
					ESensor_reg = 1;
					OSensor_reg = 1;
					estado = 19;
				}else if(ESensor_reg == 1 && OSensor_reg==0){
					ESensor_reg = 1;
					OSensor_reg = 1;
					estado = 24;
				}else if(OSensor_reg == 0 && ESensor_reg==0){
					ESensor_reg = 1;
					OSensor_reg = 1;
					estado_reg=1;
					estado = 7;
				}
			}
		break;

    	case 6:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[6].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado=7;
			}
		break;

    	case 7:
    		estadoactual=estado;
			do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOC, 1u<<5u);
				GPIO_TogglePinsOutput(GPIOA, 1u<<4u);
				while(!(TPM1->STATUS & mascara)){
				}

				if(TPM1->STATUS & mascara){
					TPM1->STATUS &=mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}

			}while(contador<=9);
			contador=0;
			estado=8;
    	break;

    	case 8:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[8].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<8u);
				Timer_init=0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				if(estado_reg==0){
					estado=1;
				}else if(estado_reg==1){
					estado=31;
				}
			}
    	break;
      	case 9:
    		estadoactual=estado;
    		do{
    			TPM_SetTimerPeriod(TPM1, 64u);
    			TPM_StartTimer(TPM1, kTPM_SystemClock);
    			GPIO_TogglePinsOutput(GPIOE, 1u<<20u);
    			while(!(TPM1->STATUS & mascara)){
    			}

    			if(TPM1->STATUS & mascara){
    				TPM1->STATUS &= mascara_Off;
    				contador=contador+1;
    				TPM_StopTimer(TPM1);
    				TPM1->CNT=0;
    			}
    		}while(contador<=9);
    		contador=0;
    		estado=10;
    	break;

    	case 10:
    		estadoactual=estado;
    		if(Timer_init==0){
    			Timer_init=1;
    			TPM_SetTimerPeriod(TPM0, FSM[10].TIMER_MOD);
    			TPM_StartTimer(TPM0, kTPM_SystemClock);
    		}

    		if(timerBandera){
    			TPM_ClearStatusFlags(TPM0, 1u<<8u);
    			Timer_init= 0;
    			TPM_StopTimer(TPM0);
    			TPM0->CNT=0;
    			estado= 11;
    		}
    	break;

    	case 11:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[11].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}

			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 12;
			}
    	break;

    	case 12:
    		estadoactual=estado;
    		do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOB, 1u<<0u);
				GPIO_TogglePinsOutput(GPIOB, 1u<<3u);
				while(!(TPM1->STATUS & mascara)){
				}

				if(TPM1->STATUS & mascara){
					TPM1->STATUS &= mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
			contador=0;
			estado=13;
		break;
    	case 13:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[13].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}

			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 5;
			}
    	break;
    	case 14:
    		estadoactual=estado;
    		do{
    			TPM_SetTimerPeriod(TPM1, 64u);
    			TPM_StartTimer(TPM1, kTPM_SystemClock);
    			GPIO_TogglePinsOutput(GPIOB, 1u<<0u);
    			while(!(TPM1->STATUS & mascara)){
    			}
    			if(TPM1->STATUS & mascara){
    				TPM1->STATUS &= mascara_Off;
    				contador=contador+1;
    				TPM_StopTimer(TPM1);
    				TPM1->CNT=0;
    			}
    		}while(contador<=9);
    		contador=0;
    		estado=15;
    	break;
    	case 15:
    		estadoactual=estado;
    		if(Timer_init==0){
    			Timer_init=1;
    			TPM_SetTimerPeriod(TPM0, FSM[15].TIMER_MOD);
    			TPM_StartTimer(TPM0, kTPM_SystemClock);
    		}

    		if(timerBandera){
    			TPM_ClearStatusFlags(TPM0, 1u<<8u);
    			Timer_init= 0;
    			TPM_StopTimer(TPM0);
    			TPM0->CNT=0;
    			estado= 16;
    		}
    	break;

    	case 16:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[11].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}

			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 17;
			}
    	break;
    	case 17:
    		estadoactual=estado;
    		do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOE, 1u<<20u);
				GPIO_TogglePinsOutput(GPIOE, 1u<<23u);
				while(!(TPM1->STATUS & mascara)){
				}

				if(TPM1->STATUS & mascara){
					TPM1->STATUS &= mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
			contador=0;
			estado=18;
		break;

    	case 18:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[18].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 5;
			}
    	break;
      	case 19:
    		estadoactual=estado;
    		do{
    			TPM_SetTimerPeriod(TPM1, 64u);
    			TPM_StartTimer(TPM1, kTPM_SystemClock);
    			GPIO_TogglePinsOutput(GPIOA, 1u<<4u);
    			while(!(TPM1->STATUS & mascara)){
    			}
    			if(TPM1->STATUS & mascara){
    				TPM1->STATUS &= mascara_Off;
    				contador=contador+1;
    				TPM_StopTimer(TPM1);
    				TPM1->CNT=0;
    			}
    		}while(contador<=9);
    		contador=0;
    		estado=20;
    	break;
    	case 20:
    		estadoactual=estado;
    		if(Timer_init==0){
    			Timer_init=1;
    			TPM_SetTimerPeriod(TPM0, FSM[20].TIMER_MOD);
    			TPM_StartTimer(TPM0, kTPM_SystemClock);
    		}
    		if(timerBandera){
    			TPM_ClearStatusFlags(TPM0, 1u<<8u);
    			Timer_init= 0;
    			TPM_StopTimer(TPM0);
    			TPM0->CNT=0;
    			estado= 21;
    		}
    	break;
    	case 21:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[21].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}

			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 22;
			}
    	break;
    	case 22:
    		estadoactual=estado;
    		do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOC, 1u<<5u);
				GPIO_TogglePinsOutput(GPIOC, 1u<<0u);
				while(!(TPM1->STATUS & mascara)){
				}
				if(TPM1->STATUS & mascara){
					TPM1->STATUS &= mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
			contador=0;
			estado=23;
		break;
    	case 23:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[23].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 1;
			}
    	break;
    	case 24:
    		estadoactual=estado;
    		do{
    			TPM_SetTimerPeriod(TPM1, 64u);
    			TPM_StartTimer(TPM1, kTPM_SystemClock);
    			GPIO_TogglePinsOutput(GPIOC, 1u<<5u);
    			while(!(TPM1->STATUS & mascara)){
    			}

    			if(TPM1->STATUS & mascara){
    				TPM1->STATUS &= mascara_Off;
    				contador=contador+1;
    				TPM_StopTimer(TPM1);
    				TPM1->CNT=0;
    			}
    		}while(contador<=9);
    		contador=0;
    		estado=25;
    	break;
    	case 25:
    		estadoactual=estado;
    		if(Timer_init==0){
    			Timer_init=1;
    			TPM_SetTimerPeriod(TPM0, FSM[25].TIMER_MOD);
    			TPM_StartTimer(TPM0, kTPM_SystemClock);
    		}
    		if(timerBandera){
    			TPM_ClearStatusFlags(TPM0, 1u<<8u);
    			Timer_init= 0;
    			TPM_StopTimer(TPM0);
    			TPM0->CNT=0;
    			estado= 26;
    		}
    	break;
    	case 26:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[16].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 27;
			}
    	break;
    	case 27:
    		estadoactual=estado;
    		do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOA, 1u<<4u);
				GPIO_TogglePinsOutput(GPIOA, 1u<<2u);
				while(!(TPM1->STATUS & mascara)){
				}
					if(TPM1->STATUS & mascara){
						TPM1->STATUS &= mascara_Off;
						contador=contador+1;
						TPM_StopTimer(TPM1);
						TPM1->CNT=0;
					}
			}while(contador<=9);
			contador=0;
			estado=28;
		break;
    	case 28:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[28].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 1;
			}
    	break;
    	case 29:
    		estadoactual=estado;
    		if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[29].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 30;
			}
    	break;
    	case 30:
    		estadoactual=estado;
    		do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOB, 1u<<3u);
				GPIO_TogglePinsOutput(GPIOE, 1u<<23u);
				while(!(TPM1->STATUS & mascara)){
				}
				if(TPM1->STATUS & mascara){
					TPM1->STATUS &= mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
			contador=0;
			estado_reg=0;
			estado=4;
    	break;
    	case 31:
    		estadoactual=estado;
			if(Timer_init==0){
				Timer_init=1;
				TPM_SetTimerPeriod(TPM0, FSM[31].TIMER_MOD);
				TPM_StartTimer(TPM0, kTPM_SystemClock);
			}
			if(timerBandera){
				TPM_ClearStatusFlags(TPM0, 1u<<8u);
				Timer_init= 0;
				TPM_StopTimer(TPM0);
				TPM0->CNT=0;
				estado= 32;
			}
		break;
		case 32:
			estadoactual=estado;
			do{
				TPM_SetTimerPeriod(TPM1, 64u);
				TPM_StartTimer(TPM1, kTPM_SystemClock);
				GPIO_TogglePinsOutput(GPIOC, 1u<<0u);
				GPIO_TogglePinsOutput(GPIOA, 1u<<2u);
				while(!(TPM1->STATUS & mascara)){
				}
				if(TPM1->STATUS & mascara){
					TPM1->STATUS &= mascara_Off;
					contador=contador+1;
					TPM_StopTimer(TPM1);
					TPM1->CNT=0;
				}
			}while(contador<=9);
			contador=0;
			estado_reg=0;
			estado=8;
		break;
    	}
    }
}
