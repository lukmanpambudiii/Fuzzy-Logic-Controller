#include <IBusBM.h>
#include <Fuzzy.h>

IBusBM IBus;

// Pake driver sek iso maju tok
int motorRightPin1 = 12;
int motorRightPin2 = 13;

int motorLeftPin1 = 18;
int motorLeftPin2 = 19;

// Pompa 1
int relay1Pin = 21;
// Pompa 2
int relay2Pin = 22;
// Fan DC
int relay3Pin = 23;

// Indikator mode manual
int ledKuning = 32;
// Indikator mode otomatis
int ledHijau = 33;

///durasi belok
int turnDuration = 4000;
//durasi maju 
int fwdDuration = 1500;

int motorSpeed;
int motorTurn;

unsigned long lastCommandTime = 0;
unsigned long lastReceiveTime = 0; // Tambahkan variabel untuk waktu terakhir menerima data

unsigned long previousTime = 0;
unsigned long sampleTime = 100; // Interval waktu untuk perhitungan PID (dalam milidetik)

const unsigned long timeoutDuration = 100;
const unsigned long timeoutDurationSTOP = 500;
const unsigned long timeoutDurationError = 500;
const unsigned long stopDuration = 1000; // Durasi untuk berhenti setelah tidak menerima data

String receivedData;
float errorValue;

// Membuat objek Fuzzy/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Fuzzy *fuzzy = new Fuzzy();

// Mendefinisikan FuzzySet untuk input dan output

// Input membership function untuk error

FuzzySet *Kiri_Tajam = new FuzzySet(-70, -70, -50, -30);
FuzzySet *Kiri = new FuzzySet(-40, -30, -20, -10);
FuzzySet *Lurus = new FuzzySet(-10, -5, 5, 10);
FuzzySet *Kanan = new FuzzySet(10, 20 , 30, 40);
FuzzySet *Kanan_Tajam = new FuzzySet(30, 50, 70, 70);

// Input membership function untuk delta_error
FuzzySet *Delta_Kiri_Tajam = new FuzzySet(-70, -50, -30, -30);
FuzzySet *Delta_Kiri = new FuzzySet(-40, -30, -20, -10);
FuzzySet *Delta_Lurus = new FuzzySet(-10, -5, 5, 10);
FuzzySet *Delta_Kanan = new FuzzySet(10, 20, 30, 40);
FuzzySet *Delta_Kanan_Tajam = new FuzzySet(30, 50, 70, 70);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Output membership function (kecepatan motor kanan)
//FuzzySet *RightStop = new FuzzySet(0, 0, 5, 10);
//FuzzySet *RightPelan = new FuzzySet(50, 50, 75, 100);
//FuzzySet *RightSedang = new FuzzySet(100, 125, 150, 175);
//FuzzySet *RightCepat = new FuzzySet(175, 200, 225, 225);


//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
//FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
//FuzzySet *LeftSedang = new FuzzySet(100, 125, 150, 175);
//FuzzySet *LeftCepat = new FuzzySet(175, 200, 225, 225);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Output membership function (kecepatan motor kanan)
//FuzzySet *RightStop = new FuzzySet(0, 0, 5, 10);
FuzzySet *RightPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *RightSedang = new FuzzySet(90, 115, 140, 165);
FuzzySet *RightCepat = new FuzzySet(155, 180, 205, 205);


//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *LeftSedang = new FuzzySet(90, 115, 140, 165);
FuzzySet *LeftCepat = new FuzzySet(155, 180, 205, 205);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float previousError = 0; // Menyimpan nilai error sebelumnya

// Channel FS
int ch_3, ch_1, ch_5, ch_6, ch_7, ch_8;

void setup() {
  Serial.begin(115200);
  IBus.begin(Serial2, 1);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledHijau, OUTPUT);
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  digitalWrite(relay3Pin, LOW);
  digitalWrite(ledKuning, LOW);
  digitalWrite(ledHijau, LOW);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, 0);
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, 0);

  // Menambahkan FuzzyInput untuk error
  FuzzyInput *error = new FuzzyInput(1);
  error->addFuzzySet(Kiri_Tajam);
  error->addFuzzySet(Kiri);
  error->addFuzzySet(Lurus);
  error->addFuzzySet(Kanan);
  error->addFuzzySet(Kanan_Tajam);
  fuzzy->addFuzzyInput(error);

  // Menambahkan FuzzyInput untuk delta_error
  FuzzyInput *delta_error = new FuzzyInput(2);
  delta_error->addFuzzySet(Delta_Kiri_Tajam);
  delta_error->addFuzzySet(Delta_Kiri);
  delta_error->addFuzzySet(Delta_Lurus);
  delta_error->addFuzzySet(Delta_Kanan);
  delta_error->addFuzzySet(Delta_Kanan_Tajam);
  fuzzy->addFuzzyInput(delta_error);

  // Menambahkan FuzzyOutput untuk motor kanan dan kiri
  FuzzyOutput *MotorKanan = new FuzzyOutput(1); // Output 1 untuk Motor Kanan
  MotorKanan->addFuzzySet(RightPelan);
  MotorKanan->addFuzzySet(RightSedang);
  MotorKanan->addFuzzySet(RightCepat);
  fuzzy->addFuzzyOutput(MotorKanan);

  FuzzyOutput *MotorKiri = new FuzzyOutput(2);
  MotorKiri->addFuzzySet(LeftPelan);
  MotorKiri->addFuzzySet(LeftSedang);
  MotorKiri->addFuzzySet(LeftCepat);
  fuzzy->addFuzzyOutput(MotorKiri);


  //  // Definisi FuzzyRuleAntecedent dan FuzzyRuleConsequent berdasarkan tabel // Belok kiri tajam
    FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
    ifKiri_TajamAndDelta_Kiri_Tajam->joinWithAND(Kiri_Tajam, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightCepatAndLeftPelan1 = new FuzzyRuleConsequent();
    thenRightCepatAndLeftPelan1->addOutput(RightCepat);
    thenRightCepatAndLeftPelan1->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifKiri_TajamAndDelta_Kiri_Tajam, thenRightCepatAndLeftPelan1);
    fuzzy->addFuzzyRule(fuzzyRule1);
  
    //Belok Kanan tajam
    FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
    ifKanan_TajamAndDelta_Kanan_Tajam->joinWithAND(Kanan_Tajam, Delta_Kanan_Tajam);
    FuzzyRuleConsequent *thenRightPelanAndLeftCepat = new FuzzyRuleConsequent();
    thenRightPelanAndLeftCepat->addOutput(RightPelan);
    thenRightPelanAndLeftCepat->addOutput(LeftCepat);
    FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifKanan_TajamAndDelta_Kanan_Tajam, thenRightPelanAndLeftCepat);
    fuzzy->addFuzzyRule(fuzzyRule2);
  
    //Lurus enek 5 (rule 3-7)
    FuzzyRuleAntecedent *ifKiriAndDelta_Lurus= new FuzzyRuleAntecedent();
    ifKiriAndDelta_Lurus->joinWithAND(Kiri, Delta_Lurus);
    FuzzyRuleConsequent *thenRightSedangAndLeftSedang1 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftSedang1->addOutput(RightSedang);
    thenRightSedangAndLeftSedang1->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifKiriAndDelta_Lurus, thenRightSedangAndLeftSedang1);
    fuzzy->addFuzzyRule(fuzzyRule3);
  
      FuzzyRuleAntecedent *ifLurusAndDelta_Kiri = new FuzzyRuleAntecedent();
    ifLurusAndDelta_Kiri->joinWithAND(Lurus, Delta_Kiri);
    FuzzyRuleConsequent *thenRightSedangAndLeftSedang2 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftSedang2->addOutput(RightSedang);
    thenRightSedangAndLeftSedang2->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifLurusAndDelta_Kiri, thenRightSedangAndLeftSedang2);
    fuzzy->addFuzzyRule(fuzzyRule4);
  
    FuzzyRuleAntecedent *ifLurusAndDelta_Lurus = new FuzzyRuleAntecedent();
    ifLurusAndDelta_Lurus->joinWithAND(Lurus, Delta_Lurus);
    FuzzyRuleConsequent *thenRightSedangAndLeftSedang3 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftSedang3->addOutput(RightSedang);
    thenRightSedangAndLeftSedang3->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifLurusAndDelta_Lurus, thenRightSedangAndLeftSedang3);
    fuzzy->addFuzzyRule(fuzzyRule5);
  
    FuzzyRuleAntecedent *ifLurusAndDelta_Kanan = new FuzzyRuleAntecedent();
    ifLurusAndDelta_Kanan->joinWithAND(Lurus, Delta_Kanan);
    FuzzyRuleConsequent *thenRightSedangAndLeftSedang4 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftSedang4->addOutput(RightSedang);
    thenRightSedangAndLeftSedang4->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule6 = new FuzzyRule(6, ifLurusAndDelta_Kanan, thenRightSedangAndLeftSedang4);
    fuzzy->addFuzzyRule(fuzzyRule6);
  
    FuzzyRuleAntecedent *ifKananAndDelta_Lurus = new FuzzyRuleAntecedent();
    ifKananAndDelta_Lurus->joinWithAND(Kanan, Delta_Lurus);
    FuzzyRuleConsequent *thenRightSedangAndLeftSedang5 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftSedang5->addOutput(RightSedang);
    thenRightSedangAndLeftSedang5->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule7 = new FuzzyRule(7, ifKananAndDelta_Lurus, thenRightSedangAndLeftSedang5);
    fuzzy->addFuzzyRule(fuzzyRule7);
  
    //Belok kiri enek 9 (Berarti rule 8 - 16)
    FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kiri = new FuzzyRuleAntecedent();
    ifKiri_TajamAndDelta_Kiri->joinWithAND(Kiri_Tajam, Delta_Kiri);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan1 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan1->addOutput(RightSedang);
    thenRightSedangAndLeftPelan1->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule8 = new FuzzyRule(8, ifKiri_TajamAndDelta_Kiri, thenRightSedangAndLeftPelan1);
    fuzzy->addFuzzyRule(fuzzyRule8);
  
    FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Lurus = new FuzzyRuleAntecedent();
    ifKiri_TajamAndDelta_Lurus->joinWithAND(Kiri_Tajam, Delta_Lurus);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan2 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan2->addOutput(RightSedang);
    thenRightSedangAndLeftPelan2->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule9 = new FuzzyRule(9, ifKiri_TajamAndDelta_Lurus, thenRightSedangAndLeftPelan2);
    fuzzy->addFuzzyRule(fuzzyRule9);
  
    FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kanan = new FuzzyRuleAntecedent();
    ifKiri_TajamAndDelta_Kanan->joinWithAND(Kiri_Tajam, Delta_Kanan);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan3 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan3->addOutput(RightSedang);
    thenRightSedangAndLeftPelan3->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule10 = new FuzzyRule(10, ifKiri_TajamAndDelta_Kanan, thenRightSedangAndLeftPelan3);
    fuzzy->addFuzzyRule(fuzzyRule10);
  
    FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
    ifKiri_TajamAndDelta_Kanan_Tajam->joinWithAND(Kiri_Tajam, Delta_Kanan_Tajam);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan4 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan4->addOutput(RightSedang);
    thenRightSedangAndLeftPelan4->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule11 = new FuzzyRule(11, ifKiri_TajamAndDelta_Kanan_Tajam, thenRightSedangAndLeftPelan4);
    fuzzy->addFuzzyRule(fuzzyRule11);
  
    FuzzyRuleAntecedent *ifKiriAndDelta_Kiri_tajam = new FuzzyRuleAntecedent();
    ifKiriAndDelta_Kiri_tajam->joinWithAND(Kiri, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan5 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan5->addOutput(RightSedang);
    thenRightSedangAndLeftPelan5->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule12 = new FuzzyRule(12, ifKiriAndDelta_Kiri_tajam, thenRightSedangAndLeftPelan5);
    fuzzy->addFuzzyRule(fuzzyRule12);
  
    FuzzyRuleAntecedent *ifKiriAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
    ifKiriAndDelta_Kiri_Tajam->joinWithAND(Kiri, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan6 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan6->addOutput(RightSedang);
    thenRightSedangAndLeftPelan6->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule13 = new FuzzyRule(13, ifKiriAndDelta_Kiri_Tajam, thenRightSedangAndLeftPelan6);
    fuzzy->addFuzzyRule(fuzzyRule13);
  
      FuzzyRuleAntecedent *ifKiriAndDelta_Kanan = new FuzzyRuleAntecedent();
    ifKiriAndDelta_Kanan->joinWithAND(Kiri, Delta_Kanan);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan7 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan7->addOutput(RightSedang);
    thenRightSedangAndLeftPelan7->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule14 = new FuzzyRule(14, ifKiriAndDelta_Kanan, thenRightSedangAndLeftPelan7);
    fuzzy->addFuzzyRule(fuzzyRule14);
  
    FuzzyRuleAntecedent *ifKiriAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
    ifKiriAndDelta_Kanan_Tajam->joinWithAND(Kiri, Delta_Kanan_Tajam);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan8 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan8->addOutput(RightSedang);
    thenRightSedangAndLeftPelan8->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule15 = new FuzzyRule(15, ifKiriAndDelta_Kanan_Tajam, thenRightSedangAndLeftPelan8);
    fuzzy->addFuzzyRule(fuzzyRule15);
  
    FuzzyRuleAntecedent *ifLurusAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
    ifLurusAndDelta_Kiri_Tajam->joinWithAND(Lurus, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightSedangAndLeftPelan9 = new FuzzyRuleConsequent();
    thenRightSedangAndLeftPelan9->addOutput(RightSedang);
    thenRightSedangAndLeftPelan9->addOutput(LeftPelan);
    FuzzyRule *fuzzyRule16 = new FuzzyRule(16, ifLurusAndDelta_Kiri_Tajam, thenRightSedangAndLeftPelan9);
    fuzzy->addFuzzyRule(fuzzyRule16);
  
    
    //belok kanan enek 9 (rule 16 -25)
    FuzzyRuleAntecedent *ifLurusAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
    ifLurusAndDelta_Kanan_Tajam->joinWithAND(Lurus, Delta_Kanan_Tajam);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang1 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang1->addOutput(RightPelan);
    thenRightPelanAndLeftSedang1->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule17 = new FuzzyRule(17, ifLurusAndDelta_Kanan_Tajam, thenRightPelanAndLeftSedang1);
    fuzzy->addFuzzyRule(fuzzyRule17);
  
    FuzzyRuleAntecedent *ifKananAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
    ifKananAndDelta_Kiri_Tajam->joinWithAND(Kanan, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang2 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang2->addOutput(RightPelan);
    thenRightPelanAndLeftSedang2->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule18 = new FuzzyRule(18, ifKananAndDelta_Kiri_Tajam, thenRightPelanAndLeftSedang2);
    fuzzy->addFuzzyRule(fuzzyRule18);
    
    FuzzyRuleAntecedent *ifKananAndDelta_Kiri= new FuzzyRuleAntecedent();
    ifKananAndDelta_Kiri->joinWithAND(Kanan, Delta_Kiri);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang3 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang3->addOutput(RightPelan);
    thenRightPelanAndLeftSedang3->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule19 = new FuzzyRule(19, ifKananAndDelta_Kiri, thenRightPelanAndLeftSedang3);
    fuzzy->addFuzzyRule(fuzzyRule19);
  
    FuzzyRuleAntecedent *ifKananAndDelta_Kanan= new FuzzyRuleAntecedent();
    ifKananAndDelta_Kanan->joinWithAND(Kanan, Delta_Kanan);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang4 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang4->addOutput(RightPelan);
    thenRightPelanAndLeftSedang4->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule20 = new FuzzyRule(20, ifKananAndDelta_Kanan, thenRightPelanAndLeftSedang4);
    fuzzy->addFuzzyRule(fuzzyRule20);
  
    FuzzyRuleAntecedent *ifKananAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
    ifKananAndDelta_Kanan_Tajam->joinWithAND(Kanan, Delta_Kanan_Tajam);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang5 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang5->addOutput(RightPelan);
    thenRightPelanAndLeftSedang5->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule21 = new FuzzyRule(21, ifKananAndDelta_Kanan_Tajam, thenRightPelanAndLeftSedang5);
    fuzzy->addFuzzyRule(fuzzyRule21);
  
    FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
    ifKanan_TajamAndDelta_Kiri_Tajam->joinWithAND(Kanan_Tajam, Delta_Kiri_Tajam);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang6 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang6->addOutput(RightPelan);
    thenRightPelanAndLeftSedang6->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule22 = new FuzzyRule(22, ifKanan_TajamAndDelta_Kiri_Tajam, thenRightPelanAndLeftSedang6);
    fuzzy->addFuzzyRule(fuzzyRule22);
  
    FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kiri = new FuzzyRuleAntecedent();
    ifKanan_TajamAndDelta_Kiri->joinWithAND(Kanan_Tajam, Delta_Kiri);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang7 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang7->addOutput(RightPelan);
    thenRightPelanAndLeftSedang7->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule23 = new FuzzyRule(23, ifKanan_TajamAndDelta_Kiri, thenRightPelanAndLeftSedang7);
    fuzzy->addFuzzyRule(fuzzyRule23);
  
    FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Lurus = new FuzzyRuleAntecedent();
    ifKanan_TajamAndDelta_Lurus->joinWithAND(Kanan_Tajam, Delta_Lurus);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang8 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang8->addOutput(RightPelan);
    thenRightPelanAndLeftSedang8->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule24 = new FuzzyRule(24, ifKanan_TajamAndDelta_Lurus, thenRightPelanAndLeftSedang8);
    fuzzy->addFuzzyRule(fuzzyRule24);
  
    FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kanan = new FuzzyRuleAntecedent();
    ifKanan_TajamAndDelta_Kanan->joinWithAND(Kanan_Tajam, Delta_Kanan);
    FuzzyRuleConsequent *thenRightPelanAndLeftSedang9 = new FuzzyRuleConsequent();
    thenRightPelanAndLeftSedang9->addOutput(RightPelan);
    thenRightPelanAndLeftSedang9->addOutput(LeftSedang);
    FuzzyRule *fuzzyRule25 = new FuzzyRule(25, ifKanan_TajamAndDelta_Kanan, thenRightPelanAndLeftSedang9);
    fuzzy->addFuzzyRule(fuzzyRule25);
}

void loop() {
  // Maju mundur
  ch_3 = IBus.readChannel(2);

  // Belok kanan kiri
  ch_1 = IBus.readChannel(0);

  // Auto manual
  ch_5 = IBus.readChannel(4);

  // Fan DC
  ch_6 = IBus.readChannel(5);

  // Pompa 1
  ch_7 = IBus.readChannel(6);

  // Pompa 2
  ch_8 = IBus.readChannel(7);

  motorSpeed = map(ch_3, 1000, 2000, -80, 80);
  motorTurn = map(ch_1, 1000, 2000, -80, 80);

  if (ch_5 > 1500) {
    digitalWrite(ledKuning, LOW);
    digitalWrite(ledHijau, HIGH);
    ch_1 = 1500;
    ch_3 = 1500;
    ch_5 = 1500;
    ch_6 = 1500;
    ch_7 = 1500;
    ch_8 = 1500;
//    otomatis();
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - previousTime;
  
    // Pastikan waktu yang telah berlalu mencapai sampleTime sebelum melakukan perhitungan PID
    if (elapsedTime >= sampleTime) {
      // Reset previousTime ke currentTime
      previousTime = currentTime;
  
      if (Serial.available() > 0) {
        receivedData = Serial.readStringUntil('\n');
  //      Serial.print("receivedData: ");
  //      Serial.println(receivedData);
        lastReceiveTime = millis();
  
        if (receivedData.startsWith("error_")) {
          String errorString = receivedData.substring(6); // Mengambil substring setelah "error_"
          float currentError = errorString.toFloat(); // Mengkonversi substring ke nilai float
    //      float deltaError = previousError - 0; // Menghitung delta_error dari nilai previousError dikurangi 0
          float deltaError = currentError - previousError; // Menghitung delta error dari error aktual - error sebelumnya
    
    
          Serial.print("error : ");
          Serial.println(currentError);
          Serial.print("delta error : ");
          Serial.println(deltaError);
    
          fuzzy->setInput(1, currentError); // Mengatur input fuzzy untuk error
          fuzzy->setInput(2, deltaError); // Mengatur input fuzzy untuk delta_error
      
          fuzzy->fuzzify(); // Melakukan fuzzifikasi
    
           // Mendefuzzifikasi output /////////////////////////////////////////////////////////////////////////////////////////////////////////////
          float output1 = fuzzy->defuzzify(1);
          float output2 = fuzzy->defuzzify(2);
    
          Serial.print("Output fuzzy1: ");
          Serial.println(output1);
          Serial.print("Output fuzzy2: ");
          Serial.println(output2);
    
          // Mengatur kecepatan motor berdasarkan hasil defuzzifikasi dan aturan fuzzy
          int pwmValueKanan = 0;
          int pwmValueKiri = 0;
    
           if (output1 >= 175 && output2 >= 50 && output2 <= 100) { // BelokKiriTajam
            pwmValueKanan = 60; // Kecepatan motor kanan tinggi
            pwmValueKiri = -10; // Kecepatan motor kiri sedang
            analogWrite(motorLeftPin1, 0);
            analogWrite(motorLeftPin2,  abs(pwmValueKiri));
            analogWrite(motorRightPin1, 0);
            analogWrite(motorRightPin2, abs(pwmValueKanan));
            
          } else if (output1 >= 100 && output1 <= 175 && output2 >=50 && output2 <= 100) { // BelokKiri
            pwmValueKanan = 50; // Kecepatan motor kanan sedang
            pwmValueKiri = 5; // Kecepatan motor kiri rendah
             analogWrite(motorLeftPin1, 0);
            analogWrite(motorLeftPin2,  abs(pwmValueKiri));
            analogWrite(motorRightPin1, 0);
            analogWrite(motorRightPin2, abs(pwmValueKanan));
          } else if (output1 >= 100 && output1 <= 175 && output2 >= 100 && output2 <= 175) { // MajuLurus
            pwmValueKanan = 40; // Kecepatan motor kanan sedang
            pwmValueKiri = 40; // Kecepatan motor kiri sedang
            analogWrite(motorLeftPin1,  abs(pwmValueKiri));
            analogWrite(motorLeftPin2, 0);
            analogWrite(motorRightPin1, abs(pwmValueKanan));
            analogWrite(motorRightPin2, 0);
           } else if (output1 >= 50 && output1 <= 100 && output2 >= 100 && output2 <= 175) { // BelokKanan
            pwmValueKanan = 5; // Kecepatan motor kanan rendah
            pwmValueKiri = 50; // Kecepatan motor kiri sedang
            analogWrite(motorLeftPin1,  abs(pwmValueKiri));
            analogWrite(motorLeftPin2, 0);
            analogWrite(motorRightPin1, 0);
            analogWrite(motorRightPin2, abs(pwmValueKanan));
           } else if (output1 >= 50 && output1 <= 100 && output2 >= 175) { // BelokKananTajam
            pwmValueKanan = -10; // Kecepatan motor kanan sedang
            pwmValueKiri = 60; // Kecepatan motor kiri tinggi
            analogWrite(motorLeftPin1,  abs(pwmValueKiri));
            analogWrite(motorLeftPin2, 0);
            analogWrite(motorRightPin1, 0);
            analogWrite(motorRightPin2, abs(pwmValueKanan));
          }
    
    
    
          // Menulis nilai PWM ke motor driver
    //      analogWrite(enA, pwmValueKanan);
    //      analogWrite(enB, pwmValueKiri);
    
          // Mengatur arah motor
    //      digitalWrite(in1, HIGH);
    //      digitalWrite(in2, LOW);
    //      digitalWrite(in3, HIGH);
    //      digitalWrite(in4, LOW);
    
          previousError = currentError; 
        }
      }
  
      // Stop motor jika tidak ada data yang diterima lagi
      if (millis() - lastReceiveTime > stopDuration && lastReceiveTime != 0) {
        stopp();
        delay(100);
      }
      
    }
  } 
  
  else {
    digitalWrite(ledKuning, HIGH);
    digitalWrite(ledHijau, LOW);
    if (motorTurn > 5) {
      kanan(motorTurn, motorTurn);
    } else if (motorTurn < -5) {
      kiri(motorTurn, motorTurn);
    } else if (motorSpeed < 5 && motorSpeed > -5 && motorTurn > -5 && motorTurn < 5) {
      stopp();
    } else if (motorSpeed > 5 && motorTurn > -5 && motorTurn < 5) {
      maju(motorSpeed, motorSpeed);
    } else if (motorSpeed < 5 && motorTurn > -5 && motorTurn < 5) {
      mundur(motorSpeed, motorSpeed);
    }

    if (ch_7 > 1500) {
      digitalWrite(relay1Pin, HIGH);
    } else {
      digitalWrite(relay1Pin, LOW);
    }

    if (ch_8 > 1500) {
      digitalWrite(relay2Pin, HIGH);
    } else {
      digitalWrite(relay2Pin, LOW);
    }

    if (ch_6 > 1500) {
      digitalWrite(relay3Pin, HIGH);
    } else {
      digitalWrite(relay3Pin, LOW);
    }
  }

  delay(100);
}

void maju(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftPin1, abs(leftSpeed));
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, abs(rightSpeed));
  analogWrite(motorRightPin2, 0);
}

void mundur(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, abs(leftSpeed));
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, abs(rightSpeed));
}

void kanan(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftPin1, abs(leftSpeed));
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, abs(rightSpeed));
}

void kiri(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, abs(leftSpeed));
  analogWrite(motorRightPin1, abs(rightSpeed));
  analogWrite(motorRightPin2, 0);
}

//void kananUJ() {
//  analogWrite(motorLeftPin1, 55);
//  analogWrite(motorLeftPin2, 0);
//  analogWrite(motorRightPin1, 0);
//  analogWrite(motorRightPin2, 55);
//}
//
//void kiriUJ() {
//  analogWrite(motorLeftPin1, 0);
//  analogWrite(motorLeftPin2, 55);
//  analogWrite(motorRightPin1, 55);
//  analogWrite(motorRightPin2, 0);
//}

void kananUJ() {
  analogWrite(motorLeftPin1, 55);
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, 28);
}

void kiriUJ() {
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, 28);
  analogWrite(motorRightPin1, 55);
  analogWrite(motorRightPin2, 0);
}

void majuU() {
  analogWrite(motorLeftPin1, 35);
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, 35);
  analogWrite(motorRightPin2, 0);
}

void stopp() {
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, 0);
}


void manual() {
  if (ch_5 < 1500) {
    stopp();
  }
}
