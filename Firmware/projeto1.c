#include <projeto1.h>

int contInt = 0;
int dutyCicles = 4;

#INT_TIMER2
void  TIMER2_isr(void) {
   contInt++;
   if(contInt > 80){
      contInt = 0;
      output_high(PIN_A0);
   }
   if(contInt >= dutyCicles) output_low(PIN_A0);
}

#define LCD_ENABLE_PIN PIN_D0
#define LCD_RS_PIN PIN_D1
#define LCD_RW_PIN PIN_D2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
#include <lcd.c>

#define RTC_SDA  PIN_C4
#define RTC_SCL  PIN_C3
#include "ds1307.c"
int sec=17, min=17, hora=17, dia=17, mes=7, ano=17, dow=6;
int secS=17, minS=17, horaS=17, diaS=17, mesS=7, anoS=17;

enum opcoes {NORMAL, MENU, ALT_ANO, ALT_MES, ALT_DIA, ALT_HORA, ALT_MIN, SUCESS_ALTDATA, SET_ALARME, ALT_ALARME, ALT_ALARME_HORA, ALT_ALARME_MIN};

int modoAtual = NORMAL;
int opcaoAtual = 1;
int maximoItens = 2;
int opcaoAlarme = 1;

int botaoConf = FALSE;
int botaoMais = FALSE;
int botaoMenos = FALSE;
int botaoEnter = FALSE;

struct timeAlarme {
   char hora;
   char min;
};

struct timeAlarme alarmeMatriz[26];

void exibeRelogio(){
      ds1307_get_date(dia,mes,ano,dow);
      printf(lcd_putc,"\fHora: %02d:%02d:%02d\nData: %02d/%02d/20%02d",hora ,min, sec, dia, mes, ano);
}

void exibeMenu(){
   maximoItens = 2;
   printf(lcd_putc,"\fMenu Principal:");
   if(opcaoAtual == 1){
      printf(lcd_putc,"\n1 - Alterar Data");
   }
   else if(opcaoAtual == 2){
      printf(lcd_putc,"\n2 - Setar Alarmes");
   }
}

void alterarAno(){
   maximoItens = 50;
   printf(lcd_putc,"\fAlterar Ano:");
   printf(lcd_putc,"\n20%02d",opcaoAtual);
}

void alterarMes(){
   maximoItens = 12;
   printf(lcd_putc,"\fAlterar Mes:");
   printf(lcd_putc,"\n%02d",opcaoAtual);
}

void alterarDia(){
   maximoItens = 31;
   printf(lcd_putc,"\fAlterar dia:");
   printf(lcd_putc,"\n%02d",opcaoAtual);
}

void alterarHora(){
   maximoItens = 24;
   printf(lcd_putc,"\fAlterar Hora:");
   printf(lcd_putc,"\n%02d",opcaoAtual-1);
}

void alterarMin(){
   maximoItens = 60;
   printf(lcd_putc,"\fAlterar Minutos:");
   printf(lcd_putc,"\n%02d",opcaoAtual-1);
}

void exibirSucesso(){
   printf(lcd_putc,"\fAlteracao com\nSucesso!!!");
   delay_ms(1500);
   modoAtual = NORMAL;
   opcaoAtual = 1;
}

void exibeMenuAlarme(){
   int horaA = 0;
   int minA = 0;
   maximoItens = 26;
   printf(lcd_putc,"\fAlarme %d:",opcaoAtual);
   //horaA = ds1307_read_nvram_byte(0x08+(opcaoAtual-1)*2);
   //minA = ds1307_read_nvram_byte(0x09+(opcaoAtual-1)*2);
   horaA = alarmeMatriz[opcaoAtual-1].hora;
   minA = alarmeMatriz[opcaoAtual-1].min;
   if(horaA == -1){
      printf(lcd_putc, "\nNao Configurado!");
   }
   else {
      printf(lcd_putc, "\n%02d:%02d:00",horaA,minA);
   }
}

void alterarAlarme(){
   maximoItens = 2;
   printf(lcd_putc,"\fAlarme %d:",opcaoAlarme);
   if(opcaoAtual == 1){
      printf(lcd_putc,"\n1 - Alterar");
   }
   else if(opcaoAtual == 2){
      printf(lcd_putc,"\n2 - Deletar");
   }
}

void alterarAlarmeHora(){
   maximoItens = 24;
   printf(lcd_putc,"\fAltere Hora (%d):",opcaoAlarme);
   printf(lcd_putc,"\n%02d",opcaoAtual-1);
}

void alterarAlarmeMin(){
   maximoItens = 60;
   printf(lcd_putc,"\fAltere Minutos(%d):",opcaoAlarme);
   printf(lcd_putc,"\n%02d",opcaoAtual-1);
}

void atualizaMatrizAlarmes(){
      char horaV, minV, cont;
      for(cont = 1; cont <= 26; cont ++){
         horaV = ds1307_read_nvram_byte(0x08+(cont-1)*2);
         minV = ds1307_read_nvram_byte(0x09+(cont-1)*2);
         alarmeMatriz[cont-1].hora = horaV;
         alarmeMatriz[cont-1].min = minV;
      }
}

void main() {
   setup_timer_2(T2_DIV_BY_16,78,1);      //252 us overflow, 252 us interrupt
   enable_interrupts(INT_TIMER2);
   enable_interrupts(GLOBAL);

   lcd_init();
   atualizaMatrizAlarmes();
   while(TRUE) {
      //capta a hora
      ds1307_get_time(hora,min,sec);

      //Verifica se Botao Conf foi pressionado
      if(input(PIN_A1) != botaoConf){
         if(botaoConf == 0){
            //INICIO Trata
            if(modoAtual == NORMAL){
               modoAtual = MENU;
            }
            else {
               modoAtual = NORMAL;
               opcaoAtual = 1;
            }
            //FIM Trata
         }
         botaoConf = input(PIN_A1);
      }

      //verifica se Botao Menos foi pressionado
      if(input(PIN_A2) != botaoMenos){
         if(botaoMenos == 0){
            //INICIO Trata
            opcaoAtual--;
            if(opcaoAtual == 0) opcaoAtual = maximoItens;
            //FIM Trata
         }
         botaoMenos = input(PIN_A2);
      }

      //verifica se Botao Mais foi pressionado
      if(input(PIN_A3) != botaoMais){
         if(botaoMais == 0){
            //INICIO Trata
            opcaoAtual++;
            if(opcaoAtual > maximoItens) opcaoAtual = 1;
            //FIM Trata
         }
         botaoMais = input(PIN_A3);
      }

      //verifica se Botao Confirmar foi pressionado
      if(input(PIN_A4) != botaoEnter){
         if(botaoEnter == 0){
            //INICIO Trata
            switch(modoAtual){
               case MENU:
                  if(opcaoAtual == 1){ // alterar data
                     modoAtual = ALT_ANO;
                     opcaoAtual = ano;
                  }
                  else if(opcaoAtual == 2){ //setar alarmes
                     modoAtual = SET_ALARME;
                     opcaoAtual = 1;
                  }
                  break;
               case ALT_ANO:
                  anoS = opcaoAtual;
                  modoAtual = ALT_MES;
                  opcaoAtual = mes;
                  break;
               case ALT_MES:
                  mesS = opcaoAtual;
                  modoAtual = ALT_DIA;
                  opcaoAtual = dia;
                  break;
               case ALT_DIA:
                  diaS = opcaoAtual;
                  modoAtual = ALT_HORA;
                  opcaoAtual = hora;
                  break;
               case ALT_HORA:
                  horaS = opcaoAtual-1;
                  modoAtual = ALT_MIN;
                  opcaoAtual = min;
                  break;
               case ALT_MIN:
                  minS = opcaoAtual-1;
                  ds1307_set_date_time(diaS, mesS, anoS, dow, horaS, minS, secS);
                  modoAtual = SUCESS_ALTDATA;
                  break;
               case SET_ALARME:
                  opcaoAlarme = opcaoAtual;
                  modoAtual = ALT_ALARME;
                  opcaoAtual = 1;
                  break;
               case ALT_ALARME:
                  if(opcaoAtual == 1){
                     modoAtual = ALT_ALARME_HORA;
                     opcaoAtual = 1;
                  }
                  else if(opcaoAtual == 2){
                     //del
                     ds1307_write_nvram_byte(0x08+(opcaoAlarme-1)*2,-1);
                     ds1307_write_nvram_byte(0x09+(opcaoAlarme-1)*2,-1);
                     modoAtual = SUCESS_ALTDATA;
                     atualizaMatrizAlarmes();
                  }
                  break;
              case ALT_ALARME_HORA:
                  ds1307_write_nvram_byte(0x08+(opcaoAlarme-1)*2,opcaoAtual-1);
                  modoAtual = ALT_ALARME_MIN;
                  opcaoAtual = 1;
                  break;
              case ALT_ALARME_MIN:
                  ds1307_write_nvram_byte(0x09+(opcaoAlarme-1)*2,opcaoAtual-1);
                  modoAtual = SUCESS_ALTDATA;
                  atualizaMatrizAlarmes();
                  break;
            }
            //FIM Trata
         }
         botaoEnter = input(PIN_A4);
      }

      //altera a exibicao do LCD
      switch (modoAtual){
         case NORMAL:
            exibeRelogio();
            break;
         case MENU:
            exibeMenu();
            break;
         case ALT_ANO:
            alterarAno();
            break;
         case ALT_MES:
            alterarMes();
            break;
         case ALT_DIA:
            alterarDia();
            break;
         case ALT_HORA:
            alterarHora();
            break;
         case ALT_MIN:
            alterarMin();
            break;
         case SUCESS_ALTDATA:
            exibirSucesso();
            break;
         case SET_ALARME:
            exibeMenuAlarme();
            break;
         case ALT_ALARME:
            alterarAlarme();
            break;
         case ALT_ALARME_HORA:
            alterarAlarmeHora();
            break;
         case ALT_ALARME_MIN:
            alterarAlarmeMin();
            break;
      }

      //varredura e despertador
      int contD;
      for(contD = 0; contD < 26; contD++){
         if(alarmeMatriz[contD].hora == hora){
            if(alarmeMatriz[contD].min == min && (sec == 01)){
               modoAtual = 255;
               printf(lcd_putc,"\fAlarme!!!\nAlimentando...");
               dutyCicles = 8;
               delay_ms(2000);
               dutyCicles = 4;
               modoAtual = NORMAL;
            }
         }
      }
      delay_ms(100);
   }
}
