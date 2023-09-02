
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <EEPROM.h>


#define EEPROM_SIZE 1024

//#define DEBUG_ALL
#define FORCE_TEMPLATED_NOPS
#include "src/ESP32-USB-Soft-Host.h"
#include "src/usbkbd.h" // KeyboardReportParser

#define PROFILE_NAME "Default Wroom"
#define DP_P0  16  // always enabled
#define DM_P0  17  // always enabled

#define SS_PIN 5
#define RST_PIN 0

#include "variables.h"

/*
  #define DP_P0  16  // always enabled
  #define DM_P0  17
  #define DP_P1  22 // -1 to disable
  #define DM_P1  23 // -1 to disable
  #define DP_P2  18 // -1 to disable
  #define DM_P2  19 // -1 to disable
  #define DP_P3  13 // -1 to disable
  #define DM_P3  15 // -1 to disable
*/
#define DP_P1  -1 // -1 to disable
#define DM_P1  -1 // -1 to disable
#define DP_P2  -1 // -1 to disable
#define DM_P2  -1 // -1 to disable
#define DP_P3  -1 // -1 to disable
#define DM_P3  -1 // -1 to disable

static void my_USB_DetectCB( uint8_t usbNum, void * dev )
{
  sDevDesc *device = (sDevDesc*)dev;
  printf("New device detected on USB#%d\n", usbNum);
  printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
  printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
  printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
  printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
  printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
  printf("desc.idVendor           = 0x%04x\n", device->idVendor);
  printf("desc.idProduct          = 0x%04x\n", device->idProduct);
  printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
  printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
  printf("desc.iProduct           = 0x%02x\n", device->iProduct);
  printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
  printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
  // if( device->iProduct == mySupportedIdProduct && device->iManufacturer == mySupportedManufacturer ) {
  //   myListenUSBPort = usbNum;
  // }
}

static void my_USB_PrintCB(uint8_t usbNum, uint8_t byte_depth, uint8_t* data, uint8_t data_len)
{
  int difNull = 0;
  KB_maxLen = data_len;
  for (int k = 0; k < data_len; k++) {
    KB_buff[k] = data[k];
    if (data[k])difNull = 1;
    //Serial.print(k); Serial.print(":"); Serial.println(data[k]);
  }
  KB_soltado = !difNull;
  if (KB_buff[2])
  {
    KB_accion = 1;
    KB_char = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
    //Serial.println(KB_char);
  }
  if (KB_buff[2] == 82 )KB_subir = 1;
  if (KB_buff[2] == 81 )KB_bajar = 1;
  if (KB_buff[2] == 80 )KB_izq = 1;
  if (KB_buff[2] == 79 )KB_der = 1;
  if (KB_buff[2] == 40 )KB_enter = 1;
  if (KB_buff[2] == 42 )KB_back = 1;
  if (KB_buff[2] == 55 )KB_punto = 1;
  if (KB_buff[0] == 64 && KB_buff[2] == 31)KB_arroba = 1;

  // if( myListenUSBPort != usbNum ) return;
  // printf("in: ");
  //for(int k=0;k<data_len;k++) {
  //  printf("0x%02x ", data[k] );
  //}
  //printf("\n");



}

usb_pins_config_t USB_Pins_Config =
{
  DP_P0, DM_P0,
  DP_P1, DM_P1,
  DP_P2, DM_P2,
  DP_P3, DM_P3
};


MFRC522 rfid(SS_PIN, RST_PIN); // Configura el lector RFID

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C y dimensiones de la pantalla LCD

void leerEEPROM()
{
  int i, j, indice = 1;
  char texto[20];
  for (i = 0; i < MAXregMaterias; i++)
  {

    for (j = 0; j < 10; j++)texto[j] = EEPROM.readByte(indice++);
    nombreMaterias[i] = String(texto);
  }
  for (i = 0; i < MAXregMaterias; i++)
  {
    for (j = 0; j < 10; j++)texto[j] = EEPROM.readByte(indice++);
    correos[i] = String(texto);
  }
  for (i = 0; i < MAXregEstudiantes; i++)
  {
    for (j = 0; j < 10; j++)texto[j] = EEPROM.readByte(indice++);
    nombreEstudiantes[i] = String(texto);
  }
  for (i = 0; i < MAXregEstudiantes; i++)
  {
    for (j = 0; j < 10; j++)texto[j] = EEPROM.readByte(indice++);
    estudianteRFid[i] = String(texto);
  }
  for (j = 0; j < MAXregMaterias; j++)horaIni[j] = EEPROM.readByte(indice++);
  for (j = 0; j < MAXregMaterias; j++)minutoIni[j] = EEPROM.readByte(indice++);
  for (j = 0; j < MAXregMaterias; j++)horaFin[j] = EEPROM.readByte(indice++);
  for (j = 0; j < MAXregMaterias; j++)minutoFin[j] = EEPROM.readByte(indice++);
  for (j = 0; j < MAXregMaterias; j++)
  {
    paralelos[j] = EEPROM.readInt(indice);
    indice+=sizeof(int);
  }

  for (j = 0; j < MAXregMaterias; j++)materiaOcupada[j] = EEPROM.readByte(indice++);
  for (j = 0; j < MAXregEstudiantes; j++)estudianteOcupado[j] = EEPROM.readByte(indice++);
  horaSistema = EEPROM.readByte(indice++); 
  minutoSistema = EEPROM.readByte(indice++);
  segundoSistema = EEPROM.readByte(indice++);

  Serial.print("Bytes leidos en EEPROM: "); Serial.println(indice);

  Serial.println("Materias:");
  for (i = 0; i < MAXregMaterias; i++)
  {
    Serial.print(nombreMaterias[i]); Serial.print(","); Serial.print(paralelos[i]); Serial.print(","); Serial.print(correos[i]); Serial.print(","); Serial.print(horaIni[i]); Serial.print("h"); Serial.print(minutoIni[i]); Serial.print("m-"); Serial.print(horaFin[i]); Serial.print("h"); Serial.println(minutoFin[i]);
  }
  Serial.println("Estudiantes:");

  for (i = 0; i < MAXregEstudiantes; i++)
  {
    Serial.print(nombreEstudiantes[i]); Serial.print(","); Serial.println(estudianteRFid[i]);
  }
  Serial.print("hora del sistema:"); Serial.print(horaSistema); Serial.print("h"); Serial.print(minutoSistema); Serial.print("m"); Serial.print(segundoSistema); Serial.println("s");

}



void guardarEEPROM()
{
  int i, j, indice = 1;
  char texto[20];
  noInterrupts();
  for (i = 0; i < MAXregMaterias; i++)
  {
    nombreMaterias[i].toCharArray(texto, 10);
    for (j = 0; j < 10; j++)EEPROM.writeByte(indice++, texto[j]);
  }
  for (i = 0; i < MAXregMaterias; i++)
  {
    correos[i].toCharArray(texto, 10);
    for (j = 0; j < 10; j++)EEPROM.writeByte(indice++, texto[j]);
  }
  for (i = 0; i < MAXregEstudiantes; i++)
  {
    nombreEstudiantes[i].toCharArray(texto, 10);
    for (j = 0; j < 10; j++)EEPROM.writeByte(indice++, texto[j]);
  }
  for (i = 0; i < MAXregEstudiantes; i++)
  {
    estudianteRFid[i].toCharArray(texto, 10);
    for (j = 0; j < 10; j++)EEPROM.writeByte(indice++, texto[j]);
  }
  for (j = 0; j < MAXregMaterias; j++)EEPROM.writeByte(indice++, horaIni[j]);
  for (j = 0; j < MAXregMaterias; j++)EEPROM.writeByte(indice++, minutoIni[j]);
  for (j = 0; j < MAXregMaterias; j++)EEPROM.writeByte(indice++, horaFin[j]);
  for (j = 0; j < MAXregMaterias; j++)EEPROM.writeByte(indice++, minutoFin[j]);
  for (j = 0; j < MAXregMaterias; j++)
  {
    EEPROM.writeInt(indice, paralelos[j]);
    indice+=sizeof(int);
  }

  for (j = 0; j < MAXregMaterias; j++)EEPROM.writeByte(indice++, materiaOcupada[j]);
  for (j = 0; j < MAXregEstudiantes; j++)EEPROM.writeByte(indice++, estudianteOcupado[j]);
  EEPROM.writeByte(indice++, horaSistema); 
  EEPROM.writeByte(indice++, minutoSistema);
  EEPROM.writeByte(indice++, segundoSistema);
  EEPROM.commit() ;
  interrupts();

  Serial.print("Bytes guardados en EEPROM: "); Serial.println(indice);
  Serial.println("Materias:");
  for (i = 0; i < MAXregMaterias; i++)
  {
    Serial.print(nombreMaterias[i]); Serial.print(","); Serial.print(paralelos[i]); Serial.print(","); Serial.print(correos[i]); Serial.print(","); Serial.print(horaIni[i]); Serial.print("h"); Serial.print(minutoIni[i]); Serial.print("m-"); Serial.print(horaFin[i]); Serial.print("h"); Serial.println(minutoFin[i]);
  }
  Serial.println("Estudiantes:");

  for (i = 0; i < MAXregEstudiantes; i++)
  {
    Serial.print(nombreEstudiantes[i]); Serial.print(","); Serial.println(estudianteRFid[i]);
  }
  Serial.print("hora del sistema:"); Serial.print(horaSistema); Serial.print("h"); Serial.print(minutoSistema); Serial.print("m"); Serial.print(segundoSistema); Serial.println("s");

  

}

void eliminarEEPROM()
{
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
    Serial.println(i);
  }

  EEPROM.end();
  Serial.println("EEPROM Clear");
}


void setup()
{
  Serial.begin(115200);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init();
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("Failed to initialise EEPROM");
  }
  else
  {
    Serial.println("Memoria EEPROM inicializado OK");
  }



  delay(1000);
  printf("USB Soft Host Test for %s\n", PROFILE_NAME );
  printf("TIMER_BASE_CLK: %d, TIMER_DIVIDER:%d, TIMER_SCALE: %d\n", TIMER_BASE_CLK, TIMER_DIVIDER, TIMER_SCALE );
  USH.setTaskCore( 1 );
  // USH.setBlinkPin( (gpio_num_t) 2 );
  // USH.setTaskPriority( 16 );
  USH.init( USB_Pins_Config, my_USB_DetectCB, my_USB_PrintCB );


  if (EEPROM.read(EEPROM_SIZE-2) == 21)
  {
    leerEEPROM();
  }
  else
  {
    EEPROM.write(EEPROM_SIZE-2, 21);
    guardarEEPROM();
  }
}


void manejadorDISP()
{
  if ( menu_opc == 0)
  {
    if (menu_0_cursor == 0)
    {
      LCD_linea0 = ">Reg. Materia      ";
      LCD_linea1 = " Reg. Estudiante      ";
    }
    if (menu_0_cursor == 1)
    {
      LCD_linea0 = ">Reg. Estudiante      ";
      LCD_linea1 = " Eliminar Clase       ";
    }
    if (menu_0_cursor == 2)
    {
      LCD_linea0 = ">Eliminar Clase       ";
      LCD_linea1 = " Eliminar TODO       ";
    }
    if (menu_0_cursor == 3)
    {
      LCD_linea0 = ">Eliminar TODO       ";
      LCD_linea1 = " Hora sistema    ";
    }
    if (menu_0_cursor == 4)
    {
      LCD_linea0 = " Eliminar TODO       ";
      LCD_linea1 = ">Hora sistema    ";
    }
  }

  if ( menu_opc == 10)
  {
    if (menu_10_proces == 0)
    {
      LCD_linea0 = "-NOMBRE MATERIA-";
      LCD_linea1 = nombreMaterias[materias_i];
      LCD_linea1 += "                ";
    }
    if (menu_10_proces == 1)
    {
      LCD_linea0 = "--PAR. MATERIA--";
      LCD_linea1 = paralelos[materias_i];
      LCD_linea1 += "                ";
    }
    if (menu_10_proces == 2 || menu_10_proces == 3 || menu_10_proces == 4 || menu_10_proces == 5 || menu_10_proces == 6)
    {
      LCD_linea0 = "    --HORA--    ";
      LCD_linea1 = " " ;
      if (horaIni[materias_i] < 10)LCD_linea1 += "0";
      LCD_linea1 += horaIni[materias_i];
      LCD_linea1 += "h";
      if (minutoIni[materias_i] < 10)LCD_linea1 += "0";
      LCD_linea1 += minutoIni[materias_i];

      LCD_linea1 += " - " ;
      if (horaFin[materias_i] < 10)LCD_linea1 += "0";
      LCD_linea1 += horaFin[materias_i];
      LCD_linea1 += "h";
      if (minutoFin[materias_i] < 10)LCD_linea1 += "0";
      LCD_linea1 += minutoFin[materias_i];
      LCD_linea1 += "     ";

    }
    if (menu_10_proces == 7)
    {
      LCD_linea0 = "  -- e-mail -- ";
      LCD_linea1 = correos[materias_i];
      LCD_linea1 += "                ";
    }

  }

  if ( menu_opc == 20)
  {
    if (menu_20_proces == 0)
    {
      LCD_linea0 = "Nombre Estudiante";
      LCD_linea1 = nombreEstudiantes[estudiantes_i];
      LCD_linea1 += "                ";
    }
    if (menu_20_proces == 1 || menu_20_proces == 2)
    {
      LCD_linea0 = "Acerque carnet    ";
      LCD_linea1 = estudianteRFid[estudiantes_i];
      LCD_linea1 += "                ";
    }
  }

  if ( menu_opc == 30)// menu de eliminar clase
  {
    if (menu_30_proces == 0 )
    {
      LCD_linea0 = "DEL:";
      LCD_linea0 += nombreMaterias[menu_30_cursor];
      LCD_linea0 += "                 ";

      LCD_linea1 = paralelos[menu_30_cursor];
      LCD_linea1 += " , ";
      if (horaIni[menu_30_cursor] < 10)LCD_linea1 += "0";
      LCD_linea1 += horaIni[menu_30_cursor];
      LCD_linea1 += "h";
      if (minutoIni[menu_30_cursor] < 10)LCD_linea1 += "0";
      LCD_linea1 += minutoIni[menu_30_cursor];

      LCD_linea1 += "-" ;
      if (horaFin[menu_30_cursor] < 10)LCD_linea1 += "0";
      LCD_linea1 += horaFin[menu_30_cursor];
      LCD_linea1 += "h";
      if (minutoFin[menu_30_cursor] < 10)LCD_linea1 += "0";
      LCD_linea1 += minutoFin[menu_30_cursor];
      LCD_linea1 += "          ";
    }
    if (menu_30_proces == 1 )
    {
      LCD_linea0 = "   MATERIA       ";
      LCD_linea1 = "  ELMINADA       ";
    }
    if (menu_30_proces == 2 )
    {
      LCD_linea0 = "NO ES POSIBLE   ";
      LCD_linea1 = "FUERA DE HORA   ";
    }
  }

  if ( menu_opc == 40)// menu de eliminar todo
  {
    if (menu_40_proces == 0 )
    {
      LCD_linea0 = "Ingrese Clave     ";
      LCD_linea1 = contra_ingresada;
      LCD_linea1 += "                ";
    }
    if (menu_40_proces == 1 )
    {
      LCD_linea0 = "BORRADO          ";
      LCD_linea1 = "EXISTOSO         ";
    }
  }

  if ( menu_opc == 50)// menu de la hora del sistema
  {
    if (menu_50_proceso == 0)
      LCD_linea0 = "Hora del sistema";
    else
      LCD_linea0 = "   Set hora     ";
    LCD_linea1 = "  " ;
    if (horaSistema < 10)LCD_linea1 += "0";
    LCD_linea1 += horaSistema;
    LCD_linea1 += "h";
    if (minutoSistema < 10)LCD_linea1 += "0";
    LCD_linea1 += minutoSistema;
    LCD_linea1 += "m";
    if (segundoSistema < 10)LCD_linea1 += "0";
    LCD_linea1 += segundoSistema;
    LCD_linea1 += "s         ";
  }

  if (millis() > timer1)
  {
    timer1 = millis() + 30;
    lcd.setCursor(0, 0);
    lcd.print(LCD_linea0);
    lcd.setCursor(0, 1);
    lcd.print(LCD_linea1);

  }

}

// hora ingresada esta dentro de la hora del sistema
int horaDentro(int horaIni, int minutoIni, int horaFin, int minutoFin)
{
  int minutosDelSistema;
  int minutosIni;
  int minutosFin;
  minutosDelSistema = horaSistema * 60 + minutoSistema;
  minutosIni = horaIni * 60 + minutoIni;
  minutosFin = horaFin * 60 + minutoFin;
  if (minutosDelSistema <= minutosFin && minutosDelSistema >= minutosIni)
  {
    return 1;
  }
  return 0;
}

int materiaBuscarEspacio()
{
  int i;
  for (i = 0; i < MAXregMaterias; i++)
  {
    if (materiaOcupada[i] == 0)
      return i;
  }
  return -1;
}
int estudianteBuscarEspacio()
{
  int i;
  for (i = 0; i < MAXregMaterias; i++)
  {
    if (estudianteOcupado[i] == 0)
      return i;
  }
  return -1;
}


void manejadorMENUS()
{

  if ( menu_opc == 0)
  {
    if (KB_subir)
    {
      KB_subir = 0;
      menu_0_cursor--;
      if (menu_0_cursor < 0)menu_0_cursor = 0;
    }
    if (KB_bajar)
    {
      KB_bajar = 0;
      menu_0_cursor++;
      if (menu_0_cursor > 4)menu_0_cursor = 3;
    }
    if (KB_der)
    {
      KB_der = 0;
      if (menu_0_cursor == 0)
      {
        materias_i = materiaBuscarEspacio();
        KB_enter = 0;
        menu_opc = 10;
        menu_10_proces = 0;
        nombreMaterias[materias_i] = "";

      }
      if (menu_0_cursor == 1)
      {
        KB_enter = 0;
        menu_opc = 20;
        menu_20_proces = 0;
      }
      if (menu_0_cursor == 2)
      {
        menu_opc = 30;
        menu_30_proces = 0;
      }
      if (menu_0_cursor == 3)menu_opc = 40;
      if (menu_0_cursor == 4)//menu de la hora del sistema
      {
        menu_opc = 50;
        menu_50_proceso = 0;
      }

    }
  }
  if ( menu_opc == 10)// menu de registro de materia
  {
    if (menu_10_proces == 0)// ingreso de nombre de materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isAlphaNumeric((char)KB_char) || KB_char == ' ')
        {
          nombreMaterias[materias_i] += (char)KB_char;
        }
      }
      if (KB_enter)
      {
        KB_enter = 0;
        menu_10_proces = 1;
        var_int_1 = 0;
        aNumero = "";
      }
    }
    if (menu_10_proces == 1)// ingreso del paralelo de la materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          paralelos[materias_i] = aNumero.toInt();
        }
      }
      if (KB_enter)
      {
        KB_enter = 0;
        menu_10_proces = 2;
        aNumero = "";
        var_int_1 = 0;
      }
    }
    if (menu_10_proces == 2)// ingreso de la hora INI de la materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          horaIni[materias_i] = aNumero.toInt();
          if (horaIni[materias_i] >= 23)horaIni[materias_i] = 23;
          if (var_int_1 == 2)
          {
            menu_10_proces = 3;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_10_proces == 3)// ingreso de la minutos INI de la materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          minutoIni[materias_i] = aNumero.toInt();
          if (minutoIni[materias_i] >= 59)minutoIni[materias_i] = 59;
          if (var_int_1 == 2)
          {
            menu_10_proces = 4;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_10_proces == 4)// ingreso de la hora FIN de la materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          horaFin[materias_i] = aNumero.toInt();
          if (horaFin[materias_i] >= 23)horaFin[materias_i] = 23;
          if (var_int_1 == 2)
          {
            menu_10_proces = 5;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_10_proces == 5)// ingreso de la minutos FIN de la materia
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          minutoFin[materias_i] = aNumero.toInt();
          if (minutoFin[materias_i] >= 59)minutoFin[materias_i] = 59;
          if (var_int_1 == 2)
          {
            menu_10_proces = 6;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_10_proces == 6)// enter para el siguiente paso que es el ingreso del email
    {
      if (KB_enter)
      {
        KB_enter = 0;
        menu_10_proces = 7;
      }
    }
    if (menu_10_proces == 7)// ingreso del email
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isAlphaNumeric((char)KB_char) || KB_char == ' ' || KB_arroba || KB_punto)
        {

          if (KB_arroba)
          {
            KB_arroba = 0;
            correos[materias_i] += "@";
          }
          else
          {
            correos[materias_i] += (char)KB_char;
          }
          KB_arroba = 0;
          KB_punto = 0;
        }
      }
      if (KB_enter)// finalizacion del ingreso de la materia
      {
        Serial.print("Nombre     :"); Serial.println(nombreMaterias[materias_i]);
        Serial.print("paralelo   :"); Serial.println(paralelos[materias_i]);
        Serial.print("hora inicio:"); Serial.print(horaIni[materias_i]); Serial.print("h"); Serial.println(minutoIni[materias_i]);
        Serial.print("hora Fin:"); Serial.print(horaFin[materias_i]); Serial.print("h"); Serial.println(minutoFin[materias_i]);
        Serial.print("correo     :"); Serial.println(correos[materias_i]);
        materiaOcupada[materias_i] = 1;
        KB_enter = 0;
        menu_10_proces = 0;
        menu_opc = 0;
        aNumero = "";

        guardarEEPROM();

      }
    }
  }

  if ( menu_opc == 20)// menu de registro de estudiante
  {
    if (menu_20_proces == 0)
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isAlphaNumeric((char)KB_char) || KB_char == ' ')
        {
          nombreEstudiantes[estudiantes_i] += (char)KB_char;
        }
      }
      if (KB_enter)
      {
        KB_enter = 0;
        menu_20_proces = 1;
        aNumero = "";
      }
    }
    if (menu_20_proces == 1)
    {
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
      {
        Serial.println("Tarjeta acercada");
        // Leer los datos del RFID y almacenarlos en rfidData
        estudianteRFid[estudiantes_i] = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
          estudianteRFid[estudiantes_i] += rfid.uid.uidByte[i];
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        menu_20_proces = 2;
      }
    }
    if (menu_20_proces == 2)
    {
      if (KB_enter)
      {
        KB_enter = 0;
        menu_20_proces = 0;
        menu_opc = 0;
        estudiantes_i++;
        guardarEEPROM();
      }
    }



  }
  if ( menu_opc == 30)// menu de eliminar clase
  {
    if (menu_30_proces == 0)
    {
      if (KB_subir)
      {
        KB_subir = 0;
        menu_30_cursor--;
        if (menu_30_cursor < 0)menu_30_cursor = 0;
      }
      if (KB_bajar)
      {
        KB_bajar = 0;
        menu_30_cursor++;
        if (menu_30_cursor >= MAXregMaterias)menu_30_cursor = MAXregMaterias - 1;
      }
      if (KB_enter)
      {
        KB_enter = 0;
        if (horaDentro(horaIni[menu_30_cursor], minutoIni[menu_30_cursor], horaFin[menu_30_cursor], minutoFin[menu_30_cursor]))
        {
          menu_30_proces = 1;
          nombreMaterias[menu_30_cursor] = "";
          horaIni[menu_30_cursor] = 0;
          horaFin[menu_30_cursor] = 0;
          minutoIni[menu_30_cursor] = 0;
          minutoFin[menu_30_cursor] = 0;
          materiaOcupada[menu_30_cursor] = 0;
          paralelos[menu_30_cursor] = 0;
          correos[menu_30_cursor] = "";
        }
        else
        {
          menu_30_proces = 2;
        }
      }
    }
    if (menu_30_proces == 1 || menu_30_proces == 2 )
    {
      if (KB_enter)
      {
        KB_enter = 0;
        menu_30_proces = 0;
        menu_opc = 0;
        guardarEEPROM();
      }
    }
  }

  if ( menu_opc == 40)// eliminar todo
   {
    if (menu_40_proceso == 0){
      if (KB_accion)
      {
        KB_accion = 0;
        if (isAlphaNumeric((char)KB_char) || KB_char == ' ')
        {
          contra_ingresada += (char)KB_char;
        }
      }
    }
    if ((KB_enter))
      {
        menu_40_proces = 1;
        KB_enter = 0;
        eliminarEEPROM();
   }
   }
      
  if ( menu_opc == 50)// menu de hora del sistema
  {
    if (menu_50_proceso == 0)
    {
      if (KB_enter)
      {
        KB_enter = 0;
        menu_opc = 0;
      }
      if (KB_der)
      {
        KB_der = 0;
        menu_50_proceso = 1;
        var_int_1 = 0;
      }
    }
    if (menu_50_proceso == 1)
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          horaSistema = aNumero.toInt();
          if (horaSistema >= 23)horaSistema = 23;
          if (var_int_1 == 2)
          {
            menu_50_proceso = 2;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }

    if (menu_50_proceso == 2)
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          minutoSistema = aNumero.toInt();
          if (minutoSistema >= 60)minutoSistema = 59;
          if (var_int_1 == 2)
          {
            menu_50_proceso = 3;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_50_proceso == 3)
    {
      if (KB_accion)
      {
        KB_accion = 0;
        if (isDigit((char)KB_char))
        {
          aNumero += (char)KB_char;
          var_int_1++;
          segundoSistema = aNumero.toInt();
          if (segundoSistema >= 60)segundoSistema = 59;
          if (var_int_1 == 2)
          {
            menu_50_proceso = 4;
            aNumero = "";
            var_int_1 = 0;
          }
        }
      }
    }
    if (menu_50_proceso == 4)
    {
      if (KB_enter)
      {
        KB_enter = 0;
        menu_opc = 0;
        menu_50_proceso = 0;
        guardarEEPROM();
      }
    }

  }
}






void loop() {

  if (millis() > timer_horaSistema)
  {
    timer_horaSistema = millis() + 1000;
    segundoSistema++;
    if (segundoSistema >= 60)
    {
      segundoSistema = 0;
      minutoSistema++;
      if (minutoSistema >= 60)
      {
        minutoSistema = 0;
        horaSistema++;
        if (horaSistema >= 24)
        {
          horaSistema = 0;
        }
      }
    }
  }

  //Serial.println("aa");
  // Manejar lectura de RFID
  //  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
  //    handleRFID();
  //    rfid.PICC_HaltA();
  //    rfid.PCD_StopCrypto1();
  //  }

  // Mostrar menú en la LCD
  //handleMenu();

  manejadorMENUS();
  manejadorDISP();

  // Manejar entrada del teclado
  //  if (KB_accion) {
  //    char tt = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
  //    KB_accion = 0;
  //    Serial.print(tt );
  //    switch (tt) {
  //      case '1':
  //        handleRegisterSubject();
  //        break;
  //      case '2':
  //        handleRegisterStudent();
  //        break;
  //      case '3':
  //        handleDeleteClass();
  //        break;
  //      case '4':
  //        handleDeleteAll();
  //        break;
  //      default:
  //        // Manejar otras teclas o casos no válidos
  //        break;
  //    }
  //  }


}


void handleMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. Registrar Materia");
  lcd.setCursor(0, 1);
  lcd.print("2. Registrar Estudiante");

  if (KB_accion == 0)return;
  //while (!KB_accion) {
  // Esperar hasta que se presione una tecla en el teclado
  //}

  char option = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);;
  KB_accion = 0;

  switch (option) {
    case '1':
      {
        lcd.clear();
        lcd.print("Ingrese el nombre");
        lcd.setCursor(0, 1);
        lcd.print("de la materia:");

        char subjectName[16]; // Para almacenar el nombre de la materia
        int charCount = 0; // Contador de caracteres

        while (!KB_accion) {
          // Esperar hasta que se presione una tecla en el teclado
        }

        while (charCount < 16 && KB_accion) {
          char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
          KB_accion = 0;

          if (key == '\n') {
            subjectName[charCount] = '\0'; // Agregar terminador de cadena
            break;
          } else if (key == '\b' && charCount > 0) {
            charCount--;
            lcd.setCursor(charCount, 1);
            lcd.print(" ");
            lcd.setCursor(charCount, 1);
          } else if (isAlphaNumeric(key)) {
            subjectName[charCount] = key;
            lcd.print(key);
            charCount++;
          }
        }

        // Aquí puedes agregar la lógica para almacenar el nombre de la materia
        // Puedes imprimir un mensaje de éxito o error en la pantalla LCD

        delay(2000); // Mostrar el mensaje por 2 segundos
        lcd.clear();
        break;
      }

    case '2':
      {
        lcd.clear();
        lcd.print("Ingrese los datos");
        lcd.setCursor(0, 1);
        lcd.print("del estudiante:");

        // Aquí puedes leer los datos del estudiante ingresados usando el teclado
        // y luego llamar a la función para registrar al estudiante
        handleRegisterStudent();
        break;
      }

    case '3':
      {
        handleDeleteClass();
        break;
      }

    case '4':
      {
        handleDeleteAll();
        break;
      }

    default:
      {
        lcd.clear();
        lcd.print("Opcion invalida");
        delay(2000); // Mostrar el mensaje por 2 segundos
        lcd.clear();
        break;
      }
  }
}


void handleRFID() {
  char rfidData[16]; // Para almacenar los datos del RFID

  // Leer los datos del RFID y almacenarlos en rfidData
  for (byte i = 0; i < rfid.uid.size; i++) {
    rfidData[i] = rfid.uid.uidByte[i];
  }
  rfidData[rfid.uid.size] = '\0'; // Agregar el terminador de cadena

  lcd.clear();
  lcd.print("Tarjeta RFID:");
  lcd.setCursor(0, 1);
  lcd.print(rfidData);

  // Aquí puedes implementar la lógica para asociar la tarjeta RFID con un estudiante

  delay(2000); // Mostrar los datos por 2 segundos en la pantalla LCD
}


void handleRegisterSubject() {
  lcd.clear();
  lcd.print("Nombre de Materia:");

  char subjectName[16]; // Para almacenar el nombre de la materia
  int charCount = 0; // Contador de caracteres

  while (!KB_accion) {
    // Esperar hasta que se presione una tecla en el teclado
  }

  while (charCount < 16 && KB_accion) {
    char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
    KB_accion = 0;

    if (key == '\n') {
      subjectName[charCount] = '\0'; // Agregar terminador de cadena
      break;
    } else if (key == '\b' && charCount > 0) {
      charCount--;
      lcd.setCursor(charCount, 1);
      lcd.print(" ");
      lcd.setCursor(charCount, 1);
    } else if (isAlphaNumeric(key)) {
      subjectName[charCount] = key;
      lcd.print(key);
      charCount++;
    }
  }

  // Aquí puedes agregar la lógica para almacenar el nombre de la materia
  // Puedes imprimir un mensaje de éxito o error en la pantalla LCD

  delay(2000); // Mostrar el mensaje por 2 segundos
  lcd.clear();
}


void handleRegisterStudent() {
  lcd.clear();
  lcd.print("Nombre:");

  char studentName[16]; // Para almacenar el nombre del estudiante
  int charCount = 0; // Contador de caracteres

  while (!KB_accion) {
    // Esperar hasta que se presione una tecla en el teclado
  }

  while (charCount < 16 && KB_accion) {
    char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
    KB_accion = 0;

    if (key == '\n') {
      studentName[charCount] = '\0'; // Agregar terminador de cadena
      break;
    } else if (key == '\b' && charCount > 0) {
      charCount--;
      lcd.setCursor(charCount, 1);
      lcd.print(" ");
      lcd.setCursor(charCount, 1);
    } else if (isAlphaNumeric(key)) {
      studentName[charCount] = key;
      lcd.print(key);
      charCount++;
    }
  }

  lcd.clear();
  lcd.print("ID:");

  char studentID[10]; // Para almacenar el número de identificación del estudiante
  charCount = 0; // Reiniciar el contador de caracteres

  while (!KB_accion) {
    // Esperar hasta que se presione una tecla en el teclado
  }

  while (charCount < 10 && KB_accion) {
    char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
    KB_accion = 0;

    if (key == '\n') {
      studentID[charCount] = '\0'; // Agregar terminador de cadena
      break;
    } else if (key == '\b' && charCount > 0) {
      charCount--;
      lcd.setCursor(charCount, 1);
      lcd.print(" ");
      lcd.setCursor(charCount, 1);
    } else if (isdigit(key)) {
      studentID[charCount] = key;
      lcd.print(key);
      charCount++;
    }
  }

  // Aquí puedes agregar la lógica para almacenar los datos del estudiante
  // en tu sistema o base de datos. Por ejemplo:
  // registrarEstudiante(studentName, studentID);

  lcd.clear();
  lcd.print("Estudiante");
  lcd.setCursor(0, 1);
  lcd.print("registrado!");
  delay(2000); // Mostrar el mensaje por 2 segundos
  lcd.clear();
}



void handleDeleteClass() {
  lcd.clear();
  lcd.print("Eliminar Clase?");
  lcd.setCursor(0, 1);
  lcd.print("1. Si   2. No");

  int choice = 0;

  while (!KB_accion) {
    // Esperar hasta que se presione una tecla en el teclado
  }

  char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
  KB_accion = 0;

  switch (key) {
    case '1':
      // Aquí puedes agregar la lógica para eliminar la clase o paralelo
      // deleteClase();
      lcd.clear();
      lcd.print("Clase eliminada!");
      break;
    case '2':
      lcd.clear();
      lcd.print("Eliminación cancelada");
      break;
    default:
      lcd.clear();
      lcd.print("Opción inválida");
      break;
  }

  delay(2000); // Mostrar el mensaje por 2 segundos
  lcd.clear();
}


void handleDeleteAll() {
  lcd.clear();
  lcd.print("Ingrese contraseña:");

  char adminPassword[] = "admin123"; // Contraseña de administrador (cámbiala por la correcta)
  char enteredPassword[16]; // Para almacenar la contraseña ingresada
  int charCount = 0; // Contador de caracteres

  while (!KB_accion) {
    // Esperar hasta que se presione una tecla en el teclado
  }

  while (charCount < 16 && KB_accion) {
    char key = keyJK.OemToAscii(KB_buff[0], KB_buff[2]);
    KB_accion = 0;

    if (key == '\n') {
      enteredPassword[charCount] = '\0'; // Agregar terminador de cadena
      break;
    } else if (key == '\b' && charCount > 0) {
      charCount--;
      lcd.setCursor(charCount, 1);
      lcd.print(" ");
      lcd.setCursor(charCount, 1);
    } else if (isAlphaNumeric(key)) {
      enteredPassword[charCount] = key;
      lcd.print('*'); // Mostrar asteriscos en lugar de la contraseña
      charCount++;
    }
  }

  if (strcmp(enteredPassword, adminPassword) == 0) {
    // Aquí puedes agregar la lógica para borrar todos los paralelos registrados
    // deleteAllParalelos();
    lcd.clear();
    lcd.print("Paralelos borrados");
  } else {
    lcd.clear();
    lcd.print("Contraseña incorrecta");
  }

  delay(2000); // Mostrar el mensaje por 2 segundos
  lcd.clear();
}
