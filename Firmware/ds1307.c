///////////////////////////////////////////////////////////////////////////////////////
///                               DS1307.C                                           ///
///                     Driver for Real Time Clock                                   ///
///                     modified by Redpic 08/2006                                   ///
///                  http://picmania.garcia-cuervo.com                               ///
///                                                                                  ///
/// void ds1307_init(val)                                                            ///
///                  - Enable oscillator without clearing the seconds register       ///
///                    used when PIC loses power and DS1307 run from 3V BAT          ///
///                  - Config Control Register with next parameters:                 ///
///                     DS1307_ALL_DISABLED          All disabled                    ///
///                     DS1307_OUT_ON_DISABLED_HIHG  Out to Hight on Disable Out     ///
///                     DS1307_OUT_ENABLED           Out Enabled                     ///
///                     DS1307_OUT_1_HZ              Freq. Out to 1 Hz               ///
///                     DS1307_OUT_4_KHZ             Freq. Out to 4.096 Khz          ///
///                     DS1307_OUT_8_KHZ             Freq. Out to 8.192 Khz          ///
///                     DS1307_OUT_32_KHZ            Freq. Out to 32.768 Khz         ///
///                                                                                  ///
///                     Example init:                                                ///
///                     ds1307_init(DS1307_ALL_DISABLED);                            ///
///                     ds1307_init(DS1307_OUT_ENABLED | DS1307_OUT_1_HZ);           ///
///                                                                                  ///
/// void ds1307_set_date_time(day,mth,year,dow,hour,min,sec) - Set the date/time     ///
///                                                                                  ///
/// void ds1307_get_date(day,mth,year,dow)                   - Get the date          ///
///                                                                                  ///
/// void ds1307_get_time(hr,min,sec)                         - Get the time          ///
///                                                                                  ///
/// char ds1307_read_nvram_byte(char addr)                   - Read byte in address  ///
///                                                                                  ///
/// void ds1307_write_nvram_byte(char addr, char value)      - Write byte in address ///
///                                                                                  ///
/// void ds1307_get_day_of_week(char* ptr)                   - Get string Day Of Week///
///                                                                                  ///
/// If defined USE_INTERRUPTS all functions disable Global Interrupts on starts and  ///
///                           enable Global on ends else usar can do it hiself       ///
///                                                                                  ///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef DELAY_DS1307
   #define DELAY_DS1307 100
#endif

#ifndef RTC_SDA
#define RTC_SDA  PIN_B0
#define RTC_SCL  PIN_B1
#endif

#use i2c(master, sda=RTC_SDA, scl=RTC_SCL)

#define DS1307_ALL_DISABLED         0b00000000 // All disabled
#define DS1307_OUT_ON_DISABLED_HIHG 0b10000000 // Out to Hight on Disable Out
#define DS1307_OUT_ENABLED          0b00010000 // Out Enabled
#define DS1307_OUT_1_HZ             0b00000000 // Freq. Out to 1 Hz
#define DS1307_OUT_4_KHZ            0b00000001 // Freq. Out to 4.096 Khz
#define DS1307_OUT_8_KHZ            0b00000010 // Freq. Out to 8.192 Khz
#define DS1307_OUT_32_KHZ           0b00000011 // Freq. Out to 32.768 Khz

#define Start_user_address_nvram    0x08
#define End_user_address_nvram      0x3f

char days_of_week[7][5]={"SAB\0","DOM\0","SEG\0","TER\0","QUA\0","QUI\0","SEX\0"};

byte ds1307_bin2bcd(byte binary_value);
byte ds1307_bcd2bin(byte bcd_value);

void ds1307_init(int val){

   byte seconds = 0;

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

   i2c_start();
   i2c_write(0xD0);
   i2c_write(0x00);
   i2c_start();
   i2c_write(0xD1);
   seconds = ds1307_bcd2bin(i2c_read(0));
   i2c_stop();
   seconds &= 0x7F;

   delay_us(3);

   i2c_start();
   i2c_write(0xD0);
   i2c_write(0x00);
   i2c_write(ds1307_bin2bcd(seconds));
   i2c_start();
   i2c_write(0xD0);
   i2c_write(0x07);
   i2c_write(val);
   i2c_stop();

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif

}

void ds1307_set_date_time(byte day, byte mth, byte year, byte dow, byte hr, byte min, byte sec){

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

  sec &= 0x7F;
  hr &= 0x3F;

  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);
  i2c_write(ds1307_bin2bcd(sec));
  i2c_write(ds1307_bin2bcd(min));
  i2c_write(ds1307_bin2bcd(hr));
  i2c_write(ds1307_bin2bcd(dow));
  i2c_write(ds1307_bin2bcd(day));
  i2c_write(ds1307_bin2bcd(mth));
  i2c_write(ds1307_bin2bcd(year));
  i2c_stop();

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif
   delay_ms(DELAY_DS1307);
}

void ds1307_get_date(byte &day, byte &mth, byte &year, byte &dow){

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x03);
  i2c_start();
  i2c_write(0xD1);
  dow  = ds1307_bcd2bin(i2c_read() & 0x7f);
  day  = ds1307_bcd2bin(i2c_read() & 0x3f);
  mth  = ds1307_bcd2bin(i2c_read() & 0x1f);
  year = ds1307_bcd2bin(i2c_read(0));
  i2c_stop();

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif
   delay_ms(DELAY_DS1307);

}

void ds1307_get_time(byte &hr, byte &min, byte &sec){

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);
  i2c_start();
  i2c_write(0xD1);
  sec = ds1307_bcd2bin(i2c_read() & 0x7f);
  min = ds1307_bcd2bin(i2c_read() & 0x7f);
  hr  = ds1307_bcd2bin(i2c_read(0) & 0x3f);
  i2c_stop();

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif
   delay_ms(DELAY_DS1307);

}


char ds1307_read_nvram_byte(char addr){

   char retval;

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

   i2c_start();
   i2c_write(0xD0);
   i2c_write(addr);

   i2c_start();
   i2c_write(0xD1);
   retval = i2c_read(0);
   i2c_stop();

   return(retval);

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif
   delay_ms(DELAY_DS1307);

}

void ds1307_write_nvram_byte(char addr, char value){

#ifndef USE_INTERRUPTS
   disable_interrupts(global);
#endif

   i2c_start();
   i2c_write(0xD0);
   i2c_write(addr);
   i2c_write(value);
   i2c_stop();

#ifndef USE_INTERRUPTS
   enable_interrupts(global);
#endif
   delay_ms(DELAY_DS1307);

}

void ds1307_get_day_of_week(char* ptr){

   byte lday;
   byte lmonth;
   byte lyr;
   byte ldow;
   ds1307_get_date(lday,lmonth,lyr,ldow);
   sprintf(ptr,"%s",days_of_week[ldow]);
}

///////////////////////////////////////////////////////////////////////////////

byte ds1307_bin2bcd(byte binary_value){

  byte temp;
  byte retval;

  temp = binary_value;
  retval = 0;
  while(1){
    if(temp >= 10){
      temp -= 10;
      retval += 0x10;
    }else{
      retval += temp;
      break;
    }
  }
  return(retval);
}

byte ds1307_bcd2bin(byte bcd_value){

  byte temp;

  temp = bcd_value;
  temp >>= 1;
  temp &= 0x78;
  return(temp + (temp >> 2) + (bcd_value & 0x0f));
}

///////////////////////////////////////////////////////////////////////////////
