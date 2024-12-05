byte target = 0;
byte valorInicial = 0;
byte estadoJogo = 1;
bool andAtivo = true;
bool xorAtivo = false;

byte AND = 4;
byte OR = 3;
byte XOR = 2;
byte botoes[3] = {AND, OR, XOR};
size_t quantidadeBotoes = sizeof(botoes)/sizeof(byte);

unsigned long ultimoClique = 0;
unsigned long tempoDesdeDebounce = 0;
byte debounceDelay = 50;
bool estadoBotao[3];
bool ultimoEstadoBotao[3] = {LOW, LOW, LOW};
bool estadoBotaoDebouncing[3] = {LOW, LOW, LOW};

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(A0));
    for (byte i = 0; i < quantidadeBotoes; i++) pinMode(botoes[i], INPUT_PULLUP);
    delay(1500);
    definirValores();
}

void loop() {
    for (byte i = 0; i < quantidadeBotoes; i++) debouncing(i);

    if (estadoJogo == 1) {
        perguntarValor();
    }
}

void definirValores() {
    target = random(256);
    valorInicial = random(256);
    andAtivo = target & 0b10;
    xorAtivo = !andAtivo;

    Serial.println();
    Serial.print("Operacoes permitidas: ");
    Serial.print("OR (branco) | ");
    if (andAtivo) Serial.println("AND (vermelho)");
    else Serial.println("XOR (azul)");

    Serial.println("-=-=-=[ NOVA RONDA ]=-=-=-");
    Serial.print("Target: ");
    Serial.println(target, BIN);
    Serial.print("Valor inicial: ");
    Serial.println(valorInicial, BIN);
    Serial.println("Intoduza um valor: ");
}

void debouncing(byte i) {
    estadoBotao[i] = !digitalRead(botoes[i]);

    if (estadoBotao[i] != ultimoEstadoBotao[i]) ultimoClique = millis();
    ultimoEstadoBotao[i] = estadoBotao[i];
    tempoDesdeDebounce = millis() - ultimoClique;

    if (millis() - ultimoClique <= 50) return;
    if (estadoBotaoDebouncing[i] != estadoBotao[i]) estadoBotaoDebouncing[i] = estadoBotao[i];
}

boolean inputValido(String input) {
    for (byte i = 0; i < input.length(); i++) {
        if (input[i] < '0' || input[i] > '9') return false;
    }
    if (input == '\r') return false;

    return true;
}

void perguntarValor() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        if (inputValido(input)) {
            Serial.print("Valor lido (BIN): ");
            Serial.println(input.toInt(), BIN);
        } else Serial.println("Insire um número: ");
    }
}