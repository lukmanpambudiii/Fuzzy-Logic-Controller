#include <Fuzzy.h>
#include <IBusBM.h>
IBusBM IBus;

////Motor driver IBT
int motorRightPin1 = 12;
int motorRightPin2 = 13;
int motorLeftPin1 = 18;
int motorLeftPin2 = 19;

// Indikator mode manual
int ledKuning = 32;
// Indikator mode otomatis
int ledHijau = 33;

int motorSpeed;
int motorTurn;

// Channel FS
int ch_3, ch_1, ch_5, ch_6;


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
FuzzySet *RightPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *RightSedang = new FuzzySet(100, 125, 150, 175);
FuzzySet *RightCepat = new FuzzySet(175, 200, 225, 225);


//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *LeftSedang = new FuzzySet(100, 125, 150, 175);
FuzzySet *LeftCepat = new FuzzySet(175, 200, 225, 225);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float previousError = 0; // Menyimpan nilai error sebelumnya

void setup(){
//  pinMode(enA, OUTPUT);
//  pinMode(enB, OUTPUT);
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledHijau, OUTPUT);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, 0);
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, 0);
  digitalWrite(ledKuning, LOW);
  digitalWrite(ledHijau, LOW);

  Serial.begin(115200);
  IBus.begin(Serial2, 1);

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


  // Aturan fuzzy
  // Definisikan semua variabel fuzzy (misal, Kiri_Tajam, Delta_Kiri_Tajam, RightSedang, LeftPelan, dsb.)


// Definisikan semua variabel fuzzy (misal, Kiri_Tajam, Delta_Kiri_Tajam, RightSedang, LeftPelan, dsb.)

// Definisi FuzzyRuleAntecedent dan FuzzyRuleConsequent berdasarkan tabel
FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
ifKiri_TajamAndDelta_Kiri_Tajam->joinWithAND(Kiri_Tajam, Delta_Kiri_Tajam);
FuzzyRuleConsequent *thenRightCepatAndLeftPelan1 = new FuzzyRuleConsequent();
thenRightCepatAndLeftPelan1->addOutput(RightCepat);
thenRightCepatAndLeftPelan1->addOutput(LeftPelan);
FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifKiri_TajamAndDelta_Kiri_Tajam, thenRightCepatAndLeftPelan1);
fuzzy->addFuzzyRule(fuzzyRule1);

// Rule 2
FuzzyRuleAntecedent *ifKiriAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
ifKiriAndDelta_Kiri_Tajam->joinWithAND(Kiri, Delta_Kiri_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftPelan1 = new FuzzyRuleConsequent();
thenRightSedangAndLeftPelan1->addOutput(RightSedang);
thenRightSedangAndLeftPelan1->addOutput(LeftPelan);
FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifKiriAndDelta_Kiri_Tajam, thenRightSedangAndLeftPelan1);
fuzzy->addFuzzyRule(fuzzyRule2);

// Rule 3
FuzzyRuleAntecedent *ifLurusAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
ifLurusAndDelta_Kiri_Tajam->joinWithAND(Lurus, Delta_Kiri_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftPelan2 = new FuzzyRuleConsequent();
thenRightSedangAndLeftPelan2->addOutput(RightSedang);
thenRightSedangAndLeftPelan2->addOutput(LeftPelan);
FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifLurusAndDelta_Kiri_Tajam, thenRightSedangAndLeftPelan2);
fuzzy->addFuzzyRule(fuzzyRule3);

// Rule 4
FuzzyRuleAntecedent *ifKananAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
ifKananAndDelta_Kiri_Tajam->joinWithAND(Kanan, Delta_Kiri_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftPelan3 = new FuzzyRuleConsequent();
thenRightSedangAndLeftPelan3->addOutput(RightSedang);
thenRightSedangAndLeftPelan3->addOutput(LeftPelan);
FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifKananAndDelta_Kiri_Tajam, thenRightSedangAndLeftPelan3);
fuzzy->addFuzzyRule(fuzzyRule4);

// Rule 5
FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kiri_Tajam = new FuzzyRuleAntecedent();
ifKanan_TajamAndDelta_Kiri_Tajam->joinWithAND(Kanan_Tajam, Delta_Kiri_Tajam);
FuzzyRuleConsequent *thenRightPelanAndLeftPelan = new FuzzyRuleConsequent();
thenRightPelanAndLeftPelan->addOutput(RightPelan);
thenRightPelanAndLeftPelan->addOutput(LeftPelan);
FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifKanan_TajamAndDelta_Kiri_Tajam, thenRightPelanAndLeftPelan);
fuzzy->addFuzzyRule(fuzzyRule5);

// Rule 6
FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kiri = new FuzzyRuleAntecedent();
ifKiri_TajamAndDelta_Kiri->joinWithAND(Kiri_Tajam, Delta_Kiri);
FuzzyRuleConsequent *thenRightCepatAndLeftPelan2 = new FuzzyRuleConsequent();
thenRightCepatAndLeftPelan2->addOutput(RightCepat);
thenRightCepatAndLeftPelan2->addOutput(LeftPelan);
FuzzyRule *fuzzyRule6 = new FuzzyRule(6, ifKiri_TajamAndDelta_Kiri, thenRightCepatAndLeftPelan2);
fuzzy->addFuzzyRule(fuzzyRule6);

// Rule 7
FuzzyRuleAntecedent *ifKiriAndDelta_Kiri = new FuzzyRuleAntecedent();
ifKiriAndDelta_Kiri->joinWithAND(Kiri, Delta_Kiri);
FuzzyRuleConsequent *thenRightSedangAndLeftPelan4 = new FuzzyRuleConsequent();
thenRightSedangAndLeftPelan4->addOutput(RightSedang);
thenRightSedangAndLeftPelan4->addOutput(LeftPelan);
FuzzyRule *fuzzyRule7 = new FuzzyRule(7, ifKiriAndDelta_Kiri, thenRightSedangAndLeftPelan4);
fuzzy->addFuzzyRule(fuzzyRule7);

// Rule 8
FuzzyRuleAntecedent *ifLurusAndDelta_Kiri = new FuzzyRuleAntecedent();
ifLurusAndDelta_Kiri->joinWithAND(Lurus, Delta_Kiri);
FuzzyRuleConsequent *thenRightSedangAndLeftPelan5 = new FuzzyRuleConsequent();
thenRightSedangAndLeftPelan5->addOutput(RightSedang);
thenRightSedangAndLeftPelan5->addOutput(LeftPelan);
FuzzyRule *fuzzyRule8 = new FuzzyRule(8, ifLurusAndDelta_Kiri, thenRightSedangAndLeftPelan5);
fuzzy->addFuzzyRule(fuzzyRule8);

// Rule 9
FuzzyRuleAntecedent *ifKananAndDelta_Kiri = new FuzzyRuleAntecedent();
ifKananAndDelta_Kiri->joinWithAND(Kanan, Delta_Kiri);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang1 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang1->addOutput(RightSedang);
thenRightSedangAndLeftSedang1->addOutput(LeftSedang);
FuzzyRule *fuzzyRule9 = new FuzzyRule(9, ifKananAndDelta_Kiri, thenRightSedangAndLeftSedang1);
fuzzy->addFuzzyRule(fuzzyRule9);

// Rule 10
FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kiri = new FuzzyRuleAntecedent();
ifKanan_TajamAndDelta_Kiri->joinWithAND(Kanan_Tajam, Delta_Kiri);
FuzzyRuleConsequent *thenRightPelanAndLeftSedang1 = new FuzzyRuleConsequent();
thenRightPelanAndLeftSedang1->addOutput(RightPelan);
thenRightPelanAndLeftSedang1->addOutput(LeftSedang);
FuzzyRule *fuzzyRule10 = new FuzzyRule(10, ifKanan_TajamAndDelta_Kiri, thenRightPelanAndLeftSedang1);
fuzzy->addFuzzyRule(fuzzyRule10);

// Rule 11
FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Lurus = new FuzzyRuleAntecedent();
ifKiri_TajamAndDelta_Lurus->joinWithAND(Kiri_Tajam, Delta_Lurus);
FuzzyRuleConsequent *thenRightCepatAndLeftSedang1 = new FuzzyRuleConsequent();
thenRightCepatAndLeftSedang1->addOutput(RightCepat);
thenRightCepatAndLeftSedang1->addOutput(LeftSedang);
FuzzyRule *fuzzyRule11 = new FuzzyRule(11, ifKiri_TajamAndDelta_Lurus, thenRightCepatAndLeftSedang1);
fuzzy->addFuzzyRule(fuzzyRule11);

// Rule 12
FuzzyRuleAntecedent *ifKiriAndDelta_Lurus = new FuzzyRuleAntecedent();
ifKiriAndDelta_Lurus->joinWithAND(Kiri, Delta_Lurus);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang2 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang2->addOutput(RightSedang);
thenRightSedangAndLeftSedang2->addOutput(LeftSedang);
FuzzyRule *fuzzyRule12 = new FuzzyRule(12, ifKiriAndDelta_Lurus, thenRightSedangAndLeftSedang2);
fuzzy->addFuzzyRule(fuzzyRule12);

// Rule 13
FuzzyRuleAntecedent *ifLurusAndDelta_Lurus = new FuzzyRuleAntecedent();
ifLurusAndDelta_Lurus->joinWithAND(Lurus, Delta_Lurus);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang3 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang3->addOutput(RightSedang);
thenRightSedangAndLeftSedang3->addOutput(LeftSedang);
FuzzyRule *fuzzyRule13 = new FuzzyRule(13, ifLurusAndDelta_Lurus, thenRightSedangAndLeftSedang3);
fuzzy->addFuzzyRule(fuzzyRule13);

// Rule 14
FuzzyRuleAntecedent *ifKananAndDelta_Lurus = new FuzzyRuleAntecedent();
ifKananAndDelta_Lurus->joinWithAND(Kanan, Delta_Lurus);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang4 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang4->addOutput(RightSedang);
thenRightSedangAndLeftSedang4->addOutput(LeftSedang);
FuzzyRule *fuzzyRule14 = new FuzzyRule(14, ifKananAndDelta_Lurus, thenRightSedangAndLeftSedang4);
fuzzy->addFuzzyRule(fuzzyRule14);

// Rule 15
FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Lurus = new FuzzyRuleAntecedent();
ifKanan_TajamAndDelta_Lurus->joinWithAND(Kanan_Tajam, Delta_Lurus);
FuzzyRuleConsequent *thenRightPelanAndLeftSedang2 = new FuzzyRuleConsequent();
thenRightPelanAndLeftSedang2->addOutput(RightPelan);
thenRightPelanAndLeftSedang2->addOutput(LeftSedang);
FuzzyRule *fuzzyRule15 = new FuzzyRule(15, ifKanan_TajamAndDelta_Lurus, thenRightPelanAndLeftSedang2);
fuzzy->addFuzzyRule(fuzzyRule15);

// Rule 16
FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kanan = new FuzzyRuleAntecedent();
ifKiri_TajamAndDelta_Kanan->joinWithAND(Kiri_Tajam, Delta_Kanan);
FuzzyRuleConsequent *thenRightCepatAndLeftSedang2 = new FuzzyRuleConsequent();
thenRightCepatAndLeftSedang2->addOutput(RightCepat);
thenRightCepatAndLeftSedang2->addOutput(LeftSedang);
FuzzyRule *fuzzyRule16 = new FuzzyRule(16, ifKiri_TajamAndDelta_Kanan, thenRightCepatAndLeftSedang2);
fuzzy->addFuzzyRule(fuzzyRule16);

// Rule 17
FuzzyRuleAntecedent *ifKiriAndDelta_Kanan = new FuzzyRuleAntecedent();
ifKiriAndDelta_Kanan->joinWithAND(Kiri, Delta_Kanan);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang5 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang5->addOutput(RightSedang);
thenRightSedangAndLeftSedang5->addOutput(LeftSedang);
FuzzyRule *fuzzyRule17 = new FuzzyRule(17, ifKiriAndDelta_Kanan, thenRightSedangAndLeftSedang5);
fuzzy->addFuzzyRule(fuzzyRule17);

// Rule 18
FuzzyRuleAntecedent *ifLurusAndDelta_Kanan = new FuzzyRuleAntecedent();
ifLurusAndDelta_Kanan->joinWithAND(Lurus, Delta_Kanan);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang6 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang6->addOutput(RightSedang);
thenRightSedangAndLeftSedang6->addOutput(LeftSedang);
FuzzyRule *fuzzyRule18 = new FuzzyRule(18, ifLurusAndDelta_Kanan, thenRightSedangAndLeftSedang6);
fuzzy->addFuzzyRule(fuzzyRule18);

// Rule 19
FuzzyRuleAntecedent *ifKananAndDelta_Kanan = new FuzzyRuleAntecedent();
ifKananAndDelta_Kanan->joinWithAND(Kanan, Delta_Kanan);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang7 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang7->addOutput(RightSedang);
thenRightSedangAndLeftSedang7->addOutput(LeftSedang);
FuzzyRule *fuzzyRule19 = new FuzzyRule(19, ifKananAndDelta_Kanan, thenRightSedangAndLeftSedang7);
fuzzy->addFuzzyRule(fuzzyRule19);

// Rule 20
FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kanan = new FuzzyRuleAntecedent();
ifKanan_TajamAndDelta_Kanan->joinWithAND(Kanan_Tajam, Delta_Kanan);
FuzzyRuleConsequent *thenRightPelanAndLeftSedang3 = new FuzzyRuleConsequent();
thenRightPelanAndLeftSedang3->addOutput(RightPelan);
thenRightPelanAndLeftSedang3->addOutput(LeftSedang);
FuzzyRule *fuzzyRule20 = new FuzzyRule(20, ifKanan_TajamAndDelta_Kanan, thenRightPelanAndLeftSedang3);
fuzzy->addFuzzyRule(fuzzyRule20);

// Rule 21
FuzzyRuleAntecedent *ifKiri_TajamAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
ifKiri_TajamAndDelta_Kanan_Tajam->joinWithAND(Kiri_Tajam, Delta_Kanan_Tajam);
FuzzyRuleConsequent *thenRightCepatAndLeftPelan3 = new FuzzyRuleConsequent();
thenRightCepatAndLeftPelan3->addOutput(RightCepat);
thenRightCepatAndLeftPelan3->addOutput(LeftPelan);
FuzzyRule *fuzzyRule21 = new FuzzyRule(21, ifKiri_TajamAndDelta_Kanan_Tajam, thenRightCepatAndLeftPelan3);
fuzzy->addFuzzyRule(fuzzyRule21);

// Rule 22
FuzzyRuleAntecedent *ifKiriAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
ifKiriAndDelta_Kanan_Tajam->joinWithAND(Kiri, Delta_Kanan_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang8 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang8->addOutput(RightSedang);
thenRightSedangAndLeftSedang8->addOutput(LeftSedang);
FuzzyRule *fuzzyRule22 = new FuzzyRule(22, ifKiriAndDelta_Kanan_Tajam, thenRightSedangAndLeftSedang8);
fuzzy->addFuzzyRule(fuzzyRule22);

// Rule 23
FuzzyRuleAntecedent *ifLurusAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
ifLurusAndDelta_Kanan_Tajam->joinWithAND(Lurus, Delta_Kanan_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang9 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang9->addOutput(RightSedang);
thenRightSedangAndLeftSedang9->addOutput(LeftSedang);
FuzzyRule *fuzzyRule23 = new FuzzyRule(23, ifLurusAndDelta_Kanan_Tajam, thenRightSedangAndLeftSedang9);
fuzzy->addFuzzyRule(fuzzyRule23);

// Rule 24
FuzzyRuleAntecedent *ifKananAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
ifKananAndDelta_Kanan_Tajam->joinWithAND(Kanan, Delta_Kanan_Tajam);
FuzzyRuleConsequent *thenRightSedangAndLeftSedang10 = new FuzzyRuleConsequent();
thenRightSedangAndLeftSedang10->addOutput(RightSedang);
thenRightSedangAndLeftSedang10->addOutput(LeftSedang);
FuzzyRule *fuzzyRule24 = new FuzzyRule(24, ifKananAndDelta_Kanan_Tajam, thenRightSedangAndLeftSedang10);
fuzzy->addFuzzyRule(fuzzyRule24);

// Rule 25
FuzzyRuleAntecedent *ifKanan_TajamAndDelta_Kanan_Tajam = new FuzzyRuleAntecedent();
ifKanan_TajamAndDelta_Kanan_Tajam->joinWithAND(Kanan_Tajam, Delta_Kanan_Tajam);
FuzzyRuleConsequent *thenRightPelanAndLeftCepat = new FuzzyRuleConsequent();
thenRightPelanAndLeftCepat->addOutput(RightPelan);
thenRightPelanAndLeftCepat->addOutput(LeftCepat);
FuzzyRule *fuzzyRule25 = new FuzzyRule(25, ifKanan_TajamAndDelta_Kanan_Tajam, thenRightPelanAndLeftCepat);
fuzzy->addFuzzyRule(fuzzyRule25);

}

void loop(){
    // Maju mundur
  ch_3 = IBus.readChannel(2);

  // Belok kanan kiri
  ch_1 = IBus.readChannel(0);

  // Auto manual
  ch_5 = IBus.readChannel(4);

  // Fan DC
  ch_6 = IBus.readChannel(5);

  // Pompa 1
//  ch_7 = IBus.readChannel(6);

  // Pompa 2
//  ch_8 = IBus.readChannel(7);
 Serial.print(ch_5);
  motorSpeed = map(ch_3, 1000, 2000, -80, 80);
  motorTurn = map(ch_1, 1000, 2000, -80, 80);
  if (ch_5 > 1500) {
    digitalWrite(ledKuning, LOW);
    digitalWrite(ledHijau, HIGH);
    ch_1 = 1500;
    ch_3 = 1500;
    ch_5 = 1500;
    ch_6 = 1500;
//    ch_7 = 1500;
//    ch_8 = 1500;
    automatis();
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

//    if (ch_7 > 1500) {
//      digitalWrite(relay1Pin, HIGH);
//    } else {
//      digitalWrite(relay1Pin, LOW);
//    }

//    if (ch_8 > 1500) {
//      digitalWrite(relay2Pin, HIGH);
//    } else {
//      digitalWrite(relay2Pin, LOW);
//    }

//    if (ch_6 > 1500) {
//      digitalWrite(relay3Pin, HIGH);
//      } else {
//      digitalWrite(relay3Pin, LOW);
//      }
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

void stopp() {
  analogWrite(motorLeftPin1, 0);
  analogWrite(motorLeftPin2, 0);
  analogWrite(motorRightPin1, 0);
  analogWrite(motorRightPin2, 0);
}

void automatis() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // Membaca data serial hingga newline (\n)
        
        if (data.startsWith("error_")) { // Memeriksa apakah data dimulai dengan "error_"
            String errorString = data.substring(6); // Mengambil substring setelah "error_"
            float currentError = errorString.toFloat(); // Mengkonversi substring ke nilai float
            float deltaError = currentError - previousError; // Menghitung delta error dari error aktual - error sebelumnya

            Serial.print("error : ");
            Serial.println(currentError);
            Serial.print("delta error : ");
            Serial.println(deltaError);

            fuzzy->setInput(1, currentError); // Mengatur input fuzzy untuk error
            fuzzy->setInput(2, deltaError); // Mengatur input fuzzy untuk delta_error
        
            fuzzy->fuzzify(); // Melakukan fuzzifikasi

            // Mendefuzzifikasi output
            float output1 = fuzzy->defuzzify(1);
            float output2 = fuzzy->defuzzify(2);

            Serial.print("Output fuzzy1: ");
            Serial.println(output1);
            Serial.print("Output fuzzy2: ");
            Serial.println(output2);

            // Mengatur kecepatan motor berdasarkan hasil defuzzifikasi dan aturan fuzzy
            int pwmValueKanan = 0;
            int pwmValueKiri = 0;

            if (output1 >= 125 && output1 <= 200 && output2 <= 50) { // BelokKiriTajam
                pwmValueKanan = 60; // Kecepatan motor kanan tinggi
                pwmValueKiri = -10; // Kecepatan motor kiri sedang
                analogWrite(motorLeftPin1, 0);
                analogWrite(motorLeftPin2, abs(pwmValueKiri));
                analogWrite(motorRightPin1, 0);
                analogWrite(motorRightPin2, abs(pwmValueKanan));
            } else if (output1 >= 75 && output1 < 125 && output2 > 50 && output2 <= 100) { // BelokKiri
                pwmValueKanan = 50; // Kecepatan motor kanan sedang
                pwmValueKiri = 5; // Kecepatan motor kiri rendah
                analogWrite(motorLeftPin1, 0);
                analogWrite(motorLeftPin2, abs(pwmValueKiri));
                analogWrite(motorRightPin1, 0);
                analogWrite(motorRightPin2, abs(pwmValueKanan));
            } else if (output1 >= 50 && output1 < 75 && output2 > 50 && output2 <= 100) { // MajuLurus
                pwmValueKanan = 40; // Kecepatan motor kanan sedang
                pwmValueKiri = 40; // Kecepatan motor kiri sedang
                analogWrite(motorLeftPin1, abs(pwmValueKiri));
                analogWrite(motorLeftPin2, 0);
                analogWrite(motorRightPin1, abs(pwmValueKanan));
                analogWrite(motorRightPin2, 0);
            } else if (output1 >= 50 && output1 < 75 && output2 > 100 && output2 <= 150) { // BelokKanan
                pwmValueKanan = 5; // Kecepatan motor kanan rendah
                pwmValueKiri = 50; // Kecepatan motor kiri sedang
                analogWrite(motorLeftPin1, abs(pwmValueKiri));
                analogWrite(motorLeftPin2, 0);
                analogWrite(motorRightPin1, 0);
                analogWrite(motorRightPin2, abs(pwmValueKanan));
            } else if (output1 >= 50 && output1 < 75 && output2 > 150 && output2 <= 200) { // BelokKananTajam
                pwmValueKanan = -10; // Kecepatan motor kanan sedang
                pwmValueKiri = 60; // Kecepatan motor kiri tinggi
                analogWrite(motorLeftPin1, abs(pwmValueKiri));
                analogWrite(motorLeftPin2, 0);
                analogWrite(motorRightPin1, 0);
                analogWrite(motorRightPin2, abs(pwmValueKanan));
            } else {
                // Jika output berada di luar rentang yang didefinisikan, set ke kecepatan default (0)
                pwmValueKanan = 0;
                pwmValueKiri = 0;
                analogWrite(motorLeftPin1, 0);
                analogWrite(motorLeftPin2, 0);
                analogWrite(motorRightPin1, 0);
                analogWrite(motorRightPin2, 0);
            }

            previousError = currentError; // Menyimpan currentError sebagai previousError untuk perhitungan berikutnya
        }
    }
}

void manual() {
  if (ch_5 < 1500) {
    stopp();
  }
}
