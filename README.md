## Alarma SMS con Arduino UNO y Shield SIM900

Alarma SMS monitoreada con Shield SIM900 y Arduino UNO.
Envía un SMS con el mensaje "Alarma Activada" en el momento que el sensor PIR conectado en el PIN 10 es activado.

Además permite desactivar la alarma enviando un SMS con la trama: 0,0,0,0$

Tambien permite controlar la iluminacion del hogar por SMS: 0,1,1,1$ (los últimos tres campos representan el estado lógico de los relays conectados en los pines A0, A1 y A2).

## Desarrolladores

- Joe Pisconte Reyes

## License

BSD license.
