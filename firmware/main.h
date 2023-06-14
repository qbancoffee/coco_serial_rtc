#include <16F19155.h>
#device ADC=12

#FUSES VBATEN                   //VBAT functionality is enabled
#FUSES PUT_1MS                  //Power-Up Timer set to 1ms
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOWDT
//#FUSES RSTOSC_SOSC
#FUSES PPS1WAY
#FUSES CLKOUT
#FUSES LVP
#FUSES CKS
#FUSES SAF
#FUSES NOWRTC
#use delay(internal=8000000)


