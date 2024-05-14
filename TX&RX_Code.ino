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
#define buzzerPin 7
#define Canal 100
#define Nintentos 3
#define Tintentos 500
#define Tbateria 5000
#define BateriaPin A0
#define BateriaMax 5.5
#define BateriaMin 5

RF24 radio(8, 10);  // CE, CSN
const uint8_t unsigned PROGMEM Pipes[6][5] = { { "1Pipe" }, { "2Pipe" }, { "3Pipe" }, { "4Pipe" }, { "5Pipe" }, { "dPipe" } };
bool PipeOcupada[5] = { 0, 0, 0, 0, 0 }, UsuariosPresionaron[5] = { 0, 0, 0, 0, 0 }, Modo, Conectado;

const int unsigned PROGMEM Tonos[32] = { 2637, 2637, 2637, 2637, 0, 0, 2637, 2637, 0, 0, 2093, 2093, 2637, 2637, 0, 0, 3136, 3136, 0, 0, 0, 0, 0, 0, 1568, 1568, 0, 0, 0, 0, 0, 0 };
int unsigned NPregunta, PuntajeObtenido, PuntajeaObtener;
uint8_t unsigned Dato[4] = { 0, 0, 0, 0 }, Nconectados, Usuario, PorcentajeBateria, Posicion, Turno, Correcto, PuestoFinal, UltimaPantalla;

volatile unsigned long TiempoUltimaInterrupcion = 0;  // Variable para almacenar el último tiempo de interrupción
volatile bool Presiono;

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



bool EnvioTransmisor(int var1) {  // Envio de mensajes por Transceptor (lado Transmisor)
  radio.stopListening();          // Transceptor modo para transmitir
  bool var2;
  switch (var1) {
    case 0:  // Solicitar asignacion de usuario
      pantallita.clearDisplay();
      Dato[0] = 0b00000000;          // Identificador
      var2 = radio.write(&Dato, 1);  // Solo enviar el primer byte del vector Data
      break;
    case 1:  // Enviar pulso de boton
      pantallita.clearDisplay();
      Dato[0] = 0b00000001;          // Identificador
      var2 = radio.write(&Dato, 1);  // Solo enviar el primer byte del vector Data
      break;
    case 2:                          // Enviar nivel de bateria
      Dato[0] = 0b00000010;          // Identificador
      Dato[1] = PorcentajeBateria;   // Bateria
      var2 = radio.write(&Dato, 2);  // Solo enviar el primer byte del vector Data
      break;
  }
  radio.startListening();  // Transceptor modo para recibir
  return var2;
}

void EnvioReceptor(int var1) {  // Envio de mensajes por Transceptor (lado Receptor)

  switch (var1) {
    case 0:                                                      // Envio asignacion de usuario
      for (Usuario = 0; Usuario < 5; Usuario++) {                // Ciclo de 0-4
        if (!PipeOcupada[Usuario]) {                             // Recorre todo el vector en busca de una pipe disponible
          PipeOcupada[Usuario] = 1;                              // Pone la pine como ocupada
          Dato[0] = 0b00000000;                                  // Identificador
          Dato[1] = Usuario;                                     // Usuario a enviar
          radio.openWritingPipe(pgm_read_word_near(&Pipes[5]));  // Asigna pipe de escritura
          radio.stopListening();                                 // Transceptor modo para transmitir
          radio.write(&Dato, 2);                                 // Envia los primeros 2 bytes del vector Data
          break;
        }
      }
      break;
    case 1:  // Iniciar nueva pregunta
      UsuariosPresionaron[0] = 0;
      UsuariosPresionaron[1] = 0;
      UsuariosPresionaron[2] = 0;  // Limpia los usuarios que presionaron el boton
      UsuariosPresionaron[3] = 0;
      UsuariosPresionaron[4] = 0;
      Dato[0] = 0b00000001;                                        // Identificador
      Dato[1] = NPregunta;                                         // Numero de pregunta
      Dato[2] = PuntajeObtenido;                                   // Envia el puntaje actual de jugador
      Dato[3] = PuntajeaObtener;                                   // Envia el puntaje que podria obtener con la pregunta
      radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de escritura
      radio.stopListening();                                       // Transceptor modo para transmitir
      radio.write(&Dato, 4);                                       // Envia los primeros 4 bytes del vector Data
      break;
    case 2:                                                        // Envio posicion
      Dato[0] = 0b00000010;                                        // Identificador
      Dato[1] = Posicion;                                          // Envia la posicion del jugador que presiono el boton
      radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de escritura
      radio.stopListening();                                       // Transceptor modo para transmitir
      radio.write(&Dato, 2);                                       // Envia los primeros 2 bytes del vector Data
      break;
    case 3:                                                        // Envio turno
      Dato[0] = 0b00000011;                                        // Identificador
      Dato[1] = Turno;                                             // Envia el turno del jugador que debe contestar
      radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de escritura
      radio.stopListening();                                       // Transceptor modo para transmitir
      radio.write(&Dato, 2);                                       // Envia los primeros 2 bytes del vector Data
      break;
    case 4:                                                        // Puesto final
      Dato[0] = 0b00000100;                                        // Identificador
      Dato[1] = PuestoFinal;                                       // Envia el puesto final de jugador
      Dato[2] = PuntajeObtenido;                                   // Envia el puntaje actual de jugador
      radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de escritura
      radio.stopListening();                                       // Transceptor modo para transmitir
      radio.write(&Dato, 3);                                       // Envia los primeros 3 bytes del vector Data
      break;
    case 5:                                                        // Contesto correctamente
      Dato[0] = 0b00000101;                                        // Identificador
      radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de escritura
      radio.stopListening();                                       // Transceptor modo para transmitir
      radio.write(&Dato, 1);                                       // Envia el primer byte del vector Data
      break;
  }
  radio.startListening();  // Transceptor modo para recibir
}


void EnvioSerial(int var1) {  // Envio de mensajes por comunicacion serial (lado receptor)
  switch (var1) {
    case 0:                   // Solicitar lista de usuarios conectados
      Dato[0] = 0b00000000;   // Identificador
      Serial.write(Dato, 1);  // Envia el primer byte del vector Data
      break;
    case 1:                   // Envia usuario conectado
      Dato[0] = 0b00000001;   // Identificador
      Dato[1] = Usuario;      // Usuario
      Serial.write(Dato, 2);  // Envia los primeros 2 bytes del vector Data
      break;
    case 2:                         // Envia nivel de bateria
      Dato[0] = 0b00000010;         // Identificador
      Dato[1] = Usuario;            // Usuario
      Dato[2] = PorcentajeBateria;  // Bateria
      Serial.write(Dato, 3);        // Envia los primeros 3 bytes del vector Data
      break;
    case 3:                   // Envia pulso de boton
      Dato[0] = 0b00000011;   // Identificador
      Dato[1] = Usuario;      // Usuario
      Serial.write(Dato, 2);  // Envia los primeros 2 bytes del vector Data
      break;
  }
}


bool RecepcionTransmisor() {  // Recepcion de mensajes por Transceptor (lado Transmisor)
  if (radio.available()) {
    radio.read(&Dato, sizeof(Dato));  // Guardar mensaje
    switch (Dato[0]) {
      case 0:  // Recibe usuario asignado
        Usuario = Dato[1];
        Pantallas(4);                                                   // Pantalla espera antes de iniciar el primer juego
        radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));     // Asigna pipe de escritura segun el usuario
        radio.openReadingPipe(0, pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de lectura segun el usuario
        break;
      case 1:                       // Inicia nueva pregunta
        NPregunta = Dato[1];        // Guarda numero de pregunta
        PuntajeObtenido = Dato[2];  // Guarda el puntaje actual de jugador
        PuntajeObtenido = Dato[3];  // Guarda el puntaje que podria obtener con la pregunta
        Pantallas(5);               // Pantalla para iniciar nuevo juego y juego en curso
        break;
      case 2:                // Recibe posicion
        Posicion = Dato[1];  // Guarda la posicion del jugador que presiono el boton
        Pantallas(6);        // Pantalla Posicion del jugador y Turno actual
        break;
      case 3:             // Recibe turno
        Turno = Dato[1];  // Guarda el turno del jugador que debe contestar
        Pantallas(6);     // Pantalla Posicion del jugador y Turno actual
        break;
      case 4:                       // Recibe puesto final
        PuestoFinal = Dato[1];      // Guarda el puesto final de jugador
        PuntajeObtenido = Dato[2];  // Guarda el puntaje actual de jugador
        Pantallas(8);               // Pantalla clasificacion de puestos finales
        break;
      case 5:  // Recibe indicador de que contesto correctamente
        Correcto = 1;
        Pantallas(7);  // Pantalla si el usuario acerto o no la pregunta
        break;
    }
    return 1;  // Si recibe un mensaje devuelve 1
  } else {
    return 0;  // Si no recibe un mensaje devuelve 0
  }
}

bool RecepcionReceptor() {          // Recepcion de mensajes por Transceptor (lado Receptor)
  if (radio.available(&Usuario)) {  // Captura si hay un mensaje disponible y de que pipe proviene
    Serial.write(Usuario);
    radio.read(&Dato, sizeof(Dato));  // Guardar mensaje
    if (Usuario < 6) {
      switch (Dato[0]) {
        case 0:               // Recibe solicitud para asignacion de usuario
          EnvioReceptor(0);   // Envio asignacion de usuario
          if (Usuario < 5) {  // No envia mensaje serial si se desbordan los usuarios
            EnvioSerial(1);   // Envia usuario conectado
            Nconectados++;    // Contador de usuarios conectados
          }
          Pantallas(3);  // Pantalla Receptor
          break;
        case 1:            // Recibe pulso de boton
          EnvioSerial(3);  // Envia pulso de boton
          break;
        case 2:                         // Recibe nivel de bateria
          PorcentajeBateria = Dato[1];  // Guarda nivel de bateria
          EnvioSerial(2);               // Envia nivel de bateria
          break;
        default:
          Serial.write(0b00001111);
          NRFsetup();
          Pantallas(UltimaPantalla);
          break;
      }
    } else {
      Serial.write(0b11110000);
      NRFsetup();
      Pantallas(UltimaPantalla);
    }
  }
}

bool RecepcionSerial() {  // Recepcion de mensajes por comunicacion serial (lado receptor)
  if (Serial.available()) {
    switch (Serial.read()) {
      case 0:                                 // Recibe lista de usuarios conectados
        while (!(Serial.available() > 0)) {}  // Espera a recibir 1 byte
        Dato[0] = Serial.read();
        for (int i = 0; i < 6; i++) {
          if (bitRead(Dato[0], i)) {
            PipeOcupada[i] = 1;  // Guarda cuales pipes estan ocupadas
            Nconectados++;       // Contador de usuarios conectados
          }
        }
        break;
      case 1:                                 // Recibe puntaje jugador
        while (!(Serial.available() > 1)) {}  // Espera a recibir 2 bytes
        Usuario = Serial.read();
        PuntajeObtenido = Serial.read();
        EnvioReceptor(1);  // Iniciar nueva pregunta
        break;
      case 2:                                 // Recibe indicacion para iniciar nueva pregunta
        while (!(Serial.available() > 1)) {}  // Espera a recibir 2 bytes
        NPregunta = Serial.read();
        PuntajeaObtener = Serial.read();
        break;
      case 3:                                 // Recibe posicion del jugador
        while (!(Serial.available() > 1)) {}  // Espera a recibir 2 bytes
        Usuario = Serial.read();
        Posicion = Serial.read();
        UsuariosPresionaron[Usuario];  // Guarda los usuarios que presionaron el boton
        EnvioReceptor(2);              // Envio posicion
        break;
      case 4:                                 // Recibe turno del jugador que debe contestar
        while (!(Serial.available() > 1)) {}  // Espera a recibir 1 byte
        Turno = Serial.read();
        for (int Usuario = 0; Usuario < 6; Usuario++) {
          if (UsuariosPresionaron[Usuario]) {
            EnvioReceptor(3);  // Envio turno
          }
        }
        break;
      case 5:                                 // Recibe indicacion de que el jugador contesto correctamente
        while (!(Serial.available() > 0)) {}  // Espera a recibir 1 byte
        Usuario = Serial.read();
        EnvioReceptor(5);  // Contesto correctamente
        break;
      case 6:                                 // Recibe puesto final del jugador
        while (!(Serial.available() > 2)) {}  // Espera a recibir 3 bytes
        Usuario = Serial.read();
        PuestoFinal = Serial.read();
        PuntajeObtenido = Serial.read();
        EnvioReceptor(4);  // Puesto final
        break;
    }
    return 1;  // Si recibe un mensaje devuelve 1
  } else {
    return 0;  // Si no recibe un mensaje devuelve 0
  }
}

void Pantallas(int var1) {  // Instrucciones para todos los estados de la pantalla OLED

  if (var1 != 1 && var1 != 2 && var1 != 9 && var1 != 10 && var1 != 11 && var1 != 12) {
    UltimaPantalla = var1;
  }

  switch (var1) {  // Casos para las pantallas
    case 0:        // Pantalla de inicio para el Transmisor
      pantallita.clearDisplay();
      pantallita.fillRect(0, 16, 128, 48, WHITE);  // Crea un rectangulo y lo rellena con blanco
      pantallita.setTextColor(BLACK);              // Ajusta el color de texto a negro
      pantallita.setTextSize(2);                   // Ajusta el tamaño del texto
      pantallita.setCursor(0, 32);
      pantallita.setTextWrap(0);  // Permite que el texto se desborde por la pantalla
      pantallita.print(F("TRANSMITTER"));
      pantallita.setTextWrap(1);  // Bloquea que el texto se desborde por la pantalla
      pantallita.setTextSize(1);
      pantallita.setCursor(52, 50);
      pantallita.print(F("mode"));
      pantallita.display();
      break;
    case 1:  // Actualizacion poner "Presiona para conectar"
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print(F("Press 2 Connect"));
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
      pantallita.print(F("RECEIVER"));
      pantallita.setTextWrap(1);
      pantallita.setTextSize(1);
      pantallita.setCursor(52, 50);
      pantallita.print(F("mode"));
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 0);
      pantallita.print(F("Connected: "));
      pantallita.print(Nconectados);
      pantallita.display();
      break;
    case 4:  // Pantalla espera antes de iniciar el primer juego
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print(F("Conectado :D"));
      pantallita.setTextColor(WHITE);
      pantallita.setTextSize(2);
      pantallita.setCursor(10, 32);
      pantallita.print(F("Jugador "));
      pantallita.print(Usuario);
      pantallita.setTextSize(1);
      pantallita.setCursor(7, 50);
      pantallita.print(F("Esperando inicio..."));
      pantallita.display();
      break;
    case 5:  // Pantalla para iniciar nuevo juego y juego en curso
      pantallita.fillRect(0, 0, 128, 16, BLACK);
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      for (int i = 1; i < 32; i++) {  // Animacion iniciando juego
        pantallita.fillCircle(64, 40, i * 3, BLACK);
        pantallita.setCursor(0, 4);
        pantallita.print(F("Iniciando juego."));
      }
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print(F("Juego en curso..."));
      pantallita.fillRect(0, 16, 128, 48, WHITE);
      pantallita.drawFastVLine(64, 16, 48, BLACK);
      pantallita.drawFastHLine(64, 40, 64, BLACK);
      pantallita.setTextColor(BLACK);
      pantallita.setCursor(11, 19);
      pantallita.print(F("Jugador"));
      pantallita.setCursor(73, 17);
      pantallita.print(F("Pregunta"));
      pantallita.setCursor(76, 41);
      pantallita.print(F("PuntajeObtenido"));
      pantallita.setTextSize(4);
      pantallita.setCursor(22, 32);
      pantallita.print(Usuario);
      pantallita.setTextSize(2);
      pantallita.setCursor(94, 25);
      pantallita.print(NPregunta);
      pantallita.setCursor(74, 49);
      pantallita.print(PuntajeObtenido);
      pantallita.display();
      interrupts();
      break;
    case 6:                     // Pantalla Posicion del jugador y Turno actual
      if (Posicion == Turno) {  // Si Posicion del jugador es la misma que el Turno actual
        pantallita.clearDisplay();
        pantallita.setTextSize(1);
        pantallita.setTextColor(WHITE);
        pantallita.setCursor(0, 4);
        pantallita.print(F("Jugador "));
        pantallita.print(Usuario);
        pantallita.setTextSize(3);
        for (int i = 0; i < 5; i++) {  // Animacion si es el TurnoActual del jugador
          pantallita.fillRect(0, 16, 128, 48, BLACK);
          pantallita.setTextColor(WHITE);
          pantallita.setCursor(49, 18);
          pantallita.print(F("TU"));
          pantallita.setCursor(19, 41);
          pantallita.print(F("TURNO"));
          pantallita.display();
          delay(500);
          pantallita.fillRect(0, 16, 128, 48, WHITE);
          pantallita.setTextColor(BLACK);
          pantallita.setCursor(49, 18);
          pantallita.print(F("TU"));
          pantallita.setCursor(19, 41);
          pantallita.print(F("TURNO"));
          pantallita.display();
          delay(500);
        }
      } else {  // Si la Posicion del jugador no es la misma que el Turno actual
        pantallita.clearDisplay();
        pantallita.setTextSize(1);
        pantallita.setTextColor(WHITE);
        pantallita.setCursor(0, 4);
        pantallita.print(F("Ya respondiste :D"));
        pantallita.fillRect(0, 16, 128, 48, WHITE);
        pantallita.drawFastVLine(64, 16, 48, BLACK);
        pantallita.drawFastHLine(64, 40, 64, BLACK);
        pantallita.setTextColor(BLACK);
        pantallita.setCursor(11, 19);
        pantallita.print(F("Jugador"));
        pantallita.setCursor(73, 17);
        pantallita.print(F("Posicion"));
        pantallita.setCursor(76, 41);
        pantallita.print(F("Turno"));
        pantallita.setTextSize(4);
        pantallita.setCursor(22, 32);
        pantallita.print(Usuario);
        pantallita.setTextSize(2);
        pantallita.setCursor(94, 25);
        pantallita.print(Posicion);
        pantallita.setCursor(74, 49);
        pantallita.print(Turno);
        pantallita.display();
      }
      break;
    case 7:  // Pantalla si el usuario acerto o no la pregunta
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      if (Correcto) {                       // Si el usuario es acerto la pregunta
        pantallita.print(PuntajeaObtener);  // Puntos ganados
        pantallita.setTextSize(3);
        pantallita.setTextWrap(0);
        for (int i = 0; i < 5; i++) {
          pantallita.fillRect(0, 16, 128, 48, BLACK);
          pantallita.setTextColor(WHITE);
          pantallita.setCursor(3, 29);
          pantallita.print(F("GANADOR"));
          pantallita.display();
          delay(500);
          pantallita.fillRect(0, 16, 128, 48, WHITE);
          pantallita.setTextColor(BLACK);
          pantallita.setCursor(3, 29);
          pantallita.print(F("GANADOR"));
          pantallita.display();
          delay(500);
        }
        pantallita.setTextWrap(1);
      } else {  // Si el usuario no acerto la pregunta
        pantallita.print(F("Estudie +"));
        pantallita.setTextSize(3);
        pantallita.fillRect(0, 16, 128, 48, WHITE);
        pantallita.setTextColor(BLACK);
        pantallita.setCursor(3, 29);
        pantallita.print(F("BRUTO"));
        pantallita.display();
      }
      break;
    case 8:  // Pantalla clasificacion de puestos finales
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print(F("Puntuacion: "));
      pantallita.print(PuntajeObtenido);
      pantallita.setTextColor(WHITE);
      pantallita.setTextSize(2);
      pantallita.setCursor(28, 17);
      pantallita.print(F("Puesto"));
      pantallita.setTextSize(4);
      pantallita.setCursor(54, 36);
      pantallita.print(PuestoFinal);
      pantallita.display();
      break;
    case 9:  // Pantalla reconectando si pierde la señal
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setTextColor(WHITE);
      pantallita.setCursor(0, 4);
      pantallita.print(F("Desconectado D:"));
      pantallita.setTextSize(2);
      pantallita.setCursor(47, 25);
      pantallita.print(F("RE-"));
      pantallita.setCursor(0, 41);
      pantallita.print(F("CONECTANDO"));
      pantallita.display();
      break;
    case 10:  // Actualizar porcentaje de bateria
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
    case 11:  // Pantalla vacia
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setCursor(0, 0);
      pantallita.setTextColor(WHITE);
      for (int i = 0; i < 84; i++) {  // Rellena toda la pantalla
        pantallita.print(F("<>"));
      }
      pantallita.display();
      break;
    case 12:  //Pantalla de error nrf
      pantallita.clearDisplay();
      pantallita.setTextSize(1);
      pantallita.setCursor(0, 57);
      pantallita.setTextColor(WHITE);
      pantallita.print(F("-Error transceptorNRF"));
      pantallita.display();
      break;
  }
}


void MedirBateria() {  // Mide nivel de bateria
  analogReference(INTERNAL);
  // Se divide entre 10 porque se divide el voltaje a la mitad por las 2 resistencias, osea: 5*2=10
  PorcentajeBateria = map(analogRead(BateriaPin), BateriaMin * 1024.0 / 10, BateriaMax * 1024.0 / 10, 0, 100);
  if (PorcentajeBateria > 100) {
    PorcentajeBateria = 100;
  }
}


void DetectorErrorNRF() {
  if (radio.failureDetected) {
    Pantallas(12);  //Pantalla de error nrf
    delay(500);
    NRFsetup();
  }
}


void NRFsetup() {

  while (!radio.begin()) {
    Pantallas(12);  //Pantalla de error nrf
  }

  radio.setChannel(Canal);                                          // Canal que trabajara el Transceptor
  radio.setPALevel(RF24_PA_MIN);                                    // Potencia de trabajo Maxima 0dbm
  radio.setDataRate(RF24_250KBPS);                                  // Velocidad de 250Kbps
  if (Modo == 0) {                                                  // Modo de transmision
    radio.openReadingPipe(0, pgm_read_word_near(&Pipes[Usuario]));  // Asigna pipe de lecura por defecto
    radio.openWritingPipe(pgm_read_word_near(&Pipes[Usuario]));     // Asigna pipe de escritura por defecto
  } else {                                                          // Modo de recepcion
    radio.openReadingPipe(0, pgm_read_word_near(&Pipes[0]));
    radio.openReadingPipe(1, pgm_read_word_near(&Pipes[1]));
    radio.openReadingPipe(2, pgm_read_word_near(&Pipes[2]));  // Asigna pipe de lectura para los usuarios
    radio.openReadingPipe(3, pgm_read_word_near(&Pipes[3]));
    radio.openReadingPipe(4, pgm_read_word_near(&Pipes[4]));
    radio.openReadingPipe(5, pgm_read_word_near(&Pipes[5]));  // Asigna pipe de lecura por defecto
  }
  radio.startListening();
}


void Pulso() {
  noInterrupts();                         // Desactiva las interrupciones
  if (Presiono) {                         // Al soltar el boton
    TiempoUltimaInterrupcion = millis();  // Actualiza el último tiempo de interrupción
    Presiono = 0;
  } else if (!digitalRead(botonPin) && ((millis() - TiempoUltimaInterrupcion) > 300)) {  // Al presionar el boton
    if (Conectado) {
      EnvioTransmisor(1);
    } else {
      Presiono = 1;
    }
    TiempoUltimaInterrupcion = millis();  // Actualiza el último tiempo de interrupción
    Presiono = 1;
  }
  interrupts();  // Activa las Interrupciones
}


void setup() {
  Serial.begin(baudios);
  Wire.begin();

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
    tone(buzzerPin, pgm_read_word_near(&Tonos[i]));  // Suena el buzzer a una frecuencia con multiplos de 150
  }
  noTone(buzzerPin);  // Detenemos el sonido del buzzer

  delay(1000);
  if (Serial.available()) {                        // Si recibe un byte de 0 en la comunicacion serial...
    Modo = 1;                                      // Se configura en modo de recepcion
    Pantallas(3);                                  // Pantalla Receptor
    while (Serial.available()) { Serial.read(); }  // Vaciar el puerto serial
    EnvioSerial(0);                                // Solicita lista de usuarios en el servidor
    while (!Serial.available()) {}                 // Espera a recibir un mensaje serial
    RecepcionSerial();                             // Recibe la lista de usuarios en el servidor
    Pantallas(3);                                  // Pantalla Receptor

  } else {  // De lo contrario...

    Modo = 0;                                                         // Modo de transmision
    pinMode(botonPin, INPUT_PULLUP);                                  // Entrada con resistencia de PULLUP interna
    attachInterrupt(digitalPinToInterrupt(botonPin), Pulso, CHANGE);  // Configura las interrupciones
    Usuario = 5;                                                      // Para pipe default
  }

  NRFsetup();
}

void loop() {
  if (Modo == 0) {  // Modo de transmision
    unsigned long t_ini = millis();
    Presiono = 0;

    while (!Conectado) {       // Ciclo cuando no esta conectado
      Pantallas(0);            // Pantalla Transmisor
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
      Pantallas(11);
      EnvioTransmisor(0);  // Solicitar asignacion de usuario
      delay(1000);
      Conectado = RecepcionTransmisor();  // Recibe usuario asignado
      Presiono = 0;
    }

    t_ini = millis();
    while (true) {
      if ((millis() - t_ini) > Tbateria) {
        MedirBateria();                  // Mide nivel de bateria
        Conectado = EnvioTransmisor(2);  // Enviar nivel de bateria
        Pantallas(10);                   // Actualizar porcentaje de bateria
        t_ini = millis();
      }

      RecepcionTransmisor();

      if (!Conectado) {
        while (!Conectado) {
          Pantallas(9);  // Pantalla reconectando si pierde la señal
          delay(200);
          Conectado = EnvioTransmisor(2);  // Enviar nivel de bateria hasta recibir ACK
        }
        t_ini = millis();
        Pantallas(UltimaPantalla);  // Vuelve a la ultima pantalla utilizada
      }
    }

  } else {

    while (true) {
      RecepcionSerial();
      RecepcionReceptor();
      DetectorErrorNRF();
    }
  }
}
