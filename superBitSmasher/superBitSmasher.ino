byte target = 0;
byte valorInicial = 0;
byte estadoJogo = 1;

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(A0));
    definirValores();
}

void loop() {
    if (estadoJogo == 1) {
        perguntarValor();
    }
}

void definirValores() {
    target = random(256);
    valorInicial = random(256);
    Serial.print("Target: ");
    Serial.println(target, BIN);
    Serial.print("Valor inicial: ");
    Serial.println(valorInicial, BIN);
}

void perguntarValor() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
    }
}

boolean inputValido(String input) {
    for (byte i = 0; i < input.length(); i++) {
        if (input[i] < '0' || input[i] > '0') return true;
    }
        
    if (input.toInt() >= 0 && input.toInt() <= 255) return false;
    return false;
}
