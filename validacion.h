#ifndef VALIDACION_H_INCLUDED
#define VALIDACION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stddef.h>

#define TRUE 1
#define FALSE 0
#define MAX_APENOM 61  // 60 caracteres + '\0'
#define MAX_CAT 11     // 10 caracteres + '\0'
#define MAX_PLAN 11    // 10 caracteres + '\0'
#define MAX_EMAIL 31   // 30 caracteres + '\0

typedef struct{
    int dia;
    int mes;
    int anio;
}t_Fecha;

typedef struct {
    long DNI;
    char ApellidosNombres[MAX_APENOM];
    t_Fecha FechaProceso;
    t_Fecha FechaNacimiento;
    char Sexo;
    t_Fecha FechaAfiliacion;
    char Categoria[MAX_CAT];
    char Estado;
    char Plan[MAX_PLAN];
    char EmailTutor[MAX_EMAIL];
    t_Fecha FechaUltimaCuotaPaga;
}t_Miembro;
/**********************************************************
*                         MACROS                          *
***********************************************************/
#define toUpper(X) (((X) >= 'a' && (X) <= 'z') ? (X) - 32 : (X))
#define ES_LETRA(X) ((((X) >= 'a' && (X) <= 'z') || ((X)>= 'A' && (X) <= 'Z')))
/**********************************************************
*                VALIDACION   DATOS                       *
***********************************************************/
int validar_Plan (char plan[10]);
int ValidarDni(long dni);
int ValidarSexo(char s);
int validar_Mail(const char* mail);
int ValidarEstado(char e);
int Validar_Categoria(const char *categoria, const t_Fecha *fechaNac, const t_Fecha *fechaProc);
int Validar_MailTutor(const char *mail, const char *categoria);
int Validar_Afiliacion(const t_Fecha *fechaAfil, const t_Fecha *fechaNac, const t_Fecha *fechaProc);
int Validar_UltimaCuota(const t_Fecha *fechaUltCuota, const t_Fecha *fechaAfil, const t_Fecha *fechaProc);
int Validar_Nacimiento(const t_Fecha *fechaNacimiento, const t_Fecha *fechaProc);
/**********************************************************
**                FUNCIONES STRING                        *
***********************************************************/
int tieneArroba(const char* str);
int validarStringAndTwo(char *str, int tam);
void str_toupper(char* str);
void normalizar_apellido_nombre(t_Miembro *miembro_ptr);
int tiene_nombre_y_apellido(const char* str);
/**********************************************************
*                VALIDACION   FECHA                       *
***********************************************************/
int anioBisiesto(int anio);
t_Fecha difFechas(const t_Fecha *fecIni, const t_Fecha *fecFin);
int validar_Fecha(const t_Fecha *fecha);
int cmpFecha(const t_Fecha f1, const t_Fecha f2);
#endif // VALIDACION_H_INCLUDED
