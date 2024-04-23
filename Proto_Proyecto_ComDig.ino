#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ANCHITO 128
#define ALTITO 64
#define baudios 115200
#define botonPin 2
#define buzzerPin 4
#define Canal 100
#define Nintentos 3
#define Tintentos 500
#define Tbateria 5000
#define BateriaPin A0
#define BateriaMax 5.5
#define BateriaMin 5

RF24 radio(9, 10);  // CE, CSN
const uint8_t DefaultPipe[5] = "dPipe";
const uint8_t Pipes[6][5] = { { "1Pipe" }, { "2Pipe" }, { "3Pipe" }, { "4Pipe" }, { "5Pipe" }, { "6Pipe" } };
bool PipeOcupada[6] = { 0, 0, 0, 0, 0, 0 }, Modo, Presiono, Conectado;

int Tonos[32] = { 2637, 2637, 2637, 2637, 0, 0, 2637, 2637, 0, 0, 2093, 2093, 2637, 2637, 0, 0, 3136, 3136, 0, 0, 0, 0, 0, 0, 1568, 1568, 0, 0, 0, 0, 0, 0 };
int Puntaje;
uint8_t Dato[4] = { 0, 0, 0, 0 }, Usuario, PorcentajeBateria, Pregunta, TurnoAsignado, TurnoActual, Correcto, PuestoFinal;

const unsigned char PROGMEM LogoUMNG[] = {  //logo de UMNG
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x07, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0xff, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0xfe, 0xe2, 0xb9, 0xff, 0x00, 0x00, 0x00, 0x03, 0xf4, 0xa8, 0xb3, 0x8f, 0xc0, 0x00,
  0x00, 0x0f, 0xa4, 0xef, 0xfb, 0xab, 0xf0, 0x00, 0x00, 0x3c, 0x23, 0xff, 0xff, 0xd0, 0x7c, 0x00,
  0x00, 0x70, 0x5f, 0xff, 0xff, 0xf8, 0x2f, 0x00, 0x01, 0xfc, 0xff, 0x00, 0x00, 0xfe, 0x5f, 0x80,
  0x01, 0xf3, 0xf8, 0x00, 0x00, 0x1f, 0xc3, 0x80, 0x00, 0xef, 0xc0, 0x00, 0x00, 0x03, 0xf7, 0x80,
  0x00, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0x00, 0x01, 0xff, 0xdf, 0xff, 0xff, 0xfd, 0xff, 0x80,
  0x03, 0xff, 0xdf, 0xff, 0xff, 0xef, 0xff, 0xc0, 0x07, 0xff, 0x8f, 0xfb, 0xff, 0xcc, 0xff, 0xf0,
  0x1f, 0xfe, 0x07, 0xfd, 0xff, 0x1c, 0x7f, 0xf8, 0x00, 0x7c, 0x03, 0xfe, 0xff, 0x1c, 0x3e, 0x00,
  0x00, 0x74, 0x01, 0xff, 0x7e, 0x3c, 0x0f, 0x00, 0x00, 0x60, 0x00, 0xff, 0xbc, 0x7c, 0x07, 0x00,
  0x00, 0xc0, 0x00, 0x7f, 0xd8, 0x7c, 0x03, 0x00, 0x00, 0x80, 0x00, 0x7f, 0xe8, 0xfc, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x3f, 0xe1, 0xfc, 0x00, 0x00, 0x00, 0x10, 0x00, 0x1f, 0xf3, 0xfc, 0x08, 0x00,
  0x00, 0x18, 0x00, 0x0f, 0xe3, 0xfc, 0x18, 0x00, 0x00, 0x18, 0x00, 0x07, 0xed, 0xfc, 0x18, 0x00,
  0x00, 0x20, 0x00, 0x03, 0xfd, 0xfc, 0x00, 0x00, 0x00, 0x20, 0x00, 0x01, 0xfe, 0xfc, 0x1e, 0x00,
  0x00, 0x30, 0x00, 0x00, 0xff, 0x7c, 0x0e, 0x00, 0x00, 0x20, 0x00, 0x00, 0xff, 0x2c, 0x04, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x7e, 0x8c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x88, 0x3f, 0x8c, 0x0f, 0x00,
  0x00, 0x70, 0x00, 0xf8, 0x1f, 0xdc, 0x0f, 0x00, 0x00, 0x60, 0x01, 0xdc, 0x0f, 0xfc, 0x06, 0x00,
  0x00, 0x20, 0x01, 0xde, 0x07, 0xfc, 0x02, 0x00, 0x00, 0xd0, 0x01, 0xce, 0x03, 0xfc, 0x0a, 0x00,
  0x00, 0x58, 0x01, 0xce, 0x01, 0xfc, 0x1a, 0x00, 0x00, 0x78, 0x00, 0xdc, 0x00, 0xfc, 0x1e, 0x00,
  0x00, 0x30, 0x00, 0x78, 0x00, 0xfc, 0x1e, 0x00, 0x00, 0x54, 0x00, 0x30, 0x00, 0x7c, 0x28, 0x00,
  0x00, 0x26, 0x02, 0x68, 0x00, 0x3c, 0x64, 0x00, 0x00, 0x3f, 0x01, 0xc0, 0x00, 0x1c, 0x6c, 0x00,
  0x00, 0x1e, 0x00, 0x00, 0x00, 0x0c, 0x78, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00,
  0x00, 0x06, 0xc0, 0x00, 0x00, 0x03, 0x60, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x00,
  0x00, 0x00, 0xf0, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x3f, 0xff, 0x80, 0x00,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x86, 0x61, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM LogoBateria[] = {  //logo Bateria
  0x0f, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xf0, 0x1c, 0x00, 0x00, 0x70, 0x18, 0x00, 0x00, 0x30,
  0x18, 0x00, 0x00, 0x30, 0xf8, 0x00, 0x00, 0x30, 0xf8, 0x00, 0x00, 0x30, 0xd8, 0x00, 0x00, 0x30,
  0xd8, 0x00, 0x00, 0x30, 0xf8, 0x00, 0x00, 0x30, 0xf8, 0x00, 0x00, 0x30, 0x18, 0x00, 0x00, 0x30,
  0x18, 0x00, 0x00, 0x30, 0x1c, 0x00, 0x00, 0x70, 0x1f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xe0
};

Adafruit_SSD1306 pantallita(ANCHITO, ALTITO, &Wire, 3);



void EnvioTransmisor(int var1) {  // Envio de mensajes por Transceptor (lado Transmisor)
  radio.stopListening(); // Transceptor modo para transmitir
  bool var2;
  switch (var1) {
    case 0:                   // Solicitar asignacion de usuario
      Dato[0] = 0b00000000;   // Identificador
      radio.flush_rx();        // Limpia el buffer de recepcion para eliminar capturas antiguas
      var2; radio.write(&Dato, 1);  // Solo enviar el primer byte del vector Data
      break;
    case 1:

      break;
    case 2:

      break;
  }
  radio.starListening(); // Transceptor modo para recibir
  return var2;
}

void EnvioReceptor(int var1) {  // Envio de mensajes por Transceptor (lado Receptor)
  radio.stopListening(); // Transceptor modo para transmitir
  switch (var1) {
    case 0:  // Envio asignacion de usuario
      for (int i = 0; i < 6; i++) {
        if (PipeOcupada[i] == 0) {  // Recorre todo el vector en busca de una pipe disponible
          Dato[0] = 0b00000000;     // Identificador
          Dato[1] = i;              // Usuario a enviar
          radio.write(&Dato, 2);    // Envia los primeros 2 bytes del vector Data
          PipeOcupada[i] = 1; // Pone la pine como ocupada       
          radio.openReadingPipe(i, Pipes[i]);  // Asigna pipe de lectura para el nuevo usuario
        }
      }
      break;
    case 1:

      break;
    case 2:

      break;
  }
  radio.starListening(); // Transceptor modo para recibir
}

void EnvioSerial(int var1) {  // Envio de mensajes por comunicacion serial (lado receptor)
  switch (var1) {
    case 0:  // Solicitar lista de usuarios conectados
      Serial.write(0b00000000); // Envia identificador
      break;
    case 1:

      break;
    case 2:

      break;
  }
}


bool RecepcionTransmisor() {  // Envio de mensajes por Transceptor (lado Transmisor)
  if (radio.available()) {
    radio.read(&Dato, sizeof(Dato));  // Guardar mensaje
    bool var1;
    switch (Dato[0]) {
      case 0:  // Recibe usuario asignado
        Usuario = Dato[1];
        break;
      case 1:

        break;
      case 2:

        break;
    }
    return 1;  // Si recibe un mensaje devuelve 1
  } else {
    return 0;  // Si no recibe un mensaje devuelve 0
  }
}

bool RecepcionReceptor() {          // Envio de mensajes por Transceptor (lado Receptor)
  radio.read(&Dato, sizeof(Dato));  // Guardar mensaje
  switch (Dato[0]) {
    case 0:  // Recibe solicitud para asignacion de usuario
      EnvioReceptor(0);
      break;
    case 1:

      break;
    case 2:

      break;
  }
}
bool RecepcionSerial() {  // Recepcion de mensajes por comunicacion serial (lado receptor)
  switch (Serial.read()) {
    case 0:  // Recibe lista de usuarios conectados
      for (int i = 0; i < 6; i++) {
        PipeOcupada[i] = bitRead(Serial.read(), i);
      }
      break;
    case 1:

      break;
    case 2:

      break;
  }
}

void Pantallas(int var1) {  // Instrucciones para todos los estados de la pantalla OLED
  float t_ini;
  switch (var1) {  // Casos para las pantallas
    case 0:        // Pantalla de inicio para el Transmisor
      pantallita.clearDisplay();
      pantallita.fillRect(0, 16, 128, 48, WHITE);  // Crea un rectangulo y lo rellena con blanco
      pantallita.setTextColor(BLACK);              // Ajusta el color de texto a negro
      pantallita.setTextSize(2);                   // Ajusta el tamaño del texto
      pantallita.setCursor(0, 32);
      pantallita.setTextWrap(0);  // Permite que el texto se desborde por la pantalla
      pantallita.print("TRANSMITTER");
      pantallita.setTextWrap(1);  // Bloquea que el texto se desborde por la pantalla
      pantallita.setTextSize(1);
      pantallita.setCursor(52, 50);
      pantallita.print("mode");
      pantallita.display();
      break;
    case 1:  // Actualizacion poner "Presiona para conectar"
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print("Press 2 Connect");
      pantallita.display();
      break;
    case 2:                                       // Actualizacion quitar "Presiona para conectar"
      pantallita.fillRect(0, 0, 100, 16, BLACK);  // Rellena un rectangulo de negro
      pantallita.display();
      break;
    case 3:  // Pantalla de inicio para el Receptor
      pantallita.clearDisplay();
      pantallita.fillRect(0, 16, 128, 48, WHITE);
      pantallita.setTextColor(BLACK);
      pantallita.setTextSize(2);
      pantallita.setCursor(16, 32);
      pantallita.setTextWrap(0);
      pantallita.print("RECEIVER");
      pantallita.setTextWrap(1);
      pantallita.setTextSize(1);
      pantallita.setCursor(52, 50);
      pantallita.print("mode");
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 0);
      pantallita.print("Connected: ");
      pantallita.display();
      break;
    case 4:  // Pantalla espera antes de iniciar el primer juego
      pantallita.clearDisplay();
      MedirBateria();
      Pantallas(10);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print("Esperando inicio...");
      pantallita.setTextColor(WHITE);
      pantallita.setTextSize(2);
      pantallita.setCursor(28, 17);
      pantallita.print("Jugador ");
      pantallita.print(Usuario);
      pantallita.setCursor(54, 36);
      pantallita.print("Conectado :D");
      pantallita.display();
      break;
    case 5:  // Pantalla para iniciar nuevo juego y juego en curso
      pantallita.fillRect(0, 0, 128, 16, BLACK);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      for (int i = 1; i < 32; i++) {  // Animacion iniciando juego
        pantallita.fillCircle(64, 40, i * 3, BLACK);
        pantallita.setCursor(0, 4);
        pantallita.print("Iniciando juego.");
        MedirBateria();
        Pantallas(10);
      }
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print("Juego en curso...");
      pantallita.fillRect(0, 16, 128, 48, WHITE);
      pantallita.drawFastVLine(64, 16, 48, BLACK);
      pantallita.drawFastHLine(64, 40, 64, BLACK);
      pantallita.setTextColor(BLACK);
      pantallita.setCursor(11, 19);
      pantallita.print("Jugador");
      pantallita.setCursor(73, 17);
      pantallita.print("Pregunta");
      pantallita.setCursor(76, 41);
      pantallita.print("Puntaje");
      pantallita.setTextSize(4);
      pantallita.setCursor(22, 32);
      pantallita.print(Usuario);
      pantallita.setTextSize(2);
      pantallita.setCursor(94, 25);
      pantallita.print(Pregunta);
      pantallita.setCursor(74, 49);
      pantallita.print(Puntaje);
      MedirBateria();
      Pantallas(10);
      pantallita.display();
      interrupts();
      break;
    case 6:                                // Pantalla TurnoAsignado del jugador y TurnoActual actual
      if (TurnoAsignado == TurnoActual) {  // Si la TurnoAsignado del jugador es la misma que el TurnoActual actual
        pantallita.clearDisplay();
        MedirBateria();
        Pantallas(10);
        pantallita.setTextSize(1);
        pantallita.setTextColor(WHITE);
        pantallita.setCursor(0, 4);
        pantallita.print("Jugador ");
        pantallita.print(Usuario);
        pantallita.setTextSize(3);
        for (int i = 0; i < 5; i++) {  // Animacion si es el TurnoActual del jugador
          pantallita.fillRect(0, 16, 128, 48, BLACK);
          pantallita.setTextColor(WHITE);
          pantallita.setCursor(49, 18);
          pantallita.print("TU");
          pantallita.setCursor(19, 41);
          pantallita.print("TURNO");
          pantallita.display();
          delay(500);
          pantallita.fillRect(0, 16, 128, 48, WHITE);
          pantallita.setTextColor(BLACK);
          pantallita.setCursor(49, 18);
          pantallita.print("TU");
          pantallita.setCursor(19, 41);
          pantallita.print("TURNO");
          pantallita.display();
          delay(500);
        }
      } else {  // Si la TurnoAsignado del jugador no es la misma que el TurnoActual actual
        pantallita.clearDisplay();
        pantallita.setTextSize(1);
        pantallita.setTextColor(WHITE);
        pantallita.setCursor(0, 4);
        pantallita.print("Ya respondiste :D");
        pantallita.fillRect(0, 16, 128, 48, WHITE);
        pantallita.drawFastVLine(64, 16, 48, BLACK);
        pantallita.drawFastHLine(64, 40, 64, BLACK);
        pantallita.setTextColor(BLACK);
        pantallita.setCursor(11, 19);
        pantallita.print("Jugador");
        pantallita.setCursor(73, 17);
        pantallita.print("Posicion");
        pantallita.setCursor(76, 41);
        pantallita.print("Turno");
        pantallita.setTextSize(4);
        pantallita.setCursor(22, 32);
        pantallita.print(Usuario);
        pantallita.setTextSize(2);
        pantallita.setCursor(94, 25);
        pantallita.print(TurnoAsignado);
        pantallita.setCursor(74, 49);
        pantallita.print(TurnoActual);
        MedirBateria();
        Pantallas(10);
        pantallita.display();
      }
      break;
    case 7:  // Pantalla si el usuario acerto o no la pregunta
      pantallita.clearDisplay();
      MedirBateria();
      Pantallas(10);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      if (Correcto) {                  // Si el usuario es acerto la pregunta
        pantallita.print("+100 pts");  // Puntos ganados
        pantallita.setTextSize(3);
        pantallita.setTextWrap(0);
        for (int i = 0; i < 5; i++) {
          pantallita.fillRect(0, 16, 128, 48, BLACK);
          pantallita.setTextColor(WHITE);
          pantallita.setCursor(3, 29);
          pantallita.print("GANADOR");
          pantallita.display();
          delay(500);
          pantallita.fillRect(0, 16, 128, 48, WHITE);
          pantallita.setTextColor(BLACK);
          pantallita.setCursor(3, 29);
          pantallita.print("GANADOR");
          pantallita.display();
          delay(500);
        }
        pantallita.setTextWrap(1);
      } else {  // Si el usuario no acerto la pregunta
        pantallita.print("Estudie +");
        pantallita.setTextSize(3);
        pantallita.fillRect(0, 16, 128, 48, WHITE);
        pantallita.setTextColor(BLACK);
        pantallita.setCursor(3, 29);
        pantallita.print("BRUTO");
        pantallita.display();
      }
      break;
    case 8:  // Pantalla clasificacion de puestos finales
      pantallita.clearDisplay();
      MedirBateria();
      Pantallas(10);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print("Puntuacion: ");
      pantallita.print(Puntaje);
      pantallita.setTextColor(WHITE);
      pantallita.setTextSize(2);
      pantallita.setCursor(28, 17);
      pantallita.print("Puesto");
      pantallita.setTextSize(4);
      pantallita.setCursor(54, 36);
      pantallita.print(PuestoFinal);
      pantallita.display();
      break;
    case 9:  // Pantalla reconectando si pierde la señal
      pantallita.clearDisplay();
      MedirBateria();
      Pantallas(10);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print("Desconectado.");
      pantallita.setTextSize(2);
      pantallita.setCursor(3, 29);
      pantallita.print("RECONECTANDO");
      pantallita.display();
      break;
    case 10: // Actualizar porcentaje de bateria
      pantallita.fillRect(100, 0, 28, 16, BLACK);
      pantallita.drawBitmap(100, 0, LogoBateria, 28, 16, WHITE);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      if (PorcentajeBateria < 10) {
        pantallita.setCursor(113, 5);
      } else if (PorcentajeBateria < 100) {
        pantallita.setCursor(110, 5);
      } else {
        pantallita.setCursor(106, 5);
      }
      pantallita.print(PorcentajeBateria);
      pantallita.display();
      break;
  }
}

void MedirBateria() {  // Mide nivel de bateria
  analogReference(INTERNAL);
  // Se divide entre 10 porque se divide el voltaje a la mitad por las 2 resistencias, osea: 5*2=10
  PorcentajeBateria = map(analogRead(BateriaPin), BateriaMin * 1024.0 / 10, BateriaMax * 1024.0 / 10, 0, 100);
}

void Pulso() {
  if (Conectado) {
    EnvioTransmisor(1);
  } else {
    Presiono = 1;
  }
}

void setup() {
  Serial.begin(baudios);
  radio.begin();
  Wire.begin();

  radio.setChannel(Canal);          // Canal que trabajara el Transceptor
  radio.setPALevel(RF24_PA_MAX);    // Potencia de trabajo Maxima 0dbm
  radio.setDataRate(RF24_250KBPS);  // Velocidad de 250Kbps

  pantallita.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Inicia la pantalla con la direccion 3C
  pantallita.setRotation(0);                     // Se rota la pantalla para que pueda usarse horizontalmente
  pantallita.clearDisplay();                     // Limpia la pantalla
  for (int i = 1; i < 32; i++) {                 // Animaciones al encender el arduino
    pantallita.fillCircle(64, 32, i * 1, WHITE);
    pantallita.display();
  }
  for (int i = 1; i < 32; i++) {
    pantallita.fillCircle(64, 32, i * 1, BLACK);
    pantallita.drawBitmap(32, 0, LogoUMNG, 64, 77, WHITE);  // Animaciones con el logoUMNG
    pantallita.display();
    tone(buzzerPin, Tonos[i]);  // Suena el buzzer a una frecuencia con multiplos de 150
  }
  noTone(buzzerPin);  // Detenemos el sonido del buzzer

  delay(1000);
  if (Serial.read() == 0) {                                            // Si recibe un byte de 0 en la comunicacion serial...
    Modo = 1;                                                          // Se configura en modo de recepcion
    while (Serial.available()) { Serial.read(); }                      // Vaciar el puerto serial
    EnvioSerial(0);                                                    // Solicita lista de usuarios en el servidor
    while (!Serial.available()) {}                                     // Espera a recibir un mensaje serial
    RecepcionSerial();                                                 // Recibe la lista de usuarios en el servidor
    for (int i = 0; i < 6; i++) {
      if(PipeOcupada[i] == 1){
        radio.openReadingPipe(i, Pipes[i]);  // Asigna pipes de lecturas para los usuarios conectados
      }
    }
  } else {                                                             // De lo contrario...
    Modo = 0;                                                          // Modo de transmision
    pinMode(botonPin, INPUT_PULLUP);                                   // Entrada con resistencia de PULLUP interna
    attachInterrupt(digitalPinToInterrupt(botonPin), Pulso, FALLING);  // Configura las interrupciones
  }
  radio.openWritingPipe(DefaultPipe);     // Asigna pipe de escritura por defecto
  radio.openReadingPipe(5, DefaultPipe);  // Asigna pipe de lecura por defecto
}

void loop() {
  if (Modo == 0) {   // Modo de transmision
    int t_ini = millis();
    Presiono = 0;
    Pantallas(0);              // Pantalla Transmisor
    while (!Conectado) {       // Ciclo cuando no esta conectado
      MedirBateria();          // Solicita medir nivel de bateria
      Pantallas(10);           // Actualizar porcentaje de bateria
      while (Presiono == 0) {  // Si no se ha presionado el boton sigue el bucle
        if ((millis() - t_ini) < 500) {
          Pantallas(1);  // Actualizacion poner "Presiona para conectar"
        } else if ((millis() - t_ini) < 1000) {
          Pantallas(2);  // Actualizacion quitar "Presiona para conectar"
        } else {
          t_ini = millis();
        }
      }
      Presiono == 0;
      EnvioTransmisor(0);                        // Solicitar asignacion de usuario
      Conectado = RecepcionTransmisor();         // Recibe usuario asignado
    }
    radio.openWritingPipe(Pipes[Usuario]);     // Asigna pipe de escritura segun el usuario
    radio.openReadingPipe(0, Pipes[Usuario]);  // Asigna pipe de lectura segun el usuario

  } else {
    radio.starListening();
    while(true){
      if(Serial.Avariable()){
        RecepcionSerial();
      }
      if(radio.Avariable()){
        RecepcionReceptor();
      }
    }
  }
}
