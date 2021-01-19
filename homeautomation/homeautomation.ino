#include<SPI.h> 
#include<LiquidCrystal_I2C.h> 
#include<MFRC522.h> 

#define SS_PIN 9 
#define RST_PIN 8 

#include<MQ2.h> 
#include<dht.h> 
#include<Wire.h> 
#include<Servo.h> 

#define DHT11_PIN 47 

MFRC522 mfrc522 (SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4); 
Servo kapumotor; 
Servo garazsmotor; 
dht DHT; 

int pir1 = 2; 
int pir2 = 3; 
int pir3 = 18; 
int pir4 = 19; 
int trigger = 0;
int nappaliLed = 30; 
int konyhaLed = 28; 
int folyosoLed1 = 24; 
int folyosoLed2 = 25; 
int garazsLed = 23; 
int medence1 = 32; 
int medence2 = 33; 
int kintiRele = 43; 
int keritesRele = 40; 
int kiskapuRele = 41; 
int garazskapuRele = 42; 
int szelloztetesRele = 44; 
int garazsRele = 45; 
int piezo = 48; 
int light = 0;
int potmeter = A0; 
int lpg, co, smoke; 
int gas = A2; 
MQ2 mq2(gas); 
int trigPin = 10; 
int echoPin = 11; 
long duration; 
int distance; 
int safetyDistance;
int dht11 = 47; 
int p = 0; 
boolean card = false; 
boolean sotet = true; 

void setup()
{
  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  pinMode(pir3, INPUT); 
  pinMode(pir4, INPUT); 
  pinMode(nappaliLed, OUTPUT); 
  pinMode(konyhaLed, OUTPUT); 
  pinMode(folyosoLed1, OUTPUT); 
  pinMode(folyosoLed2, OUTPUT); 
  pinMode(garazsLed, OUTPUT); 
  pinMode(medence1, OUTPUT); 
  pinMode(medence2, OUTPUT);  
  pinMode(kintiRele, OUTPUT); 
  pinMode(keritesRele, OUTPUT); 
  pinMode(kiskapuRele, OUTPUT); 
  pinMode(garazskapuRele, OUTPUT); 
  pinMode(szelloztetesRele, OUTPUT); 
  pinMode(garazsRele, OUTPUT); 
  pinMode(piezo, OUTPUT);
  pinMode(gas, INPUT); 
  pinMode(potmeter, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(dht11, INPUT); 
  digitalWrite(nappaliLed, LOW);
  digitalWrite(konyhaLed, LOW); 
  digitalWrite(folyosoLed1, LOW);
  digitalWrite(folyosoLed2, LOW);
  digitalWrite(garazsLed, LOW);
  digitalWrite(medence1, LOW);
  digitalWrite(medence2, LOW);
  digitalWrite(kintiRele, HIGH);
  digitalWrite(keritesRele, HIGH);
  digitalWrite(kiskapuRele, HIGH);
  digitalWrite(garazskapuRele, HIGH);
  digitalWrite(szelloztetesRele, HIGH);
  digitalWrite(garazsRele, HIGH); 
  Serial.begin(9600); 
  mq2.begin(); 
  SPI.begin(); 
  mfrc522.PCD_Init(); 
  lcd.init(); 
  lcd.backlight(); 
  lcd.setCursor(4, 0); 
  lcd.print("UDVOZOLJUK"); 
  lcd.setCursor(8, 1); //Kurzor pozicionálás ez esetben 8. karakter a 1. sorban
  lcd.print("A"); //Megadott karakterlánc kiíratása
  Serial.print("A rendszer aktív\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.setCursor(2, 2); //Kurzor pozicionálás ez esetben 2. karakter a 2. sorban
  lcd.print("RENDSZER AKTIV"); //Megadott karakterlánc kiíratása
  lcd.setCursor(8, 3); //Kurzor pozicionálás ez esetben 8. karakter a 3. sorban
  lcd.print(":)"); //Megadott karakterlánc kiíratása
  delay(2000); //Várakozás 2 másodpercig
  Serial.println("Érintse a kártyáját:"); //Megadott karakterlánc kiíratása
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(8, 0); //Kurzor pozicionálás ez esetben 8. karakter a 0. sorban
  lcd.print("A"); //Megadott karakterlánc kiíratása
  lcd.setCursor(4, 1); //Kurzor pozicionálás ez esetben 4. karakter a 1. sorban
  lcd.print("KARYTAJAT"); //Megadott karakterlánc kiíratása
  lcd.setCursor(4, 2); //Kurzor pozicionálás ez esetben 4. karakter a 2. sorban
  lcd.print("ERINTSE A"); //Megadott karakterlánc kiíratása
  lcd.setCursor(3, 3); //Kurzor pozicionálás ez esetben 3. karakter a 3. sorban
  lcd.print("TERMINALHOZ"); //Megadott karakterlánc kiíratása
  delay(500); //Várakozás 500 milliszekundum ideig
  lcd.setCursor(14, 3); //Kurzor pozicionálás ez esetben 14. karakter a 3. sorban
  lcd.print("."); //Megadott karakterlánc kiíratása
  delay(700); //Várakozás 700 milliszekundum ideig
  lcd.setCursor(15, 3); //Kurzor pozicionálás ez esetben 15. karakter a 3. sorban
  lcd.print("."); //Megadott karakterlánc kiíratása
  delay(700); //Várakozás 700 milliszekundum ideig
  lcd.setCursor(16, 3); //Kurzor pozicionálás ez esetben 16. karakter a 3. sorban
  lcd.print("."); //Megadott karakterlánc kiíratása
  noTone(piezo);
  attachInterrupt(digitalPinToInterrupt(pir1), motion1, RISING);//Nappali PIR megszakítás funkció
  attachInterrupt(digitalPinToInterrupt(pir2), motion2, RISING);//Konyha PIR megszakítás funkció
  attachInterrupt(digitalPinToInterrupt(pir3), motion3, RISING);//Folyosó PIR megszakítás funkció
  attachInterrupt(digitalPinToInterrupt(pir4), motion4, RISING);//Garázs PIR megszakítás funkció
}

void motion1() //ISR funkció mozgás észlelésére a nappaliban
{
  trigger = 1; //flag változó értékének változtatása
}

void motion2() //ISR funkció mozgás észlelésére a konyhában
{
  trigger = 2; //flag változó értékének változtatása
}

void motion3() //ISR funkció mozgás észlelésére a folyosón
{
  trigger = 3; //flag változó értékének változtatása
}

void motion4() //ISR funkció mozgás észlelésére a garázsban
{
  trigger = 4; //flag változó értékének változtatása
}

void temperature() //Funkció a hőmérséklet értékének kiíratására
{
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(1, 1); //Kurzor pozicionálás ez esetben 1. karakter a 1. sorban
  lcd.print("A HOMERSEKLET:"); //Megadott karakterlánc kiíratása
  lcd.setCursor(3, 2); //Kurzor pozicionálás ez esetben 3. karakter a 2. sorban
  lcd.print(DHT.temperature); //A hőmérséklet értékének kiíratása
  delay(5000); //Várakozás 5 másodpercig
}

void humidity() //Funkció a páratartalom értékének a kiíratására
{
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(1, 1); //Kurzor pozicionálás ez esetben 1. karakter a 1. sorban
  lcd.print("A PARATARTALOM"); //Megadott karakterlánc kiíratása
  lcd.setCursor(3, 2); //Kurzor pozicionálás ez esetben 3. karakter a 2. sorban
  lcd.print(DHT.humidity); //A páratartalom értékének kiíratása
  delay(5000); //Várakozás 5 másodpercig
}

void checktemp() //Funkció hőmérséklet értékének a kiolvasására a DHT11-ből
{
  int chk = DHT.read11(DHT11_PIN); //A DHT11 értékének a kiolvasása és segédváltozóban tárolása
  if (DHT.temperature >= 25) //Feltétel vizsgálat:
  {
    lcd.clear(); //Az LCD kijelző tartalmának a törlése
    lcd.setCursor(3, 1); //Kurzor pozicionálás ez esetben 3. karakter a 1. sorban
    lcd.print("HUTES INDITASA"); //Megadott karakterlánc kiíratása
    digitalWrite(szelloztetesRele, LOW); //A hűtés bekapcsolása
    delay(10000); //Váralkozás 10 másodpercig
  }
  else //Egyébként
  {
    digitalWrite(szelloztetesRele, HIGH); //hűtés kikapcsolvva
  }
}

void fume() //Funkció a gazszivárgás megjelenítésére
{
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(1, 1); //Kurzor pozicionálás ez esetben 1. karakter a 1. sorban
  lcd.print("NINCS GAZSZIVARGAS"); //Megadott karakterlánc kiíratása
  delay(5000); //Várakozás 5 másodpercig
  trigger = 0; //Flag változó értékének változtatása
}

void leakage() //Funkció gázszivárgás észlelésére
{
  lpg = mq2.readLPG(); //Az lpg értékének kiolvasása és változóban tárolása 
  co = mq2.readCO(); //A co értékének kiolvasása és változóban tárolása
  smoke = mq2.readSmoke(); //A füst értékének kiolvasása és változóban tárolása
  if (smoke > 10) //Feltétel vizsgálat:
  {
    digitalWrite(szelloztetesRele, LOW); //A szellőztetés indítása
    lcd.clear(); //Az LCD kijelző tartalmának a törlése
    lcd.setCursor(3, 1); //Kurzor pozicionálás ez esetben 3. karakter a 1. sorban
    lcd.print("GAZSZIVARGAS"); //Megadott karakterlánc kiíratása
    delay(10000); //Várakozás 10 másodpercig
    trigger = 0; //Flag változó értékének változtatása
  }
  else //Egyébként
  {
    delay(1000); //Várakozás 1 másodpercig
    digitalWrite(szelloztetesRele, HIGH); //A szellőztetés kikapcsolva
  }
}

void beep () //Funkció hangjelzésre
{
  tone(piezo, 800); //A piezo elem megszólaltatása 800hz-en
  delay(100); //Szüneteltetés 100 milliszekundum ideig
  tone(piezo, 1000); //A piezo elem megszólaltatása 1000hz-en
  delay(100); //Szüneteltetés 100 milliszekundum ideig
  tone(piezo, 1200); //A piezo elem megszólaltatása 1200hz-en
  delay(100); //Szüneteltetés 100 milliszekundum ideig
  tone(piezo, 1400); //A piezo elem megszólaltatása 1400hz-en
  delay(100); //Szüneteltetés 100 milliszekundum ideig
  noTone(piezo); //A piezo elem elhallgattatása
}

void enter () //Funkció a belépés igazolására
{
  delay(1000); //Várakozás 1 másodpercig
  Serial.println("BELEPES ENGEDELYEZVE"); //Megadott karakterlánc kiíratása a soros portra
  Serial.println(); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("BELEPES ENGEDELYEZVE"); //Megadott karakterlánc kiíratása
  delay(2000); //Várakozás 2 másodpercig
  check(); //Check funkció meghívása
  card = true; //A változó értékének igaz-ra változtatása
  trigger = 0; //Flag változó értékének változtatása
}

void alarmcheck() //Funkció a riasztó állapotának ellenőrzésére
{
  if (sotet == true) //Feltétel vizsgálat:
  {
    lcd.clear(); //Az LCD kijelző tartalmának a törlése
    lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
    Serial.print("RIASZTO AKTIV\n"); //Megadott karakterlánc kiíratása a soros portra
    lcd.print("A RIASZTO AKTIV"); //Megadott karakterlánc kiíratása
    delay(2000); //Várakozás 2 másodpercig
  }
  else //Egyébként
  {
    lcd.clear(); //Az LCD kijelző tartalmának a törlése
    lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
    Serial.print("RIASZTO INAKTIV\n"); //Megadott karakterlánc kiíratása a soros portra
    lcd.print("A RIASZTO INAKTIV"); //Megadott karakterlánc kiíratása
    sotet = true; //A változó értékének igaz-ra változtatása
    delay(2000); //Várakozás 2 másodpercig
  }
}

void denied() //Funkció az elutasított belépés visszajelzésére
{
  delay(1000); //Várakozás 1 másodpercig
  Serial.println("BELEPES MEGTAGADVA"); //Megadott karakterlánc kiíratása a soros portra
  Serial.println(); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("BELEPES MEGTAGADVA"); //Megadott karakterlánc kiíratása
  card = false; //A változó értékének hamis-ra változtatása
}

void check() //Funkció a mozgás érzekelésére megszakítás portokon keresztül
{
  switch (trigger) //Több irányú elágazás a flag változó értékének megfelelően:
  {
    case 1: //Első eset ha a változó = 1
      Serial.print("Nappali mozgas\n"); //Megadott karakterlánc kiíratása a soros portra
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
      lcd.print("MOZGAS A NAPPALIBAN."); //Megadott karakterlánc kiíratása
      digitalWrite(medence1, HIGH); //A medence1 vilagít
      digitalWrite(medence2, HIGH); //A medence2 világít
      digitalWrite(kintiRele, LOW); //A kinti világitás bekapcsolva
      for (int i = 0; i <= 5; i++) //for ciklus a nappali világítás villogtatására:
      {
        digitalWrite(nappaliLed, HIGH); //A nappali világítás bekapcsolva
        beep(); //Hangjelzés funkció meghívása
        delay(250); //Várakozás 250 milliszekundum ideig
        digitalWrite(nappaliLed, LOW); //A nappali világítás kikapcsolva
        delay(250); //Várakozás 250 milliszekundum ideig
      }
      digitalWrite(medence1, LOW); //Medence1 kikapcsolva
      digitalWrite(medence2, LOW); //Medence2 kikapcsolva
      digitalWrite(kintiRele, HIGH); //Kinti világítás kikapcsolva
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      trigger = 0; //Flag változó értékének változtatása
      break; //Kilépés az elágazásból
    case 2: //Második eset ha a változó = 2
      Serial.print("Konyha mozgas\n"); //Megadott karakterlánc kiíratása a soros portra
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
      lcd.print("MOZGAS A KONYHABAN."); //Megadott karakterlánc kiíratása
      for (int i = 0; i <= 5; i++) //for ciklus a konyhai világítás villogtatására:
      {
        digitalWrite(konyhaLed, HIGH); //A konyhai világítás bekapcsolva
        beep(); //Hangjelzés funkció meghívása
        delay(250); //Várakozás 250 milliszekundum ideig
        digitalWrite(konyhaLed, LOW); //A konyhai világítás kikapcsolva
        delay(250); //Várakozás 250 milliszekundum ideig
      }
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      trigger = 0; //Flag változó értékének változtatása
      break; //Kilépés az elágazásból
    case 3: //Harmadik eset ha a változó = 3
      Serial.print("Folyoso mozgas\n"); //Megadott karakterlánc kiíratása a soros portra
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
      lcd.print("MOZGAS A FOLYOSON"); //Megadott karakterlánc kiíratása
      for (int i = 0; i <= 5; i++) //for ciklus a folyosó világítás villogtatására:
      {
        digitalWrite(folyosoLed1, HIGH); //A folyosó1 világítás bekapcsolva
        digitalWrite(folyosoLed2, HIGH); //A folyosó2 világítás bekapcsolva
        beep(); //Hangjelzés funkció meghívása
        delay(250); //Várakozás 250 milliszekundum ideig
        digitalWrite(folyosoLed1, LOW); //A folyosó1 világítás kikapcsolva
        digitalWrite(folyosoLed2, LOW); //A folyosó2 világítás kikapcsolva
        delay(250); //Várakozás 250 milliszekundum ideig
      }
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      trigger = 0; //Flag változó értékének változtatása
      break; //Kilépés az elágazásból
    case 4: //Negyedik eset ha a változó = 4
      Serial.print("Garazs mozgas\n"); //Megadott karakterlánc kiíratása a soros portra
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
      lcd.print("MOZGAS A GARAZSBAN."); //Megadott karakterlánc kiíratása
      for (int i = 0; i <= 5; i++) //for ciklus a garázs világítás villogtatására:
      {
        digitalWrite(garazsLed, HIGH); //A garázs világítás bekapcsolva
        beep(); //Hangjelzés funkció meghívása
        delay(250); //Várakozás 250 milliszekundum ideig
        digitalWrite(garazsLed, LOW); //A garázs világítás kikapcsolva
        delay(250); //Várakozás 250 milliszekundum ideig
      }
      lcd.clear(); //Az LCD kijelző tartalmának a törlése
      trigger = 0; //Flag változó értékének változtatása
      break; //Kilépés az elágazásból
    default: //Egyébként:
      Serial.print("Nincs mozgas\n"); //Megadott karakterlánc kiíratása a soros portra
      digitalWrite(kintiRele, HIGH); //A kinti világítás kikapcsolva
      //trigger = 0;
      break; //Kilépés az elágazásból
  }
}

void kapunyitas() //Funkció a kapu nyitására
{
  Serial.print("Kapunyitas\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("KAPUNYITAS"); //Megadott karakterlánc kiíratása
  digitalWrite(garazskapuRele, LOW); //A garázskapu villogás bekapcsolva
  digitalWrite(kiskapuRele, LOW); //A kiskapu villogás bekapcsolva
  delay(3000); //Várakozás 3 másodpercig
  kapumotor.attach(12); //A kapumotor illesztése a 12-es pinre
  kapumotor.write(0); //A  kapumotor 0-dik pozícióba állítása
  for (int i = 0; i <= 100; i++) //For ciklus a szervomotor léptetésére:
  {
    kapumotor.write(i); //A kapumotor pozíciója megegyezik a ciklusváltozó(i) értékével
    delay(40); //A léptetés sűrűsége 40 milliszekundum
  }
  Serial.print("Kapu nyitva\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("KAPU NYITVA"); //Megadott karakterlánc kiíratása
}

void kapuzaras() //Funkció a kapu zárására
{
  Serial.print("Kapuzaras\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("KAPUZARAS"); //Megadott karakterlánc kiíratása
  delay(3000); //Várakozás 3 másodpercig
  for (int i = 95; i > 0; i--) //For ciklus a szervomotor léptetésére:
  {
    kapumotor.write(i); //A kapumotor pozíciója megegyezik a ciklusváltozó(i) értékével
    delay(40); //A léptetés sűrűsége 40 milliszekundum
  }
  delay(5000); //Várakozás 5 másodpercig
  kapumotor.write(0); //A  kapumotor 0-dik pozícióba állítása
  kapumotor.detach(); //A kapumotor leválasztása
  Serial.print("kapu zarva\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("KAPU ZARVA"); //Megadott karakterlánc kiíratása
  digitalWrite(garazskapuRele, HIGH); //A garázskapu villogás kikapcsolása
  digitalWrite(kiskapuRele, HIGH); //A kikskapu villogás kikapcsolása
}

void garazsnyitas() //Funkció a garázs nyitására
{
  Serial.print("GARAZSNYITAS\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("GARAZSNYITAS"); //Megadott karakterlánc kiíratása
  delay(3000); //Várakozás 3 másodpercig
  garazsmotor.attach(5); //A garázsmotor illesztése az 5-ös pinre
  garazsmotor.write(0); //A garázsmotor 0-ik pozícióba állítása
  for (p = 0; p < 100; p++) //For ciklus a szervomotor léptetésére:
  {
    garazsmotor.write(p); //A garázsmotor pozíciója megegyezik a ciklusváltozó(p) értékével
    delay(50); //A léptetés sűrűsége 50 milliszekundum
  }
  delay(5000); //Várakozás 5 másodpercig
  Serial.print("GARAZS NYITVA\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0, 1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("GARAZS NYITVA"); //Megadott karakterlánc kiíratása
}

void garazszaras() //Funkció a garázs zárására
{
  Serial.print("GARAZSZARAS\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0,1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("GARAZSZARAS"); //Megadott karakterlánc kiíratása
  delay(3000); //Várakozás 3 másodpercig
  for (p = 100; p >= 1; p--) //For ciklus a szervomotor léptetésére:
  {
    garazsmotor.write(p); //A garázsmotor pozíciója megegyezik a ciklusváltozó(p) értékével
    delay(50); //A léptetés sűrűsége 50 milliszekundum
  }
  garazsmotor.write(0); //A garázsmotor 0-ik pozícióba állítása
  delay(5000); //Várakozás 5 másodpercig
  garazsmotor.detach(); //A garázsmotor leválasztása
  Serial.print("GARAZS ZARVA\n"); //Megadott karakterlánc kiíratása a soros portra
  lcd.clear(); //Az LCD kijelző tartalmának a törlése
  lcd.setCursor(0,1); //Kurzor pozicionálás ez esetben 0. karakter a 1. sorban
  lcd.print("GARAZS ZARVA"); //Megadott karakterlánc kiíratása
  delay(2000); //Várakozás 2 másodpercig
  digitalWrite(garazsRele, LOW); //Garázs szellőztetés bekapcsolása
  delay(5000); //Szellőztetés 5 másodpercig
  digitalWrite(garazsRele, HIGH); //Garázs szellőztetés kikapcsolása
}

void loop() //ciklus
{
  int chk = DHT.read11(DHT11_PIN); //A DHT11 értékének a kiolvasása és segédváltozóban tárolása
  if ( !mfrc522.PICC_IsNewCardPresent()) //Feltétel vizsgálat, ha új kártya van jelen:
  {
    return; //kilépés a feltétel vizsgálatból
  }
  if (! mfrc522.PICC_ReadCardSerial()) //Feltétel vizgsálat kártya ID-jének kiolvasása
  {
    return; //kilépés a feltétel vizsgálatból
  }
  Serial.print("UID tag:"); //Megadott karakterlánc kiíratása a soros portra
  String content = ""; //String típusú üres karakterláncot tartalmazó változó
  byte letter; //Byte típusú változó
  for (byte i = 0; i < mfrc522.uid.size; i++) //for ciklus mely végighalad a kártya ID-ján:
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""); //Az ID ben lévő számok Hexadecimális stringre alakítása
    Serial.print(mfrc522.uid.uidByte[i], HEX); //Az átalakított ID kiíratása a soros portra
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ")); //konkatenálása az ID vel és 0x hozzarendelése
    content.concat(String(mfrc522.uid.uidByte[i], HEX)); //A konkatenáció kiíratása a soros portra
  }
  Serial.println(); //Üres karakterlánc kiíratása a soros portra
  content.toUpperCase(); //A változó tartalmának nagy karakterekre alakítása
  if (content.substring(1) == "F0 1E 78 7A") //Feltétel vizsgálat:
  {
    enter(); //Belépési funkció hívása
    kapunyitas(); //Kapunyitás funkció hívása
    garazsnyitas(); //Garázsnyitás funkció hívása
    kapuzaras(); //Kapuzárás funkció hívása
    garazszaras(); //Garázs zárás funkció hívása
    temperature(); //Hőmérséklet megjelenítése funkció hívása
    humidity(); //Páratartalom megjelenítése funkció hívása
    fume(); //Gázszivárgást megjelenítő funkció hívása
    alarmcheck(); //A riasztó állapotát ellenőrző funkció hívása
  }
  else //Egyébként
  {
    denied(); //Elutasítást igazoló funkció hívása
  }
  for ( ;;) //for ciklus a hőmérséklet, gázszivárgás és mozgás érzékelésére
  { 
    checktemp(); //A hőmérséklet mérése funkció hívása
    leakage(); //Gázszivárgást ellenőrző funkció hívása
    check(); //Mozgás észlelését figyelő funkció hívása
  }
}
