//*************************************
//* <16F19155.h> processor
//* Date: 3/27/2023
//* Ver. 0.9
//* by M D Siegel
//*
//* Copyright 2023  M D SIEGEL
//* All Rights Reserved
//* Last updated 3/31/2023
//**************************************
  #include <main.h>
  #include <ctype.h>
  #include <stdio.h>
  #include <stddef.h>
  #include <stdlib.h>
  #include <string.h>

   //RTCC
  rtc_time_t wizardTempTime;
////////////////////////////////////////
// Pic 16F19155 Picdem 2 plus
// B1       LED
// B2       SELA
// B3       SELB
// C7       RX
// C6       TX
// B6       PGC
// B7       PGD
// E3       MCLR
// C0-C1    Crystal
///////////////////// Global Vars

static int cntdown;


char xstring1[20];
char xstring2[5];
//char xstring3[5];
char xstring4[7];
char pls[2] ="+";
char slp[6] ="SLEEP";
const char paramname[] [6]= {"VER","SDATE","STIME","RDT","MODE","PEEK","POKE","HELP",""""};

static int parsenum; //result of compair
static int parsetype; // operator
static int16 parseval; // val to set
static int split[8];
static char *pbuffer;
char rbuff[8];
static char compbuff[20];
static int buflen;
static char rs232buff[81];
static char rsreadbuff[81];
static int rsptr;
static int rtflag;
static int blenq;
static int16 aboo; //debug
char temp;

STATIC int16 ADC_BT;     // battery

///////////////////////Defines
#define LED PIN_B1   //LED
#define SELA PIN_B2
#define SELB PIN_B3
#define RX PIN_C7
#define TX PIN_C6

#define parseitm 9

#pin_select U1RX=PIN_C7

#use rs232(baud=1200,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=DATA)

////////////// Functions
void copyright(void);
void  RTC_isr(void);
void  RDA_isr(void);
void  TIMER0_isr(void);

int read232(void);   // RS232
int is_rs232(void);
void clrprbuff(void);
void clrrsbuff(void);
int parseit(char * buf1,int clen);
char * strpiece(int pieceNum,int clen, char *buf1);
int compit(char *buf1, char *buf2, int clen);
int process(void);
void set_clock(void);
void read_clock(void);
void chan_sel(int chanx);
void updateADC();
void xsetDate();
void xsetTime();
void qsleep();
void xpoke();

////////////// code
#INT_TIMER0
void  TIMER0_isr(void) 
{
cntdown--;
   
   if(cntdown <= 0)
   { output_toggle(LED);
      cntdown = 60;
   }
    clear_interrupt(int_timer0);
}

#INT_RDA
void  RDA_isr(void) 
{
//! temp = fgetc(DATA);       //get char
//!       printf("*");
//!       rs232buff[rsptr++]=temp;  //inc buffer
//!       if (rsptr >= 79)          //max buffer
//!         {
//!            rsptr = 79;
//!         }
//!       if((temp == 0x0D))
//!       {
//!         temp = 0x0A;
//!         
//!       }
//!      
//!       if((temp == 0x0A))        //cr
//!         {
//!         
//!            if(rsptr >= 3)       //valid buffer process
//!            {
//!               rtflag = 1;
//!            }
//!            else
//!            {
//!               rsptr = 0;       //invalid buffer restart 
//!            }
//!          }
//!      
//! clear_interrupt(int_rda);
}

#INT_RTC
void  RTC_isr(void) 
{

}

void main()
{
    int16 wtmp, i, cmdsta;
    
   setup_adc_ports(0x3A);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64|RTCC_8_BIT);      //2.0 ms overflow
   
   //RTCC
   setup_rtc(RTC_ENABLE|RTC_CLOCK_SOSC,0);
   enable_interrupts(INT_TIMER0);
   //enable_interrupts(INT_RDA);
   enable_interrupts(INT_RTC);
   enable_interrupts(GLOBAL);
   
   
   chan_sel(0);
   cntdown = 60;
   
   printf(" ***\n\r");
   printf("Coco RTC Command Processor Active \n\r");
   
   while(TRUE)
   {
      
      if(kbhit())
      {
      temp = fgetc(DATA);       //get char
       rs232buff[rsptr++]=temp;  //inc buffer
       if (rsptr >= 79)          //max buffer
         {
            rsptr = 79;
         }
       if((temp == 0x0D))
       {
         temp = 0x0A;
         
       }
      
       if((temp == 0x0A))        //cr
         {
         
            if(rsptr >= 3)       //valid buffer process
            {
               rtflag = 1;
            }
            else
            {
               rsptr = 0;       //invalid buffer restart 
            }
          }
      }
//////////////RS232 Process Terminal Loop
      if(is_rs232() == 1)
      {
       blenq = read232();
                           //printf("len %i buff %s \n\r",blenq,rs232buff );
      clrrsbuff();
        
      wtmp =  parseit(rsreadbuff, sizeof(rsreadbuff)); 
      pbuffer = strpiece(split[0], 20, rsreadbuff);   //Parse input
                           //printf("part1: %s \n\r",pbuffer);
     
     for(i=0;i<=parseitm;i++)                               //Compair to table
      { 
         sprintf(compbuff,"%s",paramname[i]);
         parsenum = strcmp(compbuff, pbuffer);
         if (parsenum==0)
         {
                           //printf("parse # %i \n\r",i);
            parsenum = i;
            break;
         }
         if(i == 19)
         {
         parsenum = i;
         }
                        //printf("parse # %i \n\r",i); 
      }
      ///////////////////
      pbuffer = 35;
      pbuffer = strpiece(split[1], 20, rsreadbuff);   //Parse function type
      if(*pbuffer == 35) //#
         {
         parsetype = 0;
         }
      if(*pbuffer == 63) //? return return
         {
         parsetype = 1;
         }
      if(*pbuffer == 61) //= set var
         {
         parsetype = 2;
         }
                        //printf("part2: %s \n\r",pbuffer);
      strcpy(xstring1,pbuffer);
                        //printf("parse type %i \n\r",parsetype);
      
      pbuffer = 0;
      pbuffer = strpiece(split[2], 20, rsreadbuff);  //Parse Parm
                     //parseval = atoi(pbuffer);
      
      if(parsetype == 2)
      {
     
      pbuffer = strpiece(split[2], 20, rsreadbuff);
      
                    //printf("str buff: %s \n\r",pbuffer);
      strcpy(rbuff,pbuffer);
      parseval = atol(rbuff);
      
                  //printf("part3: %Ld \n\r",parseval);
      }
      if(parsenum <= parseitm)
      {
                  //printf("parse # %i \n\r",parsenum);
      cmdsta = process(); //Process Command
      }
      else
      {
         printf("syntax error\n\r");
      }
      clrprbuff();
      rtflag = 0;
      rsptr=0;
   }
   ////////// End rs232
      //TODO: User Code
   }
     
}
////////////////////////////////////////////
// Select rs232 TX channel 0 - 3
void chan_sel(int chanx)
{
      output_low(SELA);
      if((chanx & 1) == 1)
      {
      output_high(SELA);
      }
      
       output_low(SELB);
      if(((chanx >> 1) & 1) == 1)
      {
      output_high(SELB);
      }
}
/////////////////////////////////////////////// Battery div 3
void updateADC()
{
   int done;
   set_adc_channel (0x3A) ;
   delay_us (6);
   read_adc (ADC_START_ONLY) ; 
   done = adc_done ();
   WHILE ( !done)
   {
      done = adc_done();
   }
   
   ADC_BT = read_adc (ADC_READ_ONLY);
}
//////////////////////////////////////////////
void copyright()
{
   char one[1];
   BYTE CONST TABLE [25] = {"Copyright2023 MD SIEGEL"};
   one[0] = TABLE[0];
}
////////////////////////////
int read232()
{
   int df;
   buflen = rsptr - 2;
   
   clrprbuff();    // clear copy to buff
   if(buflen > 0)
   {
     for (df = 0; df <= buflen; df++)
     {
     rsreadbuff[df] = rs232buff[df]; // copy buffer
     }
  
  for (df = 0; df <= 81; df++)
  {
     if (rs232buff[df] > 126)  // Filter buffer
     {
      rs232buff[df] = 32;
     }
   }
   
   }
   return (buflen);
}
int is_rs232()
{
   return(rtflag); // return ready to process
}
void clrprbuff() // Clear serial input buffer
{  int i;
   for(i=0;i<=81;i++)
   {
   rsreadbuff[i]=0;
   }
}
void clrrsbuff() // Clear rs232 input buffer
{  int i;
   for(i=0;i<=80;i++)
   {
   rs232buff[i]=0;
   }
}
//////////////////// split string
int parseit(char * buf1, int clen) // Parse Cgarater String input
{
   int parsecnt,cindx,cflag,cli;
   char tmpz;
   parsecnt = 0;
   cflag = 0;
   for(cli = 0;cli <=8;cli++)
   {
   split[cli] = 0;
   }
   for(cindx = 0;cindx <= clen;cindx++)
   {
      tmpz = *(buf1 + cindx);
      if (tmpz == 0)
      {
         return(parsecnt);
      }
      if ((tmpz != 0x20)&(cflag == 0)) // look for space or 0 char
      {
         cflag = 1;
         split[parsecnt] = cindx;
         parsecnt++;
      }
      if (tmpz == 0x20)
      {
         cflag = 0;
      }
   }
}
/////////////////////////// get parm pos
char * strpiece(int pieceNum,int clen, char *buf1) // get parse piece
{
   int cindx;
   char tmpz;
   char tchr[20];
   for(cindx = 0;cindx <=20;cindx++)
   {
   tchr[cindx]=0;
   }
   for (cindx = 0; cindx <= clen; cindx++)
   {
      tmpz = *(buf1 +pieceNum + cindx);
      if((tmpz == 0x20)|(tmpz == 0))
      {
         
         return((char *) tchr);
      }
      tchr[cindx] = tmpz;
   }
   return((char *) tchr);
}

////////////////// compair return
int compit(char *buf1, char *buf2, int clen) // compair to table
{
   int i;
   char tmpz,tmpx;
    
   for (i=0; i <=clen;i++)
   {
      tmpz = *(buf1 +i);
      tmpx = *(buf2 +i);
      if((tmpz == 0)&(tmpx == 0))
      {
      return(1);
      }

      if(tmpz != tmpx)
      {
         return(0);
      }
   }
   return(1);
}
////////////////////////////////// Process terninal input
int process()
{
   int xret;
   xret = 0;
    // printf("sa #%s\n\r",xstring1);
   switch (parsenum) {

    case 0: // VER
           if(parsetype==0)
            {
            printf("VER 0.2 ");
            updateADC();
            if(ADC_BT <= 735)
            {  printf("BATT <LOW>\n\r");}
            else
            {printf("BATT <GOOD>\n\r");}
           
            }
           break;

    case 1: // SDATE
           if(parsetype==0)
            {
             xsetDate();
            }

           break;
    
    case 2: // STIME
           if(parsetype==0)
            {
            xsetTime();
            }

           break;
           
    case 3: //RDT
           if(parsetype==0)
            {
               rtc_read(&wizardTempTime);
               
                printf("%02u/%02u/20%02u %02u:%02u:%02u\n\r",wizardTempTime.tm_mon,wizardTempTime.tm_mday,wizardTempTime.tm_year,wizardTempTime.tm_hour,wizardTempTime.tm_min,wizardTempTime.tm_sec);

            }
           break;
           
    case 4: //MODE
           if(parsetype==0)
            {
             if(!strcmp(slp,xstring1))
            {
            qsleep();
            }
            else
            { printf("MODE err \n\r");
            }
            }
           break;
    case 5: //PEEK 
             if(parsetype==0)
            {
            //printf("PEEK \n\r");
            //printf("sa #%s\n\r",xstring1);
            aboo = atol(xstring1);
            printf("Value = 0x%x\n\r",*aboo);
            }
            break;
    case 6: //PEEK 
             if(parsetype==0)
            {
            //printf("POKE \n\r");
            //printf("sa #%s\n\r",xstring1);
            xpoke();
//!            aboo = atol(xstring1);
//!            printf("Value = %x\n\r",*aboo);
            }
            break;        
    case 7: //HELP
           if(parsetype==0)
            {
            printf("------------------------------\n\r");
              printf("Commands UPPER CASE\n\r");
              printf("?<Return Value>, $<Set Value> _ CMD arg(CR)\n\r");
              printf("------------------------------\n\r");
              printf("? VER; VER # <BATT> \n\r");
              printf("$ SDATE; SDATE mm/dd/yr  \n\r");
              printf("$ STIME; STIME hh:mm  \n\r");
              printf("? RTD; RTD; MM/DD/YY HH:MM:SS (24hr). \n\r");
              printf("$ MODE; MODE SLEEP. \n\r");
              printf("? HELP; HELP \n\r");
              printf("------------------------------\n\r");
              if(!strcmp(xstring1,pls))
              {
              printf("$ PEEK; PEEK 0xADD0. \n\r");
              printf("? POKE; POKE 0xADD0:0xF0 \n\r");
              }
            }
           break;
   default:printf("Error\n\r");

            break; }

   return(xret);
}
/////////////////////////////////////
void xsetTime() // time field
{
    int thr, tmn;
              
               xstring2[0] = xstring1[0];
               xstring2[1] = xstring1[1];
               thr = atoi(xstring2);
           wizardTempTime.tm_hour = thr;
                xstring2[0] = xstring1[3];
                xstring2[1] = xstring1[4];
               tmn = atoi(xstring2);
           wizardTempTime.tm_min = tmn;
           wizardTempTime.tm_sec = 1;
           rtc_write(&wizardTempTime);
     
}
/////////////////////////////  Date field
void xsetDate()
{
         int dda, dmo, dyr;
               
               xstring2[0] = xstring1[0];
               xstring2[1] = xstring1[1];
               dda = atoi(xstring2);
            wizardTempTime.tm_mon = dda;
               xstring2[0] = xstring1[3];
               xstring2[1] = xstring1[4];
               dmo = atoi(xstring2);
            wizardTempTime.tm_mday = dmo;
               xstring2[0] = xstring1[6];
               xstring2[1] = xstring1[7];
               dyr = atoi(xstring2);
            wizardTempTime.tm_year = dyr;
          
            wizardTempTime.tm_sec = 1;
            rtc_write(&wizardTempTime);
     
}
void qsleep()
{
         printf("Now in sleep mode \n\r");
          output_low(LED);
          disable_interrupts(INT_TIMER0);
          chan_sel(1);
          sleep();
}
void xpoke()
{
         char *padr;
         int8 pvalx;
               xstring4[0] = xstring1[0];
               xstring4[1] = xstring1[1];
               xstring4[2] = xstring1[2];
               xstring4[3] = xstring1[3];
               xstring4[4] = xstring1[4];
               xstring4[5] = xstring1[5];
               xstring4[6] = (char)0x00;
               padr = atol(xstring4);
         
               xstring2[0] = xstring1[7];
               xstring2[1] = xstring1[8];
               xstring2[2] = xstring1[9];
               xstring2[3] = xstring1[10];
               pvalx = atoi(xstring2);
               // printf(" %4lX : %X \n\r",padr,pvalx);
               *padr = (char)pvalx;
                
}
