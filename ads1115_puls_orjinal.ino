

#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_ADS1X15.h>
#include <EEPROM.h>

Adafruit_ADS1115 ads;

#include <SoftwareSerial.h>  // Software Serial kütüphanesini ekledik.
SoftwareSerial hc06(10,11);  // Bluetooth TX, RX pinler

int puls = 12;
int Ust = 4;
int Alt = 3;
int Sgnd = 2;
int test = A0;
byte Say = 0;
byte KesmeSay =0;
byte GonderSay = 0;
int16_t adc0, adc1, adc2, adc3, BattAdc,UstAdc,AltAdc,SgndAdc,kayitAdresi;
byte UstBekle,AltBekle;

byte Giden[3];
byte gelen[2];
byte GelenSay =0;

//long TimerSure = 4000000;
void setup() {
  pinMode(test, OUTPUT);
  pinMode(puls, OUTPUT);
  pinMode(Ust, OUTPUT);
  pinMode(Alt, OUTPUT);
  pinMode(Sgnd, OUTPUT);
  digitalWrite(Ust, LOW);
  digitalWrite(Alt, LOW);
  digitalWrite(Sgnd, LOW);
  digitalWrite(test, LOW);
  digitalWrite(test, LOW);
  digitalWrite(puls, HIGH);
  
  kayitAdresi =19;
  UstBekle =EEPROM.read(kayitAdresi);
  if(UstBekle==255 ||UstBekle ==0){UstBekle =50;}
  kayitAdresi =21;
  AltBekle =EEPROM.read(kayitAdresi);
  if(AltBekle==255 ||AltBekle ==0){AltBekle =30;}
  hc06.begin(115200);      // Bluetooth haberleşmesi başlat


  
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  //ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  delay(100);

  if (!ads.begin()) {
    don:
      Giden[2] =101;
      Giden[1] =101;
      Giden[0] =101;
      hc06.write(Giden,3);
    delay(100);
    goto don;
  }

  KesmeReset();

}

void KesmeReset() {
  cli();  /* Ayarlamaların yapılabilmesi için öncelikle kesmeler durduruldu */
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A =15;// 62;//15624;  /*  zaman sayıcısı ayarlandı */
  TCCR1B |= (1 << WGM12);  /* Adımlar arasında geçen süre kristal hızının 1024'e bölümü olarak ayarlandı */
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);  /* Timer1 kesmesi aktif hale getirildi */
  sei();  /* Timer1 kesmesinin çalışabilmesi için tüm kesmeler aktif hale getirildi */
}

ISR(TIMER1_COMPA_vect) {
  KesmeSay++;
  if(KesmeSay ==1){
    for(int i=0;i<5;i++){
      digitalWrite(puls, LOW);
      delayMicroseconds(5);
      digitalWrite(puls, HIGH);
      delayMicroseconds(5);
      }
  digitalWrite(puls, LOW);
  delayMicroseconds(100);
  digitalWrite(puls, HIGH);

  delayMicroseconds(UstBekle);

  digitalWrite(Ust, HIGH);
  delayMicroseconds(10);
  digitalWrite(Ust, LOW);

  delayMicroseconds(AltBekle);

  digitalWrite(Alt, HIGH);
  delayMicroseconds(10);
  digitalWrite(Alt, LOW);

  }
  if(KesmeSay>=2){
    KesmeSay =0;
  digitalWrite(Sgnd, HIGH);
  delayMicroseconds(10);
  digitalWrite(Sgnd, LOW);
  }
   
}



void loop() {
 Say++;
  if (Say == 1) {
   digitalWrite(test, HIGH); 
    adc0 = ads.readADC_SingleEnded(0);
    double bs =(BattAdc +adc0)/2;
    BattAdc =(int)bs; 
digitalWrite(test, LOW);
  }
  if (Say == 2) {
    adc1 = ads.readADC_SingleEnded(1);
    double as =(AltAdc +adc1)/2;
    AltAdc =(int)as;
  }
  if (Say == 3) {
    adc2 = ads.readADC_SingleEnded(2);
    double gs =(SgndAdc +adc2)/2;
    SgndAdc =(int)gs; 
  }
  if (Say == 4) {
    adc3 = ads.readADC_SingleEnded(3);
    double us =(UstAdc +adc3)/2;
    UstAdc =(int)us; 
  }
if (Say >= 5) {
    Say =0;
    GonderSay++;
    if(GonderSay==1){
      Giden[2] =lowByte(BattAdc);
      Giden[1] =highByte(BattAdc);
      Giden[0] =95;
      hc06.write(Giden,3);
      }
    if(GonderSay==2){
      Giden[2] =lowByte(AltAdc);
      Giden[1] =highByte(AltAdc);
      Giden[0] =96;
      hc06.write(Giden,3);
      }
    if(GonderSay==3){
      Giden[2] =lowByte(SgndAdc);
      Giden[1] =highByte(SgndAdc);
      Giden[0] =97;
      hc06.write(Giden,3);      
      }
    if(GonderSay>=4){
      Giden[2] =lowByte(UstAdc);
      Giden[1] =highByte(UstAdc);
      Giden[0] =98;
      GonderSay=0;
      hc06.write(Giden,3);}
  }

  while (hc06.available()>0) {
    byte a =hc06.read();
      GelenSay++;
      if(GelenSay ==1){gelen[0] =a;}
      if(GelenSay >=2){
        gelen[1] =a;
          if(gelen[0]==19){EEPROM.write(gelen[0], gelen[1]);}
          if(gelen[1]==21){EEPROM.write(gelen[0], gelen[1]);}
        GelenSay =0;
        break;
        }
    }

}
