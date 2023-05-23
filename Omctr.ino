#include <DigiPotX9Cxxx.h>
#include<LiquidCrystal.h>

//#define debug

LiquidCrystal lcd(11, 12, 4, 5, 6, 7);
DigiPot       res(8, 9, 10);


int from = 0;
int to = 100;


double Uo = 4.6;
double u = (Uo / 1024.0);
int R[100] = {
  43,
  144,
  248,
  352,
  455,
  557,
  658,
  761,
  865,
  969,
  1070,
  1169,
  1270,
  1372,
  1473,
  1573,
  1674,
  1773,
  1871,
  1969,
  2090,
  2200,
  2300,
  2410,
  2510,
  2610,
  2720,
  2820,
  2920,
  3030,
  3140,
  3240,
  3340,
  3450,
  3550,
  3650,
  3760,
  3860,
  3960,
  4060,
  4170,
  4270,
  4370,
  4470,
  4580,
  4680,
  4780,
  4880,
  4990,
  5090,
  5190,
  5300,
  5400,
  5500,
  5600,
  5700,
  5810,
  5910,
  6010,
  6110,
  6220,
  6320,
  6420,
  6530,
  6630,
  6730,
  6830,
  6940,
  7040,
  7140,
  7250,
  7350,
  7450,
  7550,
  7660,
  7760,
  7860,
  7970,
  8070,
  8170,
  8270,
  8380,
  8480,
  8580,
  8650,
  8790,
  8890,
  8990,
  9090,
  9190,
  9300,
  9400,
  9510,
  9610,
  9710,
  9810,
  9920,
 10020,
 10120,
 10220
};
int Rxs[100];
double Rx;
int dRx;
double p;


void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  res.reset();



  for(int i = 3; i < 13; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(A0, INPUT);


//  for(int i = 0; i < 100; i++){
//    R[i] = i * 100;
//  }

  digitalWrite(3, HIGH);
  
  lcd.print("restart");
  delay(100);
  lcd.clear();

}

void loop() {
  count();
  count();

  from = 0;
  to = 100;

  lcd.clear();
  lcd.setCursor(3, 0);
  if(isnan(Rx)){
    lcd.print("Infinity");
  }
  else{
    lcd.print(String((int)Rx) + " Om");
    lcd.setCursor(8, 1);
    lcd.print(String(p * 100) + "%");
  }
  
  delay(1000);
}


void count(){
  unsigned long startMs = millis(); 

  #ifdef debug
  Serial.println();
  Serial.println("---------------------");
  Serial.println();
  Serial.println();
  #endif
  
  
  int steps = to - from;
  digitalWrite(3, HIGH);
  delay(10);
  res.reset();


  int optRi = 0;
  int optr = 20000;
  
  for(int i = from; i < to; i++){
    
    res.set(i);

    int r = analogRead(A0);
    double Ux = r * u;
    double U = Uo - Ux;
    double dv = Ux / U;
    int Rxi = R[i] * dv;

    if(Rxi < 0){
      Rx = 0 / 0.0;
      digitalWrite(3, LOW);
      return;
    }

    Rxs[i] = Rxi;

    int otkl = abs(512 - r);
    if(otkl < optr){
      optr = otkl;
      optRi = res.get();
    }

    #ifdef debug
    Serial.println(String(res.get()) + ") " + String(Rxi) + " " + String(R[i]) + " " + String(r) + " " + String(dv) + " " + String(U) + " " + String(Ux));
    #endif

  }

  from = optRi / 3;
  to = optRi * 3;
  to = to > 100 ? 100 : to;
  
  
  #ifdef debug
  Serial.println();
  Serial.println(String(from) + " " + String(to));
  Serial.println();
  #endif



  unsigned long sum = 0;
  for(int i = 0; i < steps; i++){
    sum += Rxs[i + from];
  }
  Rx = sum / steps;
  sum = 0;
  for(int i = 0; i < steps; i++){
    sum += abs(Rxs[i + from] - Rx);
  }
  dRx = sum / steps;
  p = (double)dRx / Rx;


//  if(Rx < 4500){
//    Rx -= Rx * p;
//  }


  #ifdef debug
  Serial.println(String(Rx) + " " + String(p) + " " + String(dRx));
  Serial.println();
  #endif

  digitalWrite(3, LOW);


  unsigned long endMs = millis();

  Serial.println(String(endMs - startMs) + " ms");
}
