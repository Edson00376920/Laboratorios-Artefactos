#include <SPI.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================

const int MI_ID = 3;
const int DESTINO_ID = 4;

String mensajeOriginal = "Casa rivas";

// ======================================================
// PINES
// ======================================================

#define MISO  19
#define MOSI  23
#define SCK   18
#define NSS   5

// Definir el pin del sensor Touch y el umbral
const int TOUCH_PIN = 27;
const int UMBRAL_TOUCH = 1000;

// Variable de estado para el sensor Touch (evita múltiples envíos)
bool tocadoAnterior = false;

// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // Iniciar el bus SPI usando los pines definidos
  SPI.begin(SCK, MISO, MOSI, NSS);

  // Indicar a la librería los pines del módulo (NSS, RESET, DIO0)
  LoRa.setPins(NSS, -1, -1);

  if (!LoRa.begin(433E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  // Leer el sensor Touch
  int valorTouch = touchRead(TOUCH_PIN);

  // Detectar el toque (valor menor al umbral)
  if (valorTouch < UMBRAL_TOUCH) {
    if (!tocadoAnterior) {
      enviarMensaje();
      tocadoAnterior = true; 
    }
  } else {
    tocadoAnterior = false; 
  }

  delay(100);
}