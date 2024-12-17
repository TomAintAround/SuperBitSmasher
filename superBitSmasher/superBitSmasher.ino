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

int tempoLimiteSegundos = 30;
byte leds[4] = {11, 10, 9, 8};
size_t quantidadeLeds = sizeof(leds)/sizeof(byte);
unsigned long contador = 0;
unsigned long redutor = 0;

unsigned long ultimoClique[3] = {0, 0, 0};
unsigned long tempoDesdeClique[3] = {0, 0, 0};
bool estadoBotao[3];
bool ultimoEstadoBotao[3] = {LOW, LOW, LOW};
bool estadoBotaoDebouncing[3] = {LOW, LOW, LOW};

void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(A0)); // É necessário para haver aleatoriadade
	for (byte i = 0; i < quantidadeBotoes; i++) pinMode(botoes[i], INPUT_PULLUP);
	for (byte i = 0; i < quantidadeLeds; i++) pinMode(leds[i], INPUT_PULLUP);
	delay(2000); // O serial monitor mostra coisas estranhas se não dar um pouco de tempo antes de começar o estadoJogo
	definirValores();
}

void loop() {
	for (byte i = 0; i < quantidadeBotoes; i++) debouncing(i); // Ler os estados dos botões
	timer();

	// Caso o botão OR seja premido durante 2 segundos
	if (estadoBotaoDebouncing[1] && tempoDesdeClique[1] == 2000) {
		Serial.println("Iniciando uma nova ronda");
		resetTimer();
		inicio = true;
		definirValores();
		estadoJogo = 1;
	}

	if (estadoJogo == 1) inicioRonda();
	else if (estadoJogo == 2) perguntarValor();
	else if (estadoJogo == 3) perguntarOperacao();
}

void definirValores() {
	target = random(256);
	base = random(256);

	// AND fica ativo quando o bit 1 estiver aceso, e o XOR é o contrário
	andAtivo = target & 0b10;
	xorAtivo = !andAtivo;
}

void debouncing(byte i) {
	estadoBotao[i] = !digitalRead(botoes[i]);

	if (estadoBotao[i] != ultimoEstadoBotao[i]) ultimoClique[i] = millis();
	ultimoEstadoBotao[i] = estadoBotao[i];
	tempoDesdeClique[i] = millis() - ultimoClique[i];

	// Caso não passem 50 milissegundos, não irá atualizar o estado do botão
	if (tempoDesdeClique[i] <= 50) return;
	if (estadoBotaoDebouncing[i] != estadoBotao[i]) estadoBotaoDebouncing[i] = estadoBotao[i];
}

void timer() {
	unsigned long contadorReferencia = millis() - redutor;
	if (contadorReferencia - contador * 1000 >= 1000) contador++;
	
	// Ligar os LEDS
	for (byte mul = 1; mul <= quantidadeLeds; mul++) {
		if (contador >= mul * tempoLimiteSegundos / 4) digitalWrite(leds[mul - 1], HIGH);
		else digitalWrite(leds[mul - 1], LOW);
	}
	
	// Em caso de derrota
	if (contador >= tempoLimiteSegundos) {
		Serial.println("O tempo acabou!");
		resetTimer();
		estadoJogo = 1;
		inicio = true;
		definirValores();
	}
}

void resetTimer() {
	redutor += contador * 1000; // O quanto irá reduzir ao millis() para poder comparar o tempo depois
	contador = 0;
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
			numeroDado = input.toInt();
			Serial.print("Valor lido (BIN): ");
			Serial.println(numeroDado, BIN);

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
	if (input == '\r') return false; // caso o input seja literalmente nada

	return true;
}

void perguntarOperacao() {
	boolean botaoPremido = false;
	for (byte i = 0; i < quantidadeBotoes; i++) {
		//Caso o botão seja premido apenas rapidamente
		if (estadoBotaoDebouncing[i] && tempoDesdeClique[i] <= 1) botaoPremido = verificarBotao(i);
	}

	if (botaoPremido) {
		if (base == target) {
			Serial.println("Parabéns! Venceste!");
			definirValores();
			inicio = true;
			resetTimer();
		}
		estadoJogo = 1;
	}
}

boolean verificarBotao(byte i) {
	if (botoes[i] == OR) {
		base = base | numeroDado;
		return true;
	} else if (botoes[i] == AND && andAtivo) {
		base = base & numeroDado;
		return true;
	} else if (botoes[i] == XOR && xorAtivo) {
		base = base ^ numeroDado;
		return true;
	}

	Serial.println("Insire um numero valido.");
	return false;
}
