// C++ code
//
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

int intensidadDetectando = 255;
int intensidadSinDetectar = 127;
int distanciaDetencion = 150; //distancia en la que se activa el sensor 150cm
int tono = 100;
bool encendidoTono = false;
long tiempoInicioH = 0;
long tiempoFinH = 0;
long tiempoInicioC = 0;
long tiempoFinC = 0;
long tiempoInicioE = 0;
long tiempoFinE = 0;
bool tomoInicioTiempoH = false;
bool tomoInicioTiempoC = false;
bool tomoInicioTiempoE = false;

int memoriaInicializada = 0;
int memoriaActivoPos = 1;
int memoriaSensorHPos = 2;
int memoriaSensorCPos = 3;
int memoriaSensorEPos = 4;
int memoriaSonidoPos = 5;
int memoriaPinPos1 = 6;
int memoriaPinPos2 = 7;
int memoriaPinPos3 = 8;
int memoriaPinPos4 = 9;
int memoriaMinRegistroEventoPos = 10;
int memoriaMaxRegistroEventoPos = 12;
int memoriaRegistroEventoPos = 14;

//info alarma
//posicion|tipo|descripcion
//0|byte|inicializada, por defecto 0, la marcariamos en 1 al pedir la configuracion asi ya no volverlo hacer (en Tinkercad no funciona)
//1|byte|prendido (1) / apagado (0) general
//2|byte|prendido (1) / apagado (0) sensor habitacion
//3|byte|prendido (1) / apagado (0) sensor cocina
//4|byte|prendido (1) / apagado (0) sensor entrada
//5|byte|prendido (1) / apagado (0) modo sonoro
//6|char|primer digito de pin
//7|char|segundo digito de pin
//8|char|tercer digito de pin
//9|char|cuarto digito de pin
//10|int|guarda la posicion donde empieza a registrarse los eventos, ocupa dos direcciones
//11|int|guarda la posicion donde empieza a registrarse los eventos, ocupa dos direcciones
//12|int|guarda la posicion donde termina de registrarse los eventos (esta incluida), ocupa dos direcciones
//13|int|guarda la posicion donde termina de registrarse los eventos (esta incluida), ocupa dos direcciones
//14|int|guarda la posicion del evento ultimo registrado, ocupa dos direcciones
//15|int|guarda la posicion del evento ultimo registrado, ocupa dos direcciones
//16|char|evento 1, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//17|int|evento 1, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//18|int|evento 1, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//19|char|evento 2, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//20|int|evento 2, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//21|int|evento 2, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//22|char|evento 3, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//23|int|evento 3, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//24|int|evento 3, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//25|char|evento 4, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//26|int|evento 4, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//27|int|evento 4, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//28|char|evento 5, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//29|int|evento 5, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//30|int|evento 5, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//31|char|evento 6, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//32|int|evento 6, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//33|int|evento 6, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//34|char|evento 7, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//35|int|evento 7, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//36|int|evento 7, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//37|char|evento 8, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//38|int|evento 8, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//39|int|evento 8, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//40|char|evento 9, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//41|int|evento 9, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//42|int|evento 9, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//43|char|evento 10, lugar del sensor que se activo ('H'=habitacion, 'C'=cocina, 'E'=entrada)
//44|int|evento 10, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//45|int|evento 10, minutos que se activo el sensor correspondiente al lugar de la posicion anterior, ocupa dos direcciones
//siendo evento 1 el primero en suceder, y el 10 el ultimo

const int sonido = 9;

const byte filas=4;
const byte col=4;

char teclas[filas][col]={
    '1','2','3','A',
    '4','5','6','B',
    '7','8','9','C',
    '*','0','#','D'
};

byte pinesFila[filas]={13,12,11,10};
byte pinesCol[col]={8,7,4,2};

Keypad kp = Keypad(makeKeymap(teclas), pinesFila, pinesCol,filas, col);

const byte MAX_CHARS=16;
const byte MAX_ROWS=2;
int posicion=0;
int linea=0;

LiquidCrystal_I2C lcd(0x20,MAX_CHARS, MAX_ROWS);


const byte habitacionSensor = A0;
const byte cocinaSensor = A1;
const byte entradaSensor = A2;
const int habitacionLed = 3;
const int cocinaLed = 5;
const int entradaLed = 6;

void setup() {
  
    //inicializa lcd
    lcd.init();
    
    //prende la luz
    lcd.backlight();

    lcd.clear();
    
    // initialize serial communication:
    Serial.begin(9600);
    
    pinMode(habitacionLed, OUTPUT);
    pinMode(cocinaLed, OUTPUT);
    pinMode(entradaLed, OUTPUT);
    pinMode(sonido, OUTPUT);
 
}

void loop() {

    
    if (EEPROM.read(0)==0){ //En este caso, cuando se inicia la EEPROM esta en 0, porque la EEPROM se reinicia
        configuracionPredeterminanda(); //carga datos predeterminada
                                        //configuracion prodeterminada ya que la EEPROM se reinicia en Tinkercad
                                        //comentar la de abajo si esta se utliza
        //pedirConfiguracion(); //carga datos pidiendolos
                                //configuracion solicitada solo la primera vez
                                //comentar la de arriba si esta se utliza
    }else{
        detectar();

        lcd.setCursor(0,0);
        lcd.println("1-Menu|2-Alarma|");
        lcd.setCursor(0,1);
        lcd.println("3-Eventos");
        
        char key = kp.getKey();

        if (key != NO_KEY){
            if (key=='1'){
                char keyInterna;
                do{
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("1-PIN|2-Sensores");
                    lcd.setCursor(0,1);
                    lcd.print("3-Modo Sonido");
                    do{
                        keyInterna = kp.getKey();
                    }while (keyInterna == NO_KEY || !(keyInterna=='A' || keyInterna=='C' || keyInterna=='1' || keyInterna=='2' || keyInterna=='3'));
                    if (keyInterna=='1'){
                        keyInterna = cambiarPin();
                    }else if (keyInterna=='2'){
                        keyInterna = activarDesactivarSensores();
                    }else if (keyInterna=='3'){
                        keyInterna= activarDesactivarModoSonido();
                    }
                }while(keyInterna == NO_KEY || !(keyInterna=='A' || keyInterna=='C'));
                lcd.clear();
            }
            if (key=='2'){
                activarDesactivarAlarmaGeneral();
                lcd.clear();
            }
            if (key=='3'){
                mostrarEventos();
                lcd.clear();
            }
        }
    }
    
}

long microsecondsToInches(long microseconds) {
    // According to Parallax's datasheet for the PING))), there are 73.746
    // microseconds per inch (i.e. sound travels at 1130 feet per second).
    // This gives the distance travelled by the ping, outbound and return,
    // so we divide by 2 to get the distance of the obstacle.
    // See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
    return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    // The ping travels out and back, so to find the distance of the object we
    // take half of the distance travelled.
    return microseconds / 29 / 2;
}

//si el valor recibo es true -> activa sino descativa
void activarDesactivarAlarma(bool activar){
    if (activar){
        EEPROM.write(memoriaActivoPos, 1);
    }else{
        EEPROM.write(memoriaActivoPos, 0);
    }
}

//si el valor recibo es true -> activa sino descativa
void activarDesactivarSensor(String lugar, bool activar){
    int posicion;
    if (lugar.equals("Habitacion")){
        posicion = memoriaSensorHPos;
    }else if (lugar.equals("Cocina")){
        posicion = memoriaSensorCPos;
    }else {
        posicion = memoriaSensorEPos;
    }

    if (activar){
        EEPROM.write(posicion, 1);
    }else{
        EEPROM.write(posicion, 0);
    }
}

//si el valor recibo es true -> activa sino descativa
void activarDesactivarSonido(bool activar){
    if (activar){
        EEPROM.write(memoriaSonidoPos, 1);
    }else{
        EEPROM.write(memoriaSonidoPos, 0);
    }
}

//regristra evento (lugar y minutos)
//si ya hay 10 registros, elimina el mas viejo
void registrarEvento(String lugar, int minutos){
    int registroEventoPos, minRegistroEventoPos, maxRegistroEventoPos;
    
    EEPROM.get(memoriaRegistroEventoPos, registroEventoPos);
    EEPROM.get(memoriaMinRegistroEventoPos, minRegistroEventoPos);
    EEPROM.get(memoriaMaxRegistroEventoPos, maxRegistroEventoPos);
    char charLugar;

    if (lugar.equals("Habitacion")){
        charLugar = 'H';
    }else if (lugar.equals("Cocina")){
        charLugar = 'C';
    }else {
        charLugar = 'E';
    }
    //para registrar primer evento
    //el inicio de eventos es en la posicion 16
    //si la posicion de registro de eventos es mayor/igual a maxRegistroEventoPos (45)
    //es porque ya registro diez eventos, por lo que quita el evento mas viejo
    if (registroEventoPos < maxRegistroEventoPos){
        EEPROM.put(registroEventoPos, charLugar);
        EEPROM.put(registroEventoPos+1, minutos);
        EEPROM.update(memoriaRegistroEventoPos, registroEventoPos + 3);
    }else{
        //el primer evento, guardado en la posicion 9 y 10 seran pisados por el segundo evento que sucedio
        char LugarMover;
        int minutosMover;

        EEPROM.get(maxRegistroEventoPos-2, LugarMover);
        EEPROM.get(maxRegistroEventoPos-1, minutosMover);

        for (int i=maxRegistroEventoPos; i>minRegistroEventoPos+3; i-=3){
            char lugarBuffer;
            int minutosBuffer;
            
            EEPROM.get(i-5, lugarBuffer);
            EEPROM.get(i-4, minutosBuffer);

            EEPROM.put(i-5, LugarMover);
            EEPROM.put(i-4, minutosMover);

            LugarMover = lugarBuffer;
            minutosMover = minutosBuffer;

        }
        //maxRegistroEventoPos seria igual a 45
        //maxRegistroEventoPos-2=43, ultima posicion para guardar lugar
        //maxRegistroEventoPos-1=44, ultima posicion para guardar minutos
        EEPROM.put(maxRegistroEventoPos-2, charLugar);
        EEPROM.put(maxRegistroEventoPos-1, minutos);
    }
}

//muestra eventos desde el mas antiguo, con B muestra el siguiente con A y C vuelve al menu
char mostrarEventos(){
    int registroEventoPos, minRegistroEventoPos, maxRegistroEventoPos;
    int iniciar;
    char key;
    
    EEPROM.get(memoriaRegistroEventoPos, registroEventoPos);
    EEPROM.get(memoriaMinRegistroEventoPos, minRegistroEventoPos);
    EEPROM.get(memoriaMaxRegistroEventoPos, maxRegistroEventoPos);
  
    if (registroEventoPos < maxRegistroEventoPos){
        iniciar = registroEventoPos-1;
    }else{
        iniciar = maxRegistroEventoPos;
    }

    if (registroEventoPos!=16){
        do{
            char lugar;
            int minutos;
            String lugarCompleto;


            EEPROM.get(iniciar-2, lugar);
            EEPROM.get(iniciar-1, minutos);   
            iniciar = iniciar -3;

            if (lugar=='H'){
                lugarCompleto = "Habitacion";
            }else if (lugar=='C'){
                lugarCompleto = "Cocina";
            }else{
                lugarCompleto = "Entrada";
            }

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Lugar:");
            lcd.setCursor(6,0);
            lcd.print(lugarCompleto);
            lcd.setCursor(0,1);
            lcd.print("Minutos:");
            lcd.setCursor(8,1);
            lcd.print(minutos);

            do{
                key = kp.getKey();
            }while (key==NO_KEY || !(key=='A' || key=='C' || key=='B' ));

        }while(iniciar>minRegistroEventoPos && !(key=='A' || key=='C'));

        if (iniciar<minRegistroEventoPos && key!='A' && key!='C'){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("No mas eventos");
            lcd.setCursor(0,1);
            lcd.print("Volviendo...");
            delay(1000);
        }
    }else{
        lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("No hay eventos");
            lcd.setCursor(0,1);
            lcd.print("Volviendo...");
            delay(1000);
    }

    

    return key;

}

//ingresa pin en la EEPROM, valida que sean 4 digitos, devuelve true si lo inserta correctamente
bool registrarPin(String pin){
    if (pin.length()==4){
        EEPROM.put(memoriaPinPos1, pin.charAt(0));
        EEPROM.put(memoriaPinPos2, pin.charAt(1));
        EEPROM.put(memoriaPinPos3, pin.charAt(2));
        EEPROM.put(memoriaPinPos4, pin.charAt(3));
        return true;
    }else{
        return false;
    }
}

//devuelve true si el pin recibo por parametro es el mismo que la EEPROM
bool validarPin(String pin){
    String pinActualReal = "";
    char c1,c2,c3,c4;
    EEPROM.get(memoriaPinPos1, c1);
    EEPROM.get(memoriaPinPos2, c2);
    EEPROM.get(memoriaPinPos3, c3);
    EEPROM.get(memoriaPinPos4, c4);

    pinActualReal = String(c1) + String(c2) + String(c3) + String(c4);

    if (pinActualReal.equals(pin)){
        return true;
    }else{
        return false;
    }
}

//realiza el cambio de pin validando antes de ingresar el pin en la EEPROM
char cambiarPin(){
    String pinNuevo = "";
    String pinMostrar = "";
    char key;

    key = validacionDePinActual();

    if (key!='A' && key!='C'){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ingrese el pin");
        lcd.setCursor(0,1);
        lcd.print("nuevo");
        pinMostrar = "";
        key = NO_KEY;
        do{
            key = kp.getKey();
            if (key != NO_KEY && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){
                lcd.clear();
                pinNuevo = pinNuevo + key;
                lcd.setCursor(0,0);
                lcd.print("Pin: ");
                lcd.setCursor(0,1);
                pinMostrar = pinMostrar + '*';
                lcd.print(pinMostrar);
            }
            if (key == 'D'){
                pinNuevo = "";
                pinMostrar = "";
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Pin: ");
                lcd.setCursor(0,1);
                lcd.print(pinMostrar);
            }
            if (key == '#'){
                if (registrarPin(pinNuevo)){
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("PIN modificado");
                    lcd.setCursor(0,1);
                    lcd.print("volviendo...");
                    delay(1000);
                }else{
                    key=NO_KEY;
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("El PIN nuevo");
                    lcd.setCursor(0,1);
                    lcd.print("No es de 4 dig.");
                    pinNuevo = "";
                    pinMostrar = "";
                }
            }
        } while (key==NO_KEY || !(key=='#' || key=='A' || key=='C'));
    }

    if (key=='A'){
        key = NO_KEY;
    }
    return key;

}

//dado que le EEPROM se reinicia en Tinkercad en cada ejecucion, este metodo simulara que la EEPROM ya tiene informarcion
void configuracionPredeterminanda(){
    EEPROM.write(memoriaInicializada,1);
    EEPROM.put(memoriaRegistroEventoPos, 16); //ingresa el valor en donde empezar a insertar eventos
    EEPROM.put(memoriaMinRegistroEventoPos, 16); //ingresa el valor en donde estaria el primer registro de evento
    EEPROM.put(memoriaMaxRegistroEventoPos, 45); //ingresa el valor en donde estaria el ultimo registro de evento
    registrarPin("1234");

    activarDesactivarAlarma(true);
    activarDesactivarSensor("Habitacion", true);
    activarDesactivarSensor("Cocina", true);
    activarDesactivarSensor("Entrada", true);
    activarDesactivarSonido(true);
    registrarEvento("Habitacion", 2); //evento 1
    registrarEvento("Cocina", 1); //evento 2
    registrarEvento("Entrada", 4); //evento 3
    registrarEvento("Habitacion", 1); //evento 4
    registrarEvento("Habitacion", 2); //evento 5
    registrarEvento("Cocina", 4); //evento 6
    registrarEvento("Entrada", 1); //evento 7
    registrarEvento("Entrada", 5); //evento 8
    registrarEvento("Entrada", 2); //evento 9

    encenderApagarLeds(intensidadSinDetectar);
}

//activa o desactiva los sensores segun sea recibo por el teclado
char activarDesactivarSensores(){
    char key;
    byte valorSensor;
    String lugar;

    do{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("1-Habitacion|");
        lcd.setCursor(0,1);
        lcd.print("2-Cocina|3-Entrada");

        do{
            key = kp.getKey();
        } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' || key == '1' || key == '2' || key == '3'));
        
        if (key != 'A' && key != 'C'){
            char keyInterna;
            if (key=='1'){
                valorSensor = EEPROM.read(memoriaSensorHPos);
                lugar = "Habitacion";
            }else if (key=='2'){
                valorSensor = EEPROM.read(memoriaSensorCPos);
                lugar = "Cocina";
            }else if (key=='3'){
                valorSensor = EEPROM.read(memoriaSensorEPos);
                lugar = "Entrada";
            }

            if (key=='1' || key=='2' || key=='3'){
                bool activar;
                if (valorSensor==1){
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Activado");
                    lcd.setCursor(0,1);
                    lcd.print("# para cambiar");
                    activar=false;
                }else{
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Desactivado");
                    lcd.setCursor(0,1);
                    lcd.print("# para cambiar");
                    activar=true;
                }  
                do{
                    keyInterna = kp.getKey();
                } while (keyInterna == NO_KEY || !(keyInterna == '#' || keyInterna == 'A' || keyInterna == 'C' ));
                if (keyInterna == '#'){
                    activarDesactivarSensor(lugar, activar);
                    encenderApagarLeds(intensidadSinDetectar);
                    detectar();
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Cambiado");
                    lcd.setCursor(0,1);
                    lcd.print("Volviendo...");
                    delay(1000);
                }else{
                    if (keyInterna == 'A'){
                        key = NO_KEY;
                    }else{
                        key = keyInterna;
                    }
                }
            }
        }

    } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' ));

    if (key=='A'){
        key = NO_KEY;
    }   
    return key;

}

//activa o desactiva el sonido segun lo que sea recibo por el teclado
char activarDesactivarModoSonido(){
    char key;
    byte valorSensor;

    do{
        valorSensor = EEPROM.read(memoriaSonidoPos);
           
        bool activar;
        if (valorSensor==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Activado");
            lcd.setCursor(0,1);
            lcd.print("# para cambiar");
            activar=false;
        }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Desactivado");
            lcd.setCursor(0,1);
            lcd.print("# para cambiar");
            activar=true;
        }  
        do{
            key = kp.getKey();
        } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' ));
        if (key == '#'){
            activarDesactivarSonido(activar);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Cambiado");
            lcd.setCursor(0,1);
            lcd.print("Volviendo...");
            delay(1000);
        }

    } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' ));

    if (key=='A'){
        key = NO_KEY;
    }
    return key;

}

//pide pin por el teclado y valida que sea el actual, devuelve un char, # si es correcto, A para volver atras, C para salir
char validacionDePinActual(){
    String pinActual = "";
    String pinMostrar = "";
    char key;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ingrese el pin");
    lcd.setCursor(0,1);
    lcd.print("actual");
    do{
        key = kp.getKey();
        if (key != NO_KEY && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){
            lcd.clear();
            pinActual = pinActual + key;
            lcd.setCursor(0,0);
            lcd.print("Pin: ");
            lcd.setCursor(0,1);
            pinMostrar = pinMostrar + '*';
            lcd.print(pinMostrar);
        }
        if (key == 'D'){
            pinActual = "";
            pinMostrar = "";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Pin: ");
            lcd.setCursor(0,1);
            lcd.print(pinMostrar);
        }
        if (key=='#'){
            if (!validarPin(pinActual)){
                key=NO_KEY;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Pin no valido");
                pinActual = "";
                pinMostrar = "";
            }
        }
    }while (key==NO_KEY || !(key=='#' || key=='A' || key=='C'));

    return key;
}

//activa o desactiva alarma general segun lo que se recibe por pantalla
char activarDesactivarAlarmaGeneral(){
    char key;
    byte valorSensor;

    do{
        valorSensor = EEPROM.read(memoriaActivoPos);
           
        bool activar;
        if (valorSensor==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Activado");
            lcd.setCursor(0,1);
            lcd.print("# para cambiar");
            activar=false;
        }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Desactivado");
            lcd.setCursor(0,1);
            lcd.print("# para cambiar");
            activar=true;
        }  
        do{
            key = kp.getKey();
        } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' ));
        if (key == '#'){
            char keyValidacion = validacionDePinActual();
            if (keyValidacion == '#'){ //si la validacion fue exitosa #
                if (activar){
                    encenderApagarLeds(intensidadSinDetectar);
                }else{
                    DesactivarEjecucion();
                }
                activarDesactivarAlarma(activar);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Cambiado");
                lcd.setCursor(0,1);
                lcd.print("Volviendo...");
                delay(1000);
            }else{
                if (keyValidacion!='C'){
                    key = NO_KEY;
                }
            }
        }

    } while (key == NO_KEY || !(key == '#' || key == 'A' || key == 'C' ));

    return key;

}

//finaliza la toma de datos de los sensores, se utiliza cuando se apaga la alarma
void DesactivarEjecucion(){
    encenderApagarLeds(0);
    noTone(sonido);
    finalizarEjecucion();
    encendidoTono = false;
}

//si es la primera que vez que inicia la alarma se debe configurar una contraseña y los sensores, sonido y alarma
void pedirConfiguracion(){
    String pin = "";
    String pinMostrar = "";
    char key;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ingrese el pin");
    lcd.setCursor(0,1);
    lcd.print("que utilizara");
    do{
        key = kp.getKey();
        if (key != NO_KEY && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){
            lcd.clear();
            pin = pin + key;
            lcd.setCursor(0,0);
            lcd.print("Pin: ");
            lcd.setCursor(0,1);
            pinMostrar = pinMostrar + '*';
            lcd.print(pinMostrar);
        }
        if (key == 'D'){
            //comentado porque en tinkercad no funciona sino
            //pin = "";
            //pinMostrar = "";
            //lcd.clear();
            //lcd.setCursor(0,0);
            //lcd.print("Pin: ");
            //lcd.setCursor(0,1);
            //lcd.print(pinMostrar);
        }
        if (key=='#'){
            if (!registrarPin(pin)){
                key=NO_KEY;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Pin no valido");
                pin = "";
                pinMostrar = "";
            }
        }
    } while (key == NO_KEY || key != '#');

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HabitacionSensor");
    //Activacion sensor habitacion
    lcd.setCursor(0,1);
    lcd.print("1-Si|2-No");

    do{
        key = kp.getKey();       
    } while (key == NO_KEY || !(key == '1' || key == '2'));

    if (key=='1'){
        activarDesactivarSensor("Habitacion", true);
    }else{
        activarDesactivarSensor("Habitacion", false);
    }


    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cocina sensor");
    //Activacion sensor cocina
    lcd.setCursor(0,1);
    lcd.print("1-Si|2-No");

    do{
        key = kp.getKey();       
    } while (key == NO_KEY || !(key == '1' || key == '2'));

    if (key=='1'){
        activarDesactivarSensor("Cocina", true);
    }else{
        activarDesactivarSensor("Cocina", false);
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Entrada sensor");
    //Activacion sensor Entrada
    lcd.setCursor(0,1);
    lcd.print("1-Si|2-No");

    do{
        key = kp.getKey();      
    } while (key == NO_KEY || !(key == '1' || key == '2'));

    if (key=='1'){
        activarDesactivarSensor("Entrada", true);
    }else{
        activarDesactivarSensor("Entrada", false);
    }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sonido");
    //Activacion sonido
    lcd.setCursor(0,1);
    lcd.print("1-Si|2-No");

    do{
        key = kp.getKey();       
    } while (key == NO_KEY || !(key == '1' || key == '2'));

    if (key=='1'){
        activarDesactivarSonido(true);
    }else{
        activarDesactivarSonido(false);
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarma");
    //Activacion Alarma general
    lcd.setCursor(0,1);
    lcd.print("1-Si|2-No");

    do{
        key = kp.getKey();       
    } while (key == NO_KEY || !(key == '1' || key == '2'));

    if (key=='1'){
        activarDesactivarAlarma(true);
    }else{
        activarDesactivarAlarma(false);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Espere mientras");
    lcd.setCursor(0,1);
    lcd.print("se configura");

    EEPROM.put(memoriaRegistroEventoPos, 16); //ingresa el valor en donde empezar a insertar eventos
    EEPROM.put(memoriaMinRegistroEventoPos, 16); //ingresa el valor en donde estaria el primer registro de evento
    EEPROM.put(memoriaMaxRegistroEventoPos, 45); //ingresa el valor en donde estaria el ultimo registro de evento
    EEPROM.write(memoriaInicializada,1);
    delay(2000);
    encenderApagarLeds(intensidadSinDetectar);
    lcd.clear();
   
}

//detecta si algun sensor detecta presencia, devuelve un String con el lugar donde se detecto, sino ""
void detectar(){
    long durationH, cmH;
    long durationC, cmC;
    long durationE, cmE;
    byte activoG, activoH, activoC, activoE;
    bool detecto = false;

    activoG = EEPROM.read(memoriaActivoPos);
    activoH = EEPROM.read(memoriaSensorHPos);
    activoC = EEPROM.read(memoriaSensorCPos);
    activoE = EEPROM.read(memoriaSensorEPos);

    if (activoG==1){
        if (activoH==1){

            pinMode(habitacionSensor, OUTPUT);
            digitalWrite(habitacionSensor, LOW);
            delayMicroseconds(2);
            digitalWrite(habitacionSensor, HIGH);
            delayMicroseconds(5);
            digitalWrite(habitacionSensor, LOW);

            pinMode(habitacionSensor, INPUT);
            durationH = pulseIn(habitacionSensor, HIGH);

            cmH = microsecondsToCentimeters(durationH);

            if (cmH < distanciaDetencion){
                encenderApagarLed("Habitacion", intensidadDetectando);
                if (!(tomoInicioTiempoH)) {
                    tiempoInicioH = millis();
                    tomoInicioTiempoH = true;
                }
                tiempoFinH = millis();
                detecto = true;
            }else{
                encenderApagarLed("Habitacion", intensidadSinDetectar);
                if (tomoInicioTiempoH) {
                    long tiempo = tiempoFinH - tiempoInicioH;
                    int minutos = tiempo / 60000;
                    registrarEvento("Habitacion", minutos);
                    tomoInicioTiempoH = false;
                }
            }
        }else{
            if (tomoInicioTiempoH) {
                long tiempo = tiempoFinH - tiempoInicioH;
                int minutos = tiempo / 60000;
                registrarEvento("Habitacion", minutos);
                tomoInicioTiempoH = false;
            }
        }
        if (activoC==1){
        
            pinMode(cocinaSensor, OUTPUT);
            digitalWrite(cocinaSensor, LOW);
            delayMicroseconds(2);
            digitalWrite(cocinaSensor, HIGH);
            delayMicroseconds(5);
            digitalWrite(cocinaSensor, LOW);

            pinMode(cocinaSensor, INPUT);
            durationC = pulseIn(cocinaSensor, HIGH);

            cmC = microsecondsToCentimeters(durationC);

            if (cmC < distanciaDetencion){
                encenderApagarLed("Cocina", intensidadDetectando);
                if (!(tomoInicioTiempoC)) {
                    tiempoInicioC = millis();
                    tomoInicioTiempoC = true;
                }
                tiempoFinC = millis();
                detecto = true;
            }else{
                encenderApagarLed("Cocina", intensidadSinDetectar);
                if (tomoInicioTiempoC) {
                    long tiempo = tiempoFinC - tiempoInicioC;
                    int minutos = tiempo / 60000;
                    registrarEvento("Cocina", minutos);
                    tomoInicioTiempoC = false;
                }
            }
        }else{
            if (tomoInicioTiempoC) {
                long tiempo = tiempoFinC - tiempoInicioC;
                int minutos = tiempo / 60000;
                registrarEvento("Cocina", minutos);
                tomoInicioTiempoC = false;
            }    
        }
        if (activoE==1){
        
            pinMode(entradaSensor, OUTPUT);
            digitalWrite(entradaSensor, LOW);
            delayMicroseconds(2);
            digitalWrite(entradaSensor, HIGH);
            delayMicroseconds(5);
            digitalWrite(entradaSensor, LOW);

            pinMode(entradaSensor, INPUT);
            durationE = pulseIn(entradaSensor, HIGH);

            cmE = microsecondsToCentimeters(durationE);

            if (cmE < distanciaDetencion){
                encenderApagarLed("Entrada", intensidadDetectando);
            if (!(tomoInicioTiempoE)) {
                    tiempoInicioE = millis();
                    tomoInicioTiempoE = true;
                }
                tiempoFinE = millis();
                detecto = true;
            }else{
                encenderApagarLed("Entrada", intensidadSinDetectar);
                if (tomoInicioTiempoE) {
                    long tiempo = tiempoFinE - tiempoInicioE;
                    int minutos = tiempo / 60000;
                    registrarEvento("Entrada", minutos);
                    tomoInicioTiempoE = false;
                }
            }
        }else{
            if (tomoInicioTiempoE) {
                long tiempo = tiempoFinE - tiempoInicioE;
                int minutos = tiempo / 60000;
                registrarEvento("Entrada", minutos);
                tomoInicioTiempoE = false;
            }  
        }
    }
    if (detecto){
        //encenderApagarSonido(true);
    }else{
        //encenderApagarSonido(false);
    }
}

//enciende todos los leds con la intensidad recibida, siempre que el sensor correspondiente este activado, sino apaga.
void encenderApagarLeds(int intensidad){
    byte activoG, activoH, activoC, activoE;

    activoG = EEPROM.read(memoriaActivoPos);
    activoH = EEPROM.read(memoriaSensorHPos);
    activoC = EEPROM.read(memoriaSensorCPos);
    activoE = EEPROM.read(memoriaSensorEPos);
    if (activoG==1){
        if (activoH==1){
            analogWrite(habitacionLed, intensidad);
        }else{
            analogWrite(habitacionLed, 0);
        }
        if (activoC==1){
            analogWrite(cocinaLed, intensidad);
        }else{
            analogWrite(cocinaLed, 0);
        }
        if (activoE==1){
            analogWrite(entradaLed, intensidad);
        }else{
            analogWrite(entradaLed, 0);
        }
    }else{
        analogWrite(habitacionLed, 0);
        analogWrite(cocinaLed, 0);
        analogWrite(entradaLed, 0);
    }
}

//enciende el led con la intesidad recibida del lugar recibido, siempre que el sensor correspondiente este activado, sino apaga.
void encenderApagarLed(String lugar,int intensidad){
    byte activoG, activo;
    int posicion, pin;

    activoG = EEPROM.read(memoriaActivoPos);

    if (lugar.equals("Habitacion")){
        posicion = memoriaSensorHPos;
        pin = habitacionLed;
    }else if (lugar.equals("Cocina")){
        posicion = memoriaSensorCPos;
        pin = cocinaLed;
    }else{
        posicion = memoriaSensorEPos;
        pin = entradaLed;
    }

    activo = EEPROM.read(posicion);

    if (activoG==1){
        if (activo==1){
            analogWrite(pin, intensidad);
        }else{
            analogWrite(pin, 0);
        }
    }
}

//enciende el sonido solo si el modo no es silencioso, sino apaga
void encenderApagarSonido(bool activar){
    byte activoSonido;

    activoSonido = EEPROM.read(memoriaSonidoPos);

    if (activoSonido && activar){
        tone(sonido, tono);
    }else{
        noTone(sonido);
    }
    
}

//registra eventos al finalizar la ejecucion si es detenida por desactivar la alarma
void finalizarEjecucion(){
    
    if (tomoInicioTiempoH) {
        tiempoFinH = millis();
        long tiempo = tiempoFinH - tiempoInicioH;
        int minutos = tiempo / 60000;
        registrarEvento("Habitacion", minutos);
        tomoInicioTiempoC = false;
        tiempoInicioH = 0;
        tiempoFinH = 0;
    }
          
    if (tomoInicioTiempoC) {
        tiempoFinC = millis();
        long tiempo = tiempoFinC - tiempoInicioC;
        int minutos = tiempo / 60000;
        registrarEvento("Cocina", minutos);
        tomoInicioTiempoC = false;
        tiempoInicioC = 0;
        tiempoFinC = 0;
    }
    if (tomoInicioTiempoE) {
        tiempoFinE = millis();
        long tiempo = tiempoFinE - tiempoInicioE;
        int minutos = tiempo / 60000;
        registrarEvento("Entrada", minutos);
        tomoInicioTiempoE = false;
        tiempoInicioE = 0;
        tiempoFinE = 0;
    }

}
