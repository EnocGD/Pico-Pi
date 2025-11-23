// ============================================
// SISTEMA DE APRENDIZAJE ARDUINO NANO - ESTRUCTURA MODULAR
// Sistema Educativo con 5 Prácticas Independientes
// ============================================

#include <Servo.h>

// ============================================
// DEFINICIONES DE PINES
// ============================================
const int SHARED_D3 = 3, SHARED_D4 = 4, SHARED_D5 = 5, SHARED_D6 = 6;
const int SHARED_D7 = 7, SHARED_D8 = 8, SHARED_D9 = 9, SHARED_D10 = 10;
const int SHARED_D11 = 11, SHARED_D12 = 12, SHARED_A0 = A0, SHARED_A1 = A1;
const int POTENCIOMETRO = A2, LED_ESTADO = 13;

// ============================================
// MAPEO DE PINES RGB - CORREGIDO
// ============================================
const int PIN_RGB_ROJO = SHARED_D11;    // Pin para canal rojo
const int PIN_RGB_VERDE = SHARED_D10;  // Pin para canal verde
const int PIN_RGB_AZUL = SHARED_D09;   // Pin para canal azul

// ============================================
// CONSTANTES
// ============================================
const PROGMEM byte patronesDigitos[] = {
  0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
  0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110
};

// ============================================
// VARIABLES DE ESTADO DEL SISTEMA
// ============================================
enum EstadoSistema {
  MENU_PRINCIPAL,
  PRACTICA_1,
  PRACTICA_2,
  PRACTICA_3,
  PRACTICA_4,
  PRACTICA_5
};

EstadoSistema estadoActual = MENU_PRINCIPAL;
bool practicaActiva = false;
String bufferEntrada = "";

Servo miServo;

// ============================================
// ESTRUCTURAS DE ESTADO DE PRÁCTICAS
// ============================================
struct EstadoPractica1 {
  bool estadoLedRojo = false;
  bool estadoLedVerde = false;
  bool ultimoEstadoBoton = false;
  unsigned long ultimoTiempoParpadeo = 0;
  int intervaloParpadeo = 500;
} estadoP1;

struct EstadoPractica2 {
  int brillo = 0;
  int ultimoValorPot = -1;
} estadoP2;

struct EstadoPractica3 {
  int posicionServo = 90;
  int posicionObjetivoServo = 90;
  unsigned long ultimaActualizacionServo = 0;
  int brillo = 0;
} estadoP3;

struct EstadoPractica4 {
  int valorRojo = 0;
  int valorVerde = 0;
  int valorAzul = 0;
  int pasoColor = 0;
  int modoColor = 0;
  unsigned long ultimaActualizacionColor = 0;
} estadoP4;

struct EstadoPractica5 {
  int numeroDisplay = 1234;
  int digitoActual = 0;
  unsigned long ultimaActualizacionDisplay = 0;
  bool mostrarCerosIzquierda = false;
  bool displayActivo = true;  // NUEVO: Control de activación del display
} estadoP5;

// ============================================
// FUNCIÓN SETUP
// ============================================
void setup() {
  Serial.begin(9600);
  pinMode(LED_ESTADO, OUTPUT);
  digitalWrite(LED_ESTADO, HIGH);
  
  delay(2000);
  mostrarMensajeBienvenida();
  mostrarMenuPrincipal();
}

// ============================================
// BUCLE PRINCIPAL
// ============================================
void loop() {
  manejarEntradaSerial();
  
  switch (estadoActual) {
    case MENU_PRINCIPAL:
      manejarMenuPrincipal();
      break;
    case PRACTICA_1:
      ejecutarPractica1();
      break;
    case PRACTICA_2:
      ejecutarPractica2();
      break;
    case PRACTICA_3:
      ejecutarPractica3();
      break;
    case PRACTICA_4:
      ejecutarPractica4();
      break;
    case PRACTICA_5:
      ejecutarPractica5();
      break;
  }
  
  actualizarLedEstado();
}

// ============================================
// MANEJO DE ENTRADA
// ============================================
void manejarEntradaSerial() {
  while (Serial.available()) {
    char charEntrada = Serial.read();
    
    if (charEntrada == '\n' || charEntrada == '\r') {
      if (bufferEntrada.length() > 0) {
        procesarEntrada(bufferEntrada);
        bufferEntrada = "";
      }
    } else if (charEntrada >= ' ') {
      bufferEntrada += charEntrada;
      Serial.print(charEntrada);
    }
  }
}

void procesarEntrada(String entrada) {
  entrada.trim();
  entrada.toLowerCase();
  
  switch (estadoActual) {
    case MENU_PRINCIPAL:
      manejarEntradaMenuPrincipal(entrada);
      break;
    case PRACTICA_1:
      manejarEntradaPractica1(entrada);
      break;
    case PRACTICA_2:
      manejarEntradaPractica2(entrada);
      break;
    case PRACTICA_3:
      manejarEntradaPractica3(entrada);
      break;
    case PRACTICA_4:
      manejarEntradaPractica4(entrada);
      break;
    case PRACTICA_5:
      manejarEntradaPractica5(entrada);
      break;
  }
}

// ============================================
// BIENVENIDA Y MENÚ PRINCIPAL
// ============================================
void mostrarMensajeBienvenida() {
  Serial.println(F(""));
  Serial.println(F("========================================"));
  Serial.println(F("   SISTEMA DE APRENDIZAJE ARDUINO NANO"));
  Serial.println(F("      Suite de Prácticas Educativas"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Aprende fundamentos de Arduino a través"));
  Serial.println(F("de 5 prácticas interactivas con guía"));
  Serial.println(F("paso a paso y comandos interactivos."));
  Serial.println(F(""));
}

void mostrarMenuPrincipal() {
  Serial.println(F(""));
  Serial.println(F("========== MENÚ PRINCIPAL =========="));
  Serial.println(F(""));
  Serial.println(F("Prácticas Disponibles:"));
  Serial.println(F(""));
  Serial.println(F("1 - Control GPIO Digital"));
  Serial.println(F("    LEDs, botones, E/S básica"));
  Serial.println(F("    Dificultad: Principiante"));
  Serial.println(F(""));
  Serial.println(F("2 - Procesamiento de Entrada Analógica"));
  Serial.println(F("    Potenciómetro, ADC, PWM"));
  Serial.println(F("    Dificultad: Principiante"));
  Serial.println(F(""));
  Serial.println(F("3 - Control PWM y Servo"));
  Serial.println(F("    Servomotores, control suave"));
  Serial.println(F("    Dificultad: Intermedio"));
  Serial.println(F(""));
  Serial.println(F("4 - Mezcla de Colores LED RGB"));
  Serial.println(F("    Teoría del color, animaciones"));
  Serial.println(F("    Dificultad: Intermedio"));
  Serial.println(F(""));
  Serial.println(F("5 - Display de 7 Segmentos"));
  Serial.println(F("    Multiplexado, temporización"));
  Serial.println(F("    Dificultad: Avanzado"));
  Serial.println(F(""));
  Serial.println(F("info - Información del sistema"));
  Serial.println(F(""));
  Serial.println(F("==================================="));
  Serial.print(F("Ingresa tu opción: "));
}

void manejarMenuPrincipal() {
  digitalWrite(LED_ESTADO, (millis() / 1000) % 2);
}

void manejarEntradaMenuPrincipal(String entrada) {
  if (entrada == "1") {
    iniciarPractica1();
  } else if (entrada == "2") {
    iniciarPractica2();
  } else if (entrada == "3") {
    iniciarPractica3();
  } else if (entrada == "4") {
    iniciarPractica4();
  } else if (entrada == "5") {
    iniciarPractica5();
  } else if (entrada == "info") {
    mostrarInfoSistema();
    mostrarMenuPrincipal();
  } else {
    Serial.println();
    Serial.println(F("Selección inválida. Por favor ingresa 1-5 o 'info'"));
    Serial.print(F("Ingresa tu opción: "));
  }
}

// ============================================
// PRÁCTICA 1: CONTROL GPIO DIGITAL
// ============================================
void iniciarPractica1() {
  estadoActual = PRACTICA_1;
  practicaActiva = true;
  
  estadoP1.estadoLedRojo = false;
  estadoP1.estadoLedVerde = false;
  estadoP1.ultimoEstadoBoton = false;
  estadoP1.ultimoTiempoParpadeo = 0;
  estadoP1.intervaloParpadeo = 500;
  
  apagarTodasLasSalidas();
  pinMode(SHARED_D3, OUTPUT);
  pinMode(SHARED_D4, OUTPUT);
  pinMode(SHARED_D5, INPUT);
  
  mostrarInfoPractica1();
  mostrarMenuPractica1();
}

void mostrarInfoPractica1() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("   PRÁCTICA 1: CONTROL GPIO DIGITAL"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Configuración de Hardware:"));
  Serial.println(F("- LED Rojo: D3 -> 220ohm -> LED+ -> LED- -> GND"));
  Serial.println(F("- LED Verde: D4 -> 220ohm -> LED+ -> LED- -> GND"));
  Serial.println(F("- Botón: D5 <- Botón <- 10kohm <- GND"));
  Serial.println(F("         Terminal del botón -> 5V"));
  Serial.println(F(""));
  Serial.println(F("Objetivos de Aprendizaje:"));
  Serial.println(F("- Control de E/S digital"));
  Serial.println(F("- Control de LED con digitalWrite()"));
  Serial.println(F("- Lectura de botón con digitalRead()"));
  Serial.println(F("- Temporización no bloqueante"));
  Serial.println(F(""));
}

void mostrarMenuPractica1() {
  Serial.println(F("---------- Comandos Práctica 1 ----------"));
  Serial.println(F("rapido   - Parpadeo rápido (100ms)"));
  Serial.println(F("normal   - Parpadeo normal (500ms)"));
  Serial.println(F("lento    - Parpadeo lento (1000ms)"));
  Serial.println(F("estado   - Mostrar estado actual"));
  Serial.println(F("ayuda    - Mostrar este menú"));
  Serial.println(F("salir    - Volver al menú principal"));
  Serial.println(F("------------------------------------------"));
  Serial.println(F(""));
  Serial.println(F("El LED rojo parpadeará automáticamente."));
  Serial.println(F("Presiona el botón para alternar LED verde."));
  Serial.println(F(""));
  Serial.print(F("Práctica 1> "));
}

void ejecutarPractica1() {
  if (!practicaActiva) return;
  
  unsigned long tiempoActual = millis();
  
  if (tiempoActual - estadoP1.ultimoTiempoParpadeo >= estadoP1.intervaloParpadeo) {
    estadoP1.estadoLedRojo = !estadoP1.estadoLedRojo;
    digitalWrite(SHARED_D3, estadoP1.estadoLedRojo);
    estadoP1.ultimoTiempoParpadeo = tiempoActual;
  }
  
  bool estadoBotonActual = digitalRead(SHARED_D5);
  if (estadoBotonActual && !estadoP1.ultimoEstadoBoton) {
    estadoP1.estadoLedVerde = !estadoP1.estadoLedVerde;
    digitalWrite(SHARED_D4, estadoP1.estadoLedVerde);
    
    Serial.println();
    Serial.print(F("¡Botón presionado! LED Verde: "));
    Serial.println(estadoP1.estadoLedVerde ? F("ENCENDIDO") : F("APAGADO"));
    Serial.print(F("Práctica 1> "));
  }
  estadoP1.ultimoEstadoBoton = estadoBotonActual;
}

void manejarEntradaPractica1(String entrada) {
  if (entrada == "salir") {
    salirPracticaActual();
    return;
  } else if (entrada == "rapido") {
    estadoP1.intervaloParpadeo = 100;
    Serial.println(F("Velocidad de parpadeo: RÁPIDO (100ms)"));
  } else if (entrada == "normal") {
    estadoP1.intervaloParpadeo = 500;
    Serial.println(F("Velocidad de parpadeo: NORMAL (500ms)"));
  } else if (entrada == "lento") {
    estadoP1.intervaloParpadeo = 1000;
    Serial.println(F("Velocidad de parpadeo: LENTO (1000ms)"));
  } else if (entrada == "estado") {
    mostrarEstadoPractica1();
  } else if (entrada == "ayuda") {
    mostrarMenuPractica1();
    return;
  } else {
    Serial.println(F("Comando desconocido. Escribe 'ayuda' para comandos disponibles."));
  }
  
  Serial.print(F("Práctica 1> "));
}

void mostrarEstadoPractica1() {
  Serial.println();
  Serial.println(F("--- Estado Práctica 1 ---"));
  Serial.print(F("LED Rojo: "));
  Serial.println(estadoP1.estadoLedRojo ? F("ENCENDIDO") : F("APAGADO"));
  Serial.print(F("LED Verde: "));
  Serial.println(estadoP1.estadoLedVerde ? F("ENCENDIDO") : F("APAGADO"));
  Serial.print(F("Intervalo parpadeo: "));
  Serial.print(estadoP1.intervaloParpadeo);
  Serial.println(F("ms"));
  Serial.print(F("Estado botón: "));
  Serial.println(digitalRead(SHARED_D5) ? F("PRESIONADO") : F("LIBERADO"));
  Serial.println(F("-------------------------"));
}

// ============================================
// PRÁCTICA 2: PROCESAMIENTO ENTRADA ANALÓGICA
// ============================================
void iniciarPractica2() {
  estadoActual = PRACTICA_2;
  practicaActiva = true;
  
  estadoP2.brillo = 0;
  estadoP2.ultimoValorPot = -1;
  
  apagarTodasLasSalidas();
  pinMode(SHARED_D6, OUTPUT);
  
  mostrarInfoPractica2();
  mostrarMenuPractica2();
}

void mostrarInfoPractica2() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  PRÁCTICA 2: ENTRADA ANALÓGICA"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Configuración de Hardware:"));
  Serial.println(F("- Potenciómetro: Terminal1 -> 5V"));
  Serial.println(F("                 Centro -> A2"));
  Serial.println(F("                 Terminal3 -> GND"));
  Serial.println(F("- LED: D6 -> 220ohm -> LED+ -> LED- -> GND"));
  Serial.println(F(""));
  Serial.println(F("Objetivos de Aprendizaje:"));
  Serial.println(F("- Conversión Analógica-Digital (ADC)"));
  Serial.println(F("- Lectura de valores de potenciómetro"));
  Serial.println(F("- Salida PWM para brillo de LED"));
  Serial.println(F("- Mapeo y escalado de valores"));
  Serial.println(F(""));
}

void mostrarMenuPractica2() {
  Serial.println(F("---------- Comandos Práctica 2 ----------"));
  Serial.println(F("estado   - Mostrar lecturas actuales"));
  Serial.println(F("ayuda    - Mostrar este menú"));
  Serial.println(F("salir    - Volver al menú principal"));
  Serial.println(F("------------------------------------------"));
  Serial.println(F(""));
  Serial.println(F("Gira el potenciómetro para controlar brillo LED."));
  Serial.println(F("Observa la salida serial para valores en tiempo real."));
  Serial.println(F(""));
  Serial.print(F("Práctica 2> "));
}

void ejecutarPractica2() {
  if (!practicaActiva) return;
  
  int valorPot = analogRead(POTENCIOMETRO);
  estadoP2.brillo = map(valorPot, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, estadoP2.brillo);
  
  if (abs(valorPot - estadoP2.ultimoValorPot) > 10) {
    Serial.println();
    Serial.print(F("Potenciómetro: "));
    Serial.print(valorPot);
    Serial.print(F(" | PWM: "));
    Serial.print(estadoP2.brillo);
    Serial.print(F(" | Brillo: "));
    Serial.print(map(valorPot, 0, 1023, 0, 100));
    Serial.println(F("%"));
    Serial.print(F("Práctica 2> "));
    
    estadoP2.ultimoValorPot = valorPot;
  }
}

void manejarEntradaPractica2(String entrada) {
  if (entrada == "salir") {
    salirPracticaActual();
    return;
  } else if (entrada == "estado") {
    mostrarEstadoPractica2();
  } else if (entrada == "ayuda") {
    mostrarMenuPractica2();
    return;
  } else {
    Serial.println(F("Comando desconocido. Escribe 'ayuda' para comandos disponibles."));
  }
  
  Serial.print(F("Práctica 2> "));
}

void mostrarEstadoPractica2() {
  int valorPot = analogRead(POTENCIOMETRO);
  Serial.println();
  Serial.println(F("--- Estado Práctica 2 ---"));
  Serial.print(F("Potenciómetro crudo: "));
  Serial.print(valorPot);
  Serial.print(F(" ("));
  Serial.print((valorPot / 1023.0) * 5.0, 2);
  Serial.println(F("V)"));
  Serial.print(F("Valor PWM: "));
  Serial.print(estadoP2.brillo);
  Serial.print(F(" ("));
  Serial.print((estadoP2.brillo / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.println(F("-------------------------"));
}

// ============================================
// PRÁCTICA 3: CONTROL PWM Y SERVO
// ============================================
void iniciarPractica3() {
  estadoActual = PRACTICA_3;
  practicaActiva = true;
  
  estadoP3.posicionServo = 90;
  estadoP3.posicionObjetivoServo = 90;
  estadoP3.ultimaActualizacionServo = 0;
  estadoP3.brillo = 0;
  
  apagarTodasLasSalidas();
  pinMode(SHARED_D6, OUTPUT);
  miServo.attach(SHARED_D9);
  miServo.write(90);
  
  mostrarInfoPractica3();
  mostrarMenuPractica3();
}

void mostrarInfoPractica3() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("    PRÁCTICA 3: CONTROL PWM Y SERVO"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Configuración de Hardware:"));
  Serial.println(F("- Servo: Cable naranja -> D9"));
  Serial.println(F("         Cable rojo -> 5V"));
  Serial.println(F("         Cable marrón -> GND"));
  Serial.println(F("- LED: D6 -> 220ohm -> LED+ -> LED- -> GND"));
  Serial.println(F("- Potenciómetro: Igual que Práctica 2"));
  Serial.println(F(""));
  Serial.println(F("Objetivos de Aprendizaje:"));
  Serial.println(F("- Control de servomotor"));
  Serial.println(F("- Transiciones suaves de posición"));
  Serial.println(F("- Salidas PWM simultáneas"));
  Serial.println(F("- Uso de librerías"));
  Serial.println(F(""));
}

void mostrarMenuPractica3() {
  Serial.println(F("---------- Comandos Práctica 3 ----------"));
  Serial.println(F("centrar  - Mover servo al centro (90 grados)"));
  Serial.println(F("probar   - Probar rango completo del servo"));
  Serial.println(F("estado   - Mostrar posiciones actuales"));
  Serial.println(F("ayuda    - Mostrar este menú"));
  Serial.println(F("salir    - Volver al menú principal"));
  Serial.println(F("------------------------------------------"));
  Serial.println(F(""));
  Serial.println(F("El potenciómetro controla LED y servo."));
  Serial.println(F("El servo se mueve suavemente a la posición objetivo."));
  Serial.println(F(""));
  Serial.print(F("Práctica 3> "));
}

void ejecutarPractica3() {
  if (!practicaActiva) return;
  
  unsigned long tiempoActual = millis();
  
  int valorPot = analogRead(POTENCIOMETRO);
  
  estadoP3.brillo = map(valorPot, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, estadoP3.brillo);
  
  estadoP3.posicionObjetivoServo = map(valorPot, 0, 1023, 0, 180);
  
  if (tiempoActual - estadoP3.ultimaActualizacionServo >= 15) {
    if (estadoP3.posicionServo < estadoP3.posicionObjetivoServo) {
      estadoP3.posicionServo++;
    } else if (estadoP3.posicionServo > estadoP3.posicionObjetivoServo) {
      estadoP3.posicionServo--;
    }
    miServo.write(estadoP3.posicionServo);
    estadoP3.ultimaActualizacionServo = tiempoActual;
  }
}

void manejarEntradaPractica3(String entrada) {
  if (entrada == "salir") {
    salirPracticaActual();
    return;
  } else if (entrada == "centrar") {
    estadoP3.posicionObjetivoServo = 90;
    Serial.println(F("Centrando servo a 90 grados..."));
  } else if (entrada == "probar") {
    probarRangoServo();
  } else if (entrada == "estado") {
    mostrarEstadoPractica3();
  } else if (entrada == "ayuda") {
    mostrarMenuPractica3();
    return;
  } else {
    Serial.println(F("Comando desconocido. Escribe 'ayuda' para comandos disponibles."));
  }
  
  Serial.print(F("Práctica 3> "));
}

void mostrarEstadoPractica3() {
  Serial.println();
  Serial.println(F("--- Estado Práctica 3 ---"));
  Serial.print(F("Brillo LED: "));
  Serial.print(estadoP3.brillo);
  Serial.print(F(" ("));
  Serial.print((estadoP3.brillo / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.print(F("Posición servo: "));
  Serial.print(estadoP3.posicionServo);
  Serial.print(F(" grados (Objetivo: "));
  Serial.print(estadoP3.posicionObjetivoServo);
  Serial.println(F(" grados)"));
  Serial.println(F("-------------------------"));
}

void probarRangoServo() {
  Serial.println();
  Serial.println(F("Probando rango completo del servo..."));
  
  Serial.println(F("Moviendo a 0 grados"));
  for (int pos = estadoP3.posicionServo; pos >= 0; pos--) {
    miServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("Moviendo a 180 grados"));
  for (int pos = 0; pos <= 180; pos++) {
    miServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("Regresando al centro"));
  for (int pos = 180; pos >= 90; pos--) {
    miServo.write(pos);
    delay(15);
  }
  
  estadoP3.posicionServo = 90;
  estadoP3.posicionObjetivoServo = 90;
  Serial.println(F("Prueba de servo completada"));
}

// ============================================
// PRÁCTICA 4: MEZCLA DE COLORES LED RGB - CORREGIDA
// ============================================
void iniciarPractica4() {
  estadoActual = PRACTICA_4;
  practicaActiva = true;
  
  estadoP4.valorRojo = 0;
  estadoP4.valorVerde = 0;
  estadoP4.valorAzul = 0;
  estadoP4.pasoColor = 0;
  estadoP4.modoColor = 0;
  estadoP4.ultimaActualizacionColor = 0;
  
  apagarTodasLasSalidas();
  pinMode(PIN_RGB_ROJO, OUTPUT);   // D11 - Canal Rojo
  pinMode(PIN_RGB_VERDE, OUTPUT);  // D10 - Canal Verde
  pinMode(PIN_RGB_AZUL, OUTPUT);   // D9 - Canal Azul
  
  mostrarInfoPractica4();
  mostrarMenuPractica4();
}

void mostrarInfoPractica4() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("   PRÁCTICA 4: MEZCLA COLORES LED RGB"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Configuración de Hardware:"));
  Serial.println(F("- LED RGB pin Rojo: D11 -> 220ohm -> Pin Rojo"));
  Serial.println(F("- LED RGB pin Verde: D10 -> 220ohm -> Pin Verde"));
  Serial.println(F("- LED RGB pin Azul: D9 -> 220ohm -> Pin Azul"));
  Serial.println(F("- LED RGB Cátodo Común -> GND"));
  Serial.println(F(""));
  Serial.println(F("IMPORTANTE: Verifica que los colores coincidan"));
  Serial.println(F("Si no coinciden, ajusta las conexiones físicas."));
  Serial.println(F(""));
  Serial.println(F("Objetivos de Aprendizaje:"));
  Serial.println(F("- Teoría del color RGB"));
  Serial.println(F("- Algoritmos de mezcla de colores"));
  Serial.println(F("- Programación de animaciones"));
  Serial.println(F("- Diseño de máquinas de estado"));
  Serial.println(F(""));
}

void mostrarMenuPractica4() {
  Serial.println(F("---------- Comandos Práctica 4 ----------"));
  Serial.println(F("modo     - Cambiar modo de animación"));
  Serial.println(F("rojo     - Color rojo sólido"));
  Serial.println(F("verde    - Color verde sólido"));
  Serial.println(F("azul     - Color azul sólido"));
  Serial.println(F("blanco   - Color blanco"));
  Serial.println(F("apagar   - Apagar LED RGB"));
  Serial.println(F("probar   - Probar cada color individualmente"));
  Serial.println(F("estado   - Mostrar valores de color actuales"));
  Serial.println(F("ayuda    - Mostrar este menú"));
  Serial.println(F("salir    - Volver al menú principal"));
  Serial.println(F("------------------------------------------"));
  Serial.println(F(""));
  Serial.println(F("Iniciando con modo de animación arcoíris."));
  Serial.println(F(""));
  Serial.print(F("Práctica 4> "));
}

void ejecutarPractica4() {
  if (!practicaActiva) return;
  
  unsigned long tiempoActual = millis();
  
  if (tiempoActual - estadoP4.ultimaActualizacionColor >= 50) {
    actualizarColoresRGB();
    estadoP4.ultimaActualizacionColor = tiempoActual;
    estadoP4.pasoColor++;
  }
  
  // MAPEO CORREGIDO: Aplicar valores a los pines correctos
  analogWrite(PIN_RGB_ROJO, estadoP4.valorRojo);     // D11 -> Rojo
  analogWrite(PIN_RGB_VERDE, estadoP4.valorVerde);   // D10 -> Verde
  analogWrite(PIN_RGB_AZUL, estadoP4.valorAzul);     // D9 -> Azul
}

void manejarEntradaPractica4(String entrada) {
  if (entrada == "salir") {
    salirPracticaActual();
    return;
  } else if (entrada == "modo") {
    estadoP4.modoColor = (estadoP4.modoColor + 1) % 4;
    estadoP4.pasoColor = 0;
    Serial.print(F("Modo de animación cambiado a: "));
    switch (estadoP4.modoColor) {
      case 0: Serial.println(F("Arcoíris")); break;
      case 1: Serial.println(F("Respiración")); break;
      case 2: Serial.println(F("Estroboscópico")); break;
      case 3: Serial.println(F("Estático")); break;
    }
  } else if (entrada == "rojo") {
    establecerColorEstatico(255, 0, 0);
    Serial.println(F("Color establecido a ROJO"));
  } else if (entrada == "verde") {
    establecerColorEstatico(0, 255, 0);
    Serial.println(F("Color establecido a VERDE"));
  } else if (entrada == "azul") {
    establecerColorEstatico(0, 0, 255);
    Serial.println(F("Color establecido a AZUL"));
  } else if (entrada == "blanco") {
    establecerColorEstatico(255, 255, 255);
    Serial.println(F("Color establecido a BLANCO"));
  } else if (entrada == "apagar") {
    establecerColorEstatico(0, 0, 0);
    Serial.println(F("LED RGB APAGADO"));
  } else if (entrada == "probar") {
    probarColoresRGB();
  } else if (entrada == "estado") {
    mostrarEstadoPractica4();
  } else if (entrada == "ayuda") {
    mostrarMenuPractica4();
    return;
  } else {
    Serial.println(F("Comando desconocido. Escribe 'ayuda' para comandos disponibles."));
  }
  
  Serial.print(F("Práctica 4> "));
}

// NUEVA FUNCIÓN: Probar colores individualmente
void probarColoresRGB() {
  Serial.println();
  Serial.println(F("Probando colores RGB individualmente..."));
  Serial.println(F("Verifica que cada color coincida con el nombre."));
  Serial.println(F(""));
  
  // Apagar todo
  establecerColorEstatico(0, 0, 0);
  delay(1000);
  
  // Probar ROJO
  Serial.println(F("Mostrando ROJO (debe ser rojo)"));
  establecerColorEstatico(255, 0, 0);
  analogWrite(PIN_RGB_ROJO, 255);
  analogWrite(PIN_RGB_VERDE, 0);
  analogWrite(PIN_RGB_AZUL, 0);
  delay(2000);
  
  // Probar VERDE
  Serial.println(F("Mostrando VERDE (debe ser verde)"));
  establecerColorEstatico(0, 255, 0);
  analogWrite(PIN_RGB_ROJO, 0);
  analogWrite(PIN_RGB_VERDE, 255);
  analogWrite(PIN_RGB_AZUL, 0);
  delay(2000);
  
  // Probar AZUL
  Serial.println(F("Mostrando AZUL (debe ser azul)"));
  establecerColorEstatico(0, 0, 255);
  analogWrite(PIN_RGB_ROJO, 0);
  analogWrite(PIN_RGB_VERDE, 0);
  analogWrite(PIN_RGB_AZUL, 255);
  delay(2000);
  
  // Apagar
  Serial.println(F("Apagando..."));
  establecerColorEstatico(0, 0, 0);
  analogWrite(PIN_RGB_ROJO, 0);
  analogWrite(PIN_RGB_VERDE, 0);
  analogWrite(PIN_RGB_AZUL, 0);
  
  Serial.println(F(""));
  Serial.println(F("Prueba completada."));
  Serial.println(F("Si los colores no coinciden, verifica las conexiones:"));
  Serial.println(F("- Pin D11 debe ir al canal ROJO del LED"));
  Serial.println(F("- Pin D10 debe ir al canal VERDE del LED"));
  Serial.println(F("- Pin D9 debe ir al canal AZUL del LED"));
}

void establecerColorEstatico(int rojo, int verde, int azul) {
  estadoP4.modoColor = 3; // Modo estático
  estadoP4.valorRojo = rojo;
  estadoP4.valorVerde = verde;
  estadoP4.valorAzul = azul;
}

void mostrarEstadoPractica4() {
  Serial.println();
  Serial.println(F("--- Estado Práctica 4 ---"));
  Serial.print(F("Modo de animación: "));
  switch (estadoP4.modoColor) {
    case 0: Serial.println(F("Arcoíris")); break;
    case 1: Serial.println(F("Respiración")); break;
    case 2: Serial.println(F("Estroboscópico")); break;
    case 3: Serial.println(F("Estático")); break;
  }
  Serial.print(F("Valores RGB: R="));
  Serial.print(estadoP4.valorRojo);
  Serial.print(F(" G="));
  Serial.print(estadoP4.valorVerde);
  Serial.print(F(" B="));
  Serial.println(estadoP4.valorAzul);
  Serial.print(F("Mapeo de pines: D"));
  Serial.print(PIN_RGB_ROJO);
  Serial.print(F("=R, D"));
  Serial.print(PIN_RGB_VERDE);
  Serial.print(F("=G, D"));
  Serial.print(PIN_RGB_AZUL);
  Serial.println(F("=B"));
  Serial.print(F("Paso de animación: "));
  Serial.println(estadoP4.pasoColor);
  Serial.println(F("-------------------------"));
}

void actualizarColoresRGB() {
  if (estadoP4.modoColor == 3) return; // Modo estático - no actualizar
  
  int fase = estadoP4.pasoColor % 360;
  
  switch (estadoP4.modoColor) {
    case 0: // Arcoíris
      if (fase < 60) {
        estadoP4.valorRojo = 255;
        estadoP4.valorVerde = map(fase, 0, 59, 0, 255);
        estadoP4.valorAzul = 0;
      } else if (fase < 120) {
        estadoP4.valorRojo = map(fase, 60, 119, 255, 0);
        estadoP4.valorVerde = 255;
        estadoP4.valorAzul = 0;
      } else if (fase < 180) {
        estadoP4.valorRojo = 0;
        estadoP4.valorVerde = 255;
        estadoP4.valorAzul = map(fase, 120, 179, 0, 255);
      } else if (fase < 240) {
        estadoP4.valorRojo = 0;
        estadoP4.valorVerde = map(fase, 180, 239, 255, 0);
        estadoP4.valorAzul = 255;
      } else if (fase < 300) {
        estadoP4.valorRojo = map(fase, 240, 299, 0, 255);
        estadoP4.valorVerde = 0;
        estadoP4.valorAzul = 255;
      } else {
        estadoP4.valorRojo = 255;
        estadoP4.valorVerde = 0;
        estadoP4.valorAzul = map(fase, 300, 359, 255, 0);
      }
      break;
      
    case 1: // Respiración
      {
        int brillo = (sin(estadoP4.pasoColor * 0.1) + 1) * 127.5;
        estadoP4.valorRojo = brillo;
        estadoP4.valorVerde = brillo;
        estadoP4.valorAzul = brillo;
      }
      break;
      
    case 2: // Estroboscópico
      {
        int faseEstrobo = (estadoP4.pasoColor / 20) % 8;
        estadoP4.valorRojo = estadoP4.valorVerde = estadoP4.valorAzul = 0;
        switch (faseEstrobo) {
          case 0: estadoP4.valorRojo = 255; break;
          case 1: estadoP4.valorVerde = 255; break;
          case 2: estadoP4.valorAzul = 255; break;
          case 3: estadoP4.valorRojo = estadoP4.valorVerde = 255; break;
          case 4: estadoP4.valorRojo = estadoP4.valorAzul = 255; break;
          case 5: estadoP4.valorVerde = estadoP4.valorAzul = 255; break;
          case 6: estadoP4.valorRojo = estadoP4.valorVerde = estadoP4.valorAzul = 255; break;
          case 7: break; // Apagado
        }
      }
      break;
  }
}

// ============================================
// PRÁCTICA 5: DISPLAY DE 7 SEGMENTOS - CORREGIDA
// ============================================
void iniciarPractica5() {
  estadoActual = PRACTICA_5;
  practicaActiva = true;
  
  estadoP5.numeroDisplay = 1234;
  estadoP5.digitoActual = 0;
  estadoP5.ultimaActualizacionDisplay = 0;
  estadoP5.mostrarCerosIzquierda = false;
  estadoP5.displayActivo = true;
  
  apagarTodasLasSalidas();
  
  // Configurar pines de segmentos (D3-D10)
  for (int i = SHARED_D3; i <= SHARED_D10; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  // Configurar pines de control de dígitos
  pinMode(SHARED_D11, OUTPUT);
  pinMode(SHARED_D12, OUTPUT);
  pinMode(SHARED_A0, OUTPUT);
  pinMode(SHARED_A1, OUTPUT);
  
  // Apagar todos los dígitos inicialmente
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
  
  mostrarInfoPractica5();
  mostrarMenuPractica5();
}

void mostrarInfoPractica5() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("     PRÁCTICA 5: DISPLAY 7 SEGMENTOS"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Configuración de Hardware:"));
  Serial.println(F("- Display 7 segmentos de 4 dígitos"));
  Serial.println(F("- Segmentos A-G,DP: D3-D10 -> 220ohm -> segmentos"));
  Serial.println(F("- Control dígitos: D11,D12,A0,A1 -> 1kohm -> transistores"));
  Serial.println(F("- Cableado complejo - consultar diagrama detallado"));
  Serial.println(F(""));
  Serial.println(F("Objetivos de Aprendizaje:"));
  Serial.println(F("- Multiplexado de display"));
  Serial.println(F("- Programación crítica de tiempo"));
  Serial.println(F("- Patrones de números binarios"));
  Serial.println(F("- Persistencia de la visión"));
  Serial.println(F(""));
}

void mostrarMenuPractica5() {
  Serial.println(F("---------- Comandos Práctica 5 ----------"));
  Serial.println(F("establecer <numero> - Mostrar número (0-9999)"));
  Serial.println(F("limpiar             - Limpiar display (mostrar 0000)"));
  Serial.println(F("ceros               - Alternar ceros a la izquierda"));
  Serial.println(F("probar              - Probar todos los segmentos"));
  Serial.println(F("contar              - Contar de 0 a 9999"));
  Serial.println(F("diagnostico         - Probar cada dígito individualmente"));
  Serial.println(F("estado              - Mostrar configuración actual"));
  Serial.println(F("ayuda               - Mostrar este menú"));
  Serial.println(F("salir               - Volver al menú principal"));
  Serial.println(F("------------------------------------------"));
  Serial.println(F(""));
  Serial.println(F("Mostrando actualmente: 1234"));
  Serial.println(F(""));
  Serial.print(F("Práctica 5> "));
}

void ejecutarPractica5() {
  if (!practicaActiva || !estadoP5.displayActivo) return;
  
  unsigned long tiempoActual = millis();
  
  // CAMBIO CRÍTICO: Aumentar intervalo de 2ms a 4ms para mayor estabilidad
  if (tiempoActual - estadoP5.ultimaActualizacionDisplay >= 4) {
    actualizarDisplaySieteSegmentos();
    estadoP5.ultimaActualizacionDisplay = tiempoActual;
  }
}

void manejarEntradaPractica5(String entrada) {
  if (entrada == "salir") {
    salirPracticaActual();
    return;
  } else if (entrada.startsWith("establecer ")) {
    String cadenaNumero = entrada.substring(11);
    int numero = cadenaNumero.toInt();
    if (numero >= 0 && numero <= 9999 || cadenaNumero == "0") {
      estadoP5.numeroDisplay = numero;
      estadoP5.displayActivo = true;
      Serial.print(F("Display establecido a: "));
      Serial.println(estadoP5.numeroDisplay);
    } else {
      Serial.println(F("Número inválido. Ingresa 0-9999"));
    }
  } else if (entrada == "limpiar") {
    // CORRECCIÓN CRÍTICA: Forzar mostrar ceros y activar display
    estadoP5.numeroDisplay = 0;
    estadoP5.mostrarCerosIzquierda = true;  // Forzar mostrar 0000
    estadoP5.displayActivo = true;
    Serial.println(F("Display limpiado - mostrando 0000"));
  } else if (entrada == "ceros") {
    estadoP5.mostrarCerosIzquierda = !estadoP5.mostrarCerosIzquierda;
    Serial.print(F("Ceros a la izquierda: "));
    Serial.println(estadoP5.mostrarCerosIzquierda ? F("ACTIVADO") : F("DESACTIVADO"));
  } else if (entrada == "probar") {
    probarDisplaySieteSegmentos();
  } else if (entrada == "contar") {
    contarDisplay();
  } else if (entrada == "diagnostico") {
    diagnosticarDisplay();
  } else if (entrada == "estado") {
    mostrarEstadoPractica5();
  } else if (entrada == "ayuda") {
    mostrarMenuPractica5();
    return;
  } else {
    Serial.println(F("Comando desconocido. Escribe 'ayuda' para comandos disponibles."));
  }
  
  Serial.print(F("Práctica 5> "));
}

void mostrarEstadoPractica5() {
  Serial.println();
  Serial.println(F("--- Estado Práctica 5 ---"));
  Serial.print(F("Número actual: "));
  Serial.println(estadoP5.numeroDisplay);
  Serial.print(F("Ceros a la izquierda: "));
  Serial.println(estadoP5.mostrarCerosIzquierda ? F("ACTIVADO") : F("DESACTIVADO"));
  Serial.print(F("Display activo: "));
  Serial.println(estadoP5.displayActivo ? F("SÍ") : F("NO"));
  Serial.print(F("Dígito actual siendo refrescado: "));
  Serial.println(estadoP5.digitoActual);
  Serial.print(F("Intervalo de multiplexado: 4ms (250Hz)"));
  Serial.println(F("-------------------------"));
}

// FUNCIÓN CORREGIDA: Probar display con multiplexado activo
void probarDisplaySieteSegmentos() {
  Serial.println();
  Serial.println(F("Probando display de 7 segmentos..."));
  
  estadoP5.displayActivo = true;
  
  for (int i = 0; i <= 9; i++) {
    estadoP5.numeroDisplay = i * 1111;  // 0000, 1111, 2222, etc.
    Serial.print(F("Mostrando: "));
    Serial.println(estadoP5.numeroDisplay);
    
    // CORRECCIÓN CRÍTICA: Mantener multiplexado durante la pausa
    unsigned long tiempoInicio = millis();
    while (millis() - tiempoInicio < 1000) {
      if (millis() - estadoP5.ultimaActualizacionDisplay >= 4) {
        actualizarDisplaySieteSegmentos();
        estadoP5.ultimaActualizacionDisplay = millis();
      }
    }
  }
  
  estadoP5.numeroDisplay = 1234;
  Serial.println(F("Prueba completada - restaurado a 1234"));
}

void contarDisplay() {
  Serial.println();
  Serial.println(F("Contando de 0 a 9999 (presiona cualquier tecla para detener)..."));
  
  estadoP5.displayActivo = true;
  
  for (int i = 0; i <= 9999; i++) {
    estadoP5.numeroDisplay = i;
    
    unsigned long tiempoInicio = millis();
    while (millis() - tiempoInicio < 100) {
      if (millis() - estadoP5.ultimaActualizacionDisplay >= 4) {
        actualizarDisplaySieteSegmentos();
        estadoP5.ultimaActualizacionDisplay = millis();
      }
      
      if (Serial.available()) {
        while (Serial.available()) Serial.read();
        Serial.println();
        Serial.println(F("Conteo detenido por el usuario"));
        return;
      }
    }
  }
  
  Serial.println();
  Serial.println(F("Conteo completado"));
}

// NUEVA FUNCIÓN: Diagnóstico individual de dígitos
void diagnosticarDisplay() {
  Serial.println();
  Serial.println(F("Iniciando diagnóstico del display..."));
  Serial.println(F("Cada dígito mostrará '8' por 3 segundos"));
  
  estadoP5.displayActivo = false;  // Desactivar multiplexado automático
  
  // Probar cada dígito individualmente
  for (int digito = 0; digito < 4; digito++) {
    Serial.print(F("Probando dígito "));
    Serial.print(digito + 1);
    Serial.println(F(" (de izquierda a derecha)"));
    
    // Apagar todos los dígitos y segmentos
    apagarTodoDisplay();
    
    // Configurar patrón para '8' (todos los segmentos excepto DP)
    byte patron8 = pgm_read_byte(&patronesDigitos[8]);
    
    // Activar segmentos para '8'
    digitalWrite(SHARED_D3, (patron8 >> 7) & 1);  // A
    digitalWrite(SHARED_D4, (patron8 >> 6) & 1);  // B
    digitalWrite(SHARED_D5, (patron8 >> 5) & 1);  // C
    digitalWrite(SHARED_D6, (patron8 >> 4) & 1);  // D
    digitalWrite(SHARED_D7, (patron8 >> 3) & 1);  // E
    digitalWrite(SHARED_D8, (patron8 >> 2) & 1);  // F
    digitalWrite(SHARED_D9, (patron8 >> 1) & 1);  // G
    digitalWrite(SHARED_D10, LOW);                 // DP apagado
    
    // Activar solo el dígito correspondiente
    switch (digito) {
      case 0: digitalWrite(SHARED_D11, HIGH); break;  // Dígito 1 (miles)
      case 1: digitalWrite(SHARED_D12, HIGH); break;  // Dígito 2 (centenas)
      case 2: digitalWrite(SHARED_A0, HIGH); break;   // Dígito 3 (decenas)
      case 3: digitalWrite(SHARED_A1, HIGH); break;   // Dígito 4 (unidades)
    }
    
    delay(3000);  // Mostrar por 3 segundos
  }
  
  // Apagar todo
  apagarTodoDisplay();
  
  Serial.println(F(""));
  Serial.println(F("Diagnóstico completado."));
  Serial.println(F("Si algún dígito no se mostró correctamente:"));
  Serial.println(F("- Verifica conexiones de transistores"));
  Serial.println(F("- Verifica conexiones de segmentos"));
  Serial.println(F("- Verifica resistencias de 220ohm y 1kohm"));
  
  // Reactivar multiplexado
  estadoP5.displayActivo = true;
  estadoP5.numeroDisplay = 1234;
}

// NUEVA FUNCIÓN: Apagar completamente el display
void apagarTodoDisplay() {
  // Apagar todos los segmentos
  for (int pin = SHARED_D3; pin <= SHARED_D10; pin++) {
    digitalWrite(pin, LOW);
  }
  
  // Apagar todos los dígitos
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
}

// FUNCIÓN CORREGIDA: Multiplexado mejorado
void actualizarDisplaySieteSegmentos() {
  // PASO 1: Apagar todos los dígitos primero (previene ghosting)
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
  
  // PASO 2: Pequeña pausa para estabilización
  delayMicroseconds(10);
  
  // PASO 3: Apagar todos los segmentos
  for (int pin = SHARED_D3; pin <= SHARED_D10; pin++) {
    digitalWrite(pin, LOW);
  }
  
  // PASO 4: Calcular dígitos
  int miles = (estadoP5.numeroDisplay / 1000) % 10;
  int centenas = (estadoP5.numeroDisplay / 100) % 10;
  int decenas = (estadoP5.numeroDisplay / 10) % 10;
  int unidades = estadoP5.numeroDisplay % 10;
  
  int valorDigito;
  bool mostrarDigito = true;
  
  switch (estadoP5.digitoActual) {
    case 0:
      valorDigito = miles;
      // CORRECCIÓN: Lógica mejorada para mostrar dígitos
      mostrarDigito = estadoP5.mostrarCerosIzquierda || miles > 0 || estadoP5.numeroDisplay >= 1000;
      break;
    case 1:
      valorDigito = centenas;
      mostrarDigito = estadoP5.mostrarCerosIzquierda || centenas > 0 || estadoP5.numeroDisplay >= 100;
      break;
    case 2:
      valorDigito = decenas;
      mostrarDigito = estadoP5.mostrarCerosIzquierda || decenas > 0 || estadoP5.numeroDisplay >= 10;
      break;
    case 3:
      valorDigito = unidades;
      mostrarDigito = true;  // Siempre mostrar unidades
      break;
  }
  
  // PASO 5: Si debe mostrar el dígito, configurar segmentos y activar
  if (mostrarDigito) {
    byte patron = pgm_read_byte(&patronesDigitos[valorDigito]);
    
    // Configurar segmentos
    digitalWrite(SHARED_D3, (patron >> 7) & 1);   // Segmento A
    digitalWrite(SHARED_D4, (patron >> 6) & 1);   // Segmento B
    digitalWrite(SHARED_D5, (patron >> 5) & 1);   // Segmento C
    digitalWrite(SHARED_D6, (patron >> 4) & 1);   // Segmento D
    digitalWrite(SHARED_D7, (patron >> 3) & 1);   // Segmento E
    digitalWrite(SHARED_D8, (patron >> 2) & 1);   // Segmento F
    digitalWrite(SHARED_D9, (patron >> 1) & 1);   // Segmento G
    digitalWrite(SHARED_D10, patron & 1);         // Punto decimal
    
    // PASO 6: Pequeña pausa antes de activar dígito
    delayMicroseconds(5);
    
    // PASO 7: Activar el dígito correspondiente
    switch (estadoP5.digitoActual) {
      case 0: digitalWrite(SHARED_D11, HIGH); break;  // Dígito 1 (miles)
      case 1: digitalWrite(SHARED_D12, HIGH); break;  // Dígito 2 (centenas)
      case 2: digitalWrite(SHARED_A0, HIGH); break;   // Dígito 3 (decenas)
      case 3: digitalWrite(SHARED_A1, HIGH); break;   // Dígito 4 (unidades)
    }
  }
  
  // PASO 8: Avanzar al siguiente dígito
  estadoP5.digitoActual = (estadoP5.digitoActual + 1) % 4;
}

// ============================================
// FUNCIONES DE UTILIDAD
// ============================================
void salirPracticaActual() {
  Serial.println();
  Serial.println(F("Saliendo de la práctica actual..."));
  
  apagarTodasLasSalidas();
  
  if (miServo.attached()) {
    miServo.detach();
  }
  
  estadoActual = MENU_PRINCIPAL;
  practicaActiva = false;
  
  Serial.println(F("Regresado al menú principal"));
  mostrarMenuPrincipal();
}

void apagarTodasLasSalidas() {
  for (int pin = 3; pin <= 12; pin++) {
    digitalWrite(pin, LOW);
  }
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
}

void actualizarLedEstado() {
  int intervalo;
  switch (estadoActual) {
    case MENU_PRINCIPAL: intervalo = 1000; break;
    case PRACTICA_1: intervalo = 200; break;
    case PRACTICA_2: intervalo = 400; break;
    case PRACTICA_3: intervalo = 600; break;
    case PRACTICA_4: intervalo = 800; break;
    case PRACTICA_5: intervalo = 100; break;
  }
  
  digitalWrite(LED_ESTADO, (millis() / intervalo) % 2);
}

void mostrarInfoSistema() {
  Serial.println();
  Serial.println(F("========== INFORMACIÓN DEL SISTEMA =========="));
  Serial.println(F("Hardware: Arduino Nano (ATmega328P)"));
  Serial.println(F("Velocidad de reloj: 16 MHz"));
  Serial.println(F("Memoria Flash: 32 KB"));
  Serial.println(F("SRAM: 2 KB"));
  Serial.println(F("EEPROM: 1 KB"));
  Serial.println();
  Serial.print(F("Tiempo de funcionamiento: "));
  Serial.print(millis() / 1000);
  Serial.println(F(" segundos"));
  Serial.print(F("RAM libre: "));
  Serial.print(obtenerRAMLibre());
  Serial.println(F(" bytes"));
  Serial.println(F("============================================="));
  Serial.println();
}

int obtenerRAMLibre() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
