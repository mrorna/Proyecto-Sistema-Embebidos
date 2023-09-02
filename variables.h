
KeyboardReportParser keyJK ;
int KB_buff[20];
int KB_accion = 0;
int KB_maxLen;
int KB_char;
int KB_subir, KB_bajar, KB_izq, KB_der, KB_enter, KB_back, KB_arroba, KB_punto;
int KB_soltado;



#define MAXregMaterias 10
#define MAXregEstudiantes 15

String nombreMaterias[MAXregMaterias];
byte horaIni[MAXregMaterias], minutoIni[MAXregMaterias];
byte horaFin[MAXregMaterias], minutoFin[MAXregMaterias];
int materias_i = 0;
int paralelos[MAXregMaterias];
byte materiaOcupada[MAXregMaterias];
String correos[MAXregMaterias];
String contrasena = "espol";
String contra_ingresada = "" ;

String nombreEstudiantes[MAXregEstudiantes];
int estudiantes_i = 0;
String estudianteRFid[MAXregEstudiantes];
byte estudianteOcupado[MAXregEstudiantes];





int disp_mostrar;
int  menu_opc = 0;
int menu_0_cursor = 0;
int menu_10_proces = 0;
int menu_20_proces = 0;
int menu_30_proces = 0;
int menu_40_proces = 0;
int menu_40_proceso = 0;
int menu_30_cursor = 0;
int menu_50_proceso = 0;
String LCD_linea0;
String LCD_linea1;
String aNumero = "";
unsigned long timer1;
int var_int_1 = 0;

byte horaSistema = 0;
byte minutoSistema = 0;
byte segundoSistema = 0;
unsigned long timer_horaSistema;
