byte target;
byte base;
int numeroDado;
byte estadoJogo = 1;
bool andAtivo;
bool xorAtivo;
bool inicio = true;

byte AND = 4;
byte OR = 3;
byte XOR = 2;
byte botoes[3] = {AND, OR, XOR};
size_t quantidadeBotoes = sizeof(botoes)/sizeof(byte);

unsigned long ultimoClique[3] = {0, 0, 0};
unsigned long tempoDesdeDebounce[3] = {0, 0, 0};
byte debounceDelay = 50;
bool estadoBotao[3];
bool ultimoEstadoBotao[3] = {LOW, LOW, LOW};
bool estadoBotaoDebouncing[3] = {LOW, LOW, LOW};

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(A0));
    for (byte i = 0; i < quantidadeBotoes; i++) pinMode(botoes[i], INPUT_PULLUP);

    target = random(256);
    base = random(256);
    andAtivo = target & 0b10;
    xorAtivo = !andAtivo;

    delay(1500);
}

void loop() {
    for (byte i = 0; i < quantidadeBotoes; i++) debouncing(i);

    if (estadoJogo == 1) inicioRonda();
    else if (estadoJogo == 2) perguntarValor();
    else if (estadoJogo == 3) perguntarOperacao();
}

void debouncing(byte i) {
    estadoBotao[i] = !digitalRead(botoes[i]);

    if (estadoBotao[i] != ultimoEstadoBotao[i]) ultimoClique[i] = millis();
    ultimoEstadoBotao[i] = estadoBotao[i];
    tempoDesdeDebounce[i] = millis() - ultimoClique[i];

    if (millis() - ultimoClique[i] <= 50) return;
    if (estadoBotaoDebouncing[i] != estadoBotao[i]) estadoBotaoDebouncing[i] = estadoBotao[i];
}

void inicioRonda() {
    Serial.println();
    if (inicio) {
        Serial.println("-=-=-=[ NOVA RONDA ]=-=-=-");
        inicio = false;
    }
    printOperacoes();
    printValores();
    Serial.println("Intoduza um valor: ");
    estadoJogo++;
}

void printOperacoes() {
    Serial.print("Operacoes permitidas: ");
    Serial.print("OR (branco) | ");
    if (andAtivo) Serial.println("AND (vermelho)");
    else Serial.println("XOR (azul)");
}

void printValores() {
    Serial.print("Target: ");
    Serial.println(target, BIN);
    Serial.print("Valor base: ");
    Serial.println(base, BIN);
}

void perguntarValor() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        if (inputValido(input)) {
            Serial.print("Valor lido (BIN): ");
            Serial.println(input.toInt(), BIN);

            printOperacoes();
            Serial.println("Clique em um dos botoes para aplicar uma operacao.");
            estadoJogo++;
        } else Serial.println("Insire um numero: ");
    }
}

boolean inputValido(String input) {
    for (byte i = 0; i < input.length(); i++) {
        if (input[i] < '0' || input[i] > '9') return false;
    }
    if (input == '\r') return false;

    return true;
}

void perguntarOperacao() {
    delay(2000);
    estadoJogo = 1;
}