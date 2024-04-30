# Bienvenido
Este espacio está dedicado a todo lo relacionado con Arduino en nuestro proyecto. Aquí, detallamos el hardware utilizado, visible en el diagrama esquemático e informal y en el diseño del montaje físico. Además, incluimos el diseño 3D donde se montó todo este hardware. En segundo lugar, compartimos el software, que corresponde al código que se ejecuta en el Arduino `TX&RX_Code.ino` y cómo interactúa con todo el hardware.

# Reglas de comunicacion
A continuación, encontrarás las reglas que rigen la comunicación inalámbrica y serial entre dispositivos. Estas pautas explican cómo se envían y reciben cada byte de los mensajes intercambiados.

## RX a TX
| Identificador | Byte1 | Byte2 | Byte3 | Byte4 |
| ------------- | ----- | ----- | ----- | ----- |
| Asigna usuario | 0     | Usuario | N/A | N/A |
| Iniciar nueva pregunta | 1     | Numero Pregunta | Puntaje Obtenido | Puntaje a Obtener |
| Envio posición | 2     | Posición | N/A | N/A |
| Envio turno | 3     | Turno | N/A | N/A |
| Puesto final | 4     | PuestoFinal | Puntaje Obtenido | N/A |
| Contesto correctamente | 5     | N/A | N/A | N/A |

## TX a RX
| Identificador | Byte1 | Byte2 |
| ------------- | ----- | ----- |
| Solicitar usuario | 0     | N/A |
| Envia pulso botón | 1     | N/A |
| Envia nivel batería | 2     | Batería |

## RX a PC
| Identificador | Byte1 | Byte2 | Byte3 |
| ------------- | ----- | ----- | ----- |
| Solicitar lista de usuarios conectados | 0     | N/A | N/A |
| Usuario conectado | 1     | Usuario | N/A |
| Envía nivel batería | 2     | Usuario | Batería |
| Envia pulso de botón | 3     | Usuario | N/A |

## PC a RX
| Identificador | Byte1 | Byte2 | Byte3 | Byte4 |
| ------------- | ----- | ----- | ----- | ----- |
| Envia lista de usuarios conectados | 0 | Usuarios Conectados | N/A | N/A |
| Enviar puntaje jugador | 1 | Usuario | Puntaje Obtenido | N/A |
| Iniciar nueva pregunta | 2 | Numero Pregunta | Puntaje a Obtener | N/A |
| Envio posición jugador | 3 | Usuario | Posición | N/A |
| Envio turno actual | 4 | Turno | N/A | N/A |
| Contesto correctamente | 5 | Usuario | N/A | N/A |
| Puesto final | 6 | Usuario | PuestoFinal | PuntajeObtenido |
