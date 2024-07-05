
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
