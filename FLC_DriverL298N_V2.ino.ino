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
FuzzySet *RightSedang = new FuzzySet(90, 115, 140, 165);
FuzzySet *RightCepat = new FuzzySet(155, 180, 205, 205);


//// Output membership function (kecepatan motor kiri)
//FuzzySet *LeftStop = new FuzzySet(0, 0, 0.5, 1);
FuzzySet *LeftPelan = new FuzzySet(50, 50, 75, 100);
FuzzySet *LeftSedang = new FuzzySet(90, 115, 140, 165);
FuzzySet *LeftCepat = new FuzzySet(155, 180, 205, 205);

float previousError = 0; // Menyimpan nilai error sebelumnya

void addFuzzyRule(int ruleNumber, FuzzySet* input1, FuzzySet* input2, FuzzySet* output1, FuzzySet* output2) {
    FuzzyRuleAntecedent *antecedent = new FuzzyRuleAntecedent();
    antecedent->joinWithAND(input1, input2);
    
    FuzzyRuleConsequent *consequent = new FuzzyRuleConsequent();
    consequent->addOutput(output1);
    consequent->addOutput(output2);
    
    FuzzyRule *fuzzyRule = new FuzzyRule(ruleNumber, antecedent, consequent);
    fuzzy->addFuzzyRule(fuzzyRule);
}

void setup() {
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
    FuzzyOutput *MotorKanan = new FuzzyOutput(1);
    MotorKanan->addFuzzySet(RightPelan);
    MotorKanan->addFuzzySet(RightSedang);
    MotorKanan->addFuzzySet(RightCepat);
    fuzzy->addFuzzyOutput(MotorKanan);

    FuzzyOutput *MotorKiri = new FuzzyOutput(2);
    MotorKiri->addFuzzySet(LeftPelan);
    MotorKiri->addFuzzySet(LeftSedang);
    MotorKiri->addFuzzySet(LeftCepat);
    fuzzy->addFuzzyOutput(MotorKiri);

    // Menambahkan aturan fuzzy menggunakan fungsi
    addFuzzyRule(1, Kiri_Tajam, Delta_Kiri_Tajam, RightCepat, LeftPelan); //Kiri_Tajam
    addFuzzyRule(2, Kanan_Tajam, Delta_Kanan_Tajam, RightPelan, LeftCepat); //Kanan_Tajam
    //////////////// Lurus
    addFuzzyRule(3, Kiri, Delta_Kanan_Tajam, RightSedang, LeftSedang); 
    addFuzzyRule(4, Lurus, Delta_Kiri, RightSedang, LeftSedang);
    addFuzzyRule(5, Lurus, Delta_Lurus, RightSedang, LeftSedang);
    addFuzzyRule(6, Lurus, Delta_Kanan, RightSedang, LeftSedang);
    addFuzzyRule(7, Kanan, Delta_Kiri_Tajam, RightSedang, LeftSedang);

    //////////////// Kiri
    addFuzzyRule(8, Kiri_Tajam, Delta_Kiri, RightSedang, LeftPelan);
    addFuzzyRule(9, Kiri_Tajam, Delta_Lurus, RightSedang, LeftPelan); 
    addFuzzyRule(10, Kiri_Tajam, Delta_Kanan, RightSedang, LeftPelan);
    addFuzzyRule(11, Kiri_Tajam, Delta_Kanan_Tajam, RightSedang, LeftPelan);
    addFuzzyRule(12, Kiri, Delta_Kiri_Tajam, RightSedang, LeftPelan);
    addFuzzyRule(13, Kiri, Delta_Kiri, RightSedang, LeftPelan);
    addFuzzyRule(14, Kiri, Delta_Lurus, RightSedang, LeftPelan);
    addFuzzyRule(15, Kiri, Delta_Kanan_Tajam, RightSedang, LeftPelan);
    addFuzzyRule(16, Lurus, Delta_Kiri_Tajam, RightSedang, LeftPelan);

    //////////////// Kanan
    addFuzzyRule(17, Lurus, Delta_Kanan_Tajam, RightSedang, LeftSedang);
    addFuzzyRule(18, Kanan, Delta_Kiri, RightSedang, LeftSedang);
    addFuzzyRule(19, Kanan, Delta_Lurus, RightSedang, LeftSedang);
    addFuzzyRule(20, Kanan, Delta_Kanan, RightSedang, LeftSedang);
    addFuzzyRule(21, Kanan, Delta_Kanan_Tajam, RightSedang, LeftSedang);
    addFuzzyRule(22, Kanan_Tajam, Delta_Kiri_Tajam, RightSedang, LeftSedang);
    addFuzzyRule(23, Kanan_Tajam, Delta_Kiri, RightSedang, LeftSedang);
    addFuzzyRule(24, Kanan_Tajam, Delta_Lurus, RightSedang, LeftSedang);
    addFuzzyRule(25, Kanan_Tajam, Delta_Kanan, RightSedang, LeftSedang);
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
      } else if (output1 >= 90 && output1 <= 165 && output2 >=50 && output2 <= 100) { // BelokKiri
        pwmValueKanan = 200; // Kecepatan motor kanan sedang
        pwmValueKiri = 100; // Kecepatan motor kiri pelan
      } else if (output1 >= 90 && output1 <= 165 && output2 >= 90 && output2 <= 165) { // MajuLurus
        pwmValueKanan = 200; // Kecepatan motor kanan sedang
        pwmValueKiri = 200; // Kecepatan motor kiri sedang
      } else if (output1 >= 50 && output1 <= 100 && output2 >= 90 && output2 <= 165) { // BelokKanan
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
