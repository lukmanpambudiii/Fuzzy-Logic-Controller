#include <Fuzzy.h>

#define enA 14
#define in1 27
#define in2 26
#define enB 12
#define in3 32
#define in4 33

int motorSpeedA = 0;
int motorSpeedB = 0;


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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Output membership function (kecepatan motor kanan)
//FuzzySet *RightStop = new FuzzySet(0, 0, 5, 10);
FuzzySet *RightPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *RightSedang = new FuzzySet(100, 115, 140, 175);
FuzzySet *RightCepat = new FuzzySet(175, 180, 205, 205);


//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *LeftSedang = new FuzzySet(100, 115, 140, 175);
FuzzySet *LeftCepat = new FuzzySet(175, 180, 205, 205);

float previousError = 0; // Menyimpan nilai error sebelumnya

void setup(){
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(115200);

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


void loop(){
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // Membaca data serial hingga newline (\n)
    
    if (data.startsWith("error_")) { // Memeriksa apakah data dimulai dengan "error_"
      String errorString = data.substring(6); // Mengambil substring setelah "error_"
      float currentError = errorString.toFloat(); // Mengkonversi substring ke nilai float
//      float deltaError = previousError - 0; // Menghitung delta_error dari nilai previousError dikurangi 0

      float deltaError = currentError - previousError; // Menghitung delta_error dari nilai previousError dikurangi error sebelumnya

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

      
//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
//FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
//FuzzySet *LeftSedang = new FuzzySet(90, 115, 140, 165);
//FuzzySet *LeftCepat = new FuzzySet(155, 180, 205, 205);

       if (output1 >= 155 && output2 >= 50 && output2 <= 100) { // BelokKiriTajam
        pwmValueKanan = 240; // Kecepatan motor kanan cepat
        pwmValueKiri = 100; // Kecepatan motor kiri pelan
      } else if (output1 >= 100 && output1 <= 175 && output2 >=50 && output2 <= 100) { // BelokKiri
        pwmValueKanan = 200; // Kecepatan motor kanan sedang
        pwmValueKiri = 100; // Kecepatan motor kiri pelan
      } else if (output1 >= 100 && output1 <= 175 && output2 >= 100 && output2 <= 175) { // MajuLurus
        pwmValueKanan = 200; // Kecepatan motor kanan sedang
        pwmValueKiri = 200; // Kecepatan motor kiri sedang
      } else if (output1 >= 50 && output1 <= 100 && output2 >= 100 && output2 <= 175) { // BelokKanan
        pwmValueKanan = 100; // Kecepatan motor kanan rendah
        pwmValueKiri = 200; // Kecepatan motor kiri sedang
      } else if (output1 >= 50 && output1 <= 100 && output2 >= 155) { // BelokKananTajam
        pwmValueKanan = 100; // Kecepatan motor kanan sedang
        pwmValueKiri = 240; // Kecepatan motor kiri tinggi
      }


      // Menulis nilai PWM ke motor driver
      analogWrite(enA, pwmValueKanan);
      analogWrite(enB, pwmValueKiri);

      // Mengatur arah motor
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);

      previousError = currentError; // Menyimpan currentError sebagai previousError untuk perhitungan berikutnya
    }
  }
}
