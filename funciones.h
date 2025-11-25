#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include "validacion.h"
#include "indice.h"


#define TAM_LINEA 500
#define TODO_OK 0
#define DUPLICADO 93
#define ERR_ARCHIVO 1
#define ERR_MEMORIA 2
#define ERR_LINEA_LARGA 3
#define TAM_MENU 50


#define ERR_DNI 1001
#define ERR_NAC 1002
#define ERR_SEXO 1003
#define ERR_AFIL 1004
#define ERR_CAT 1005
#define ERR_ULT_CUOTA 1006
#define ERR_ESTADO 1007
#define ERR_PLAN 1008
#define ERR_MAIL 1009
#define ERR_MAIL_TUTOR 1010

typedef int (*TxtABin)(const char* linea, void* reg, const t_Fecha* fechaP);
typedef bool (*EsErrorFatal)(int cod);

int txtABin(const char* linea, void* reg, const t_Fecha* fechaP);
bool esErrorFatal(int cod);

int mostrarArchivoBinario(const char* nombreArchivoBin);
int existeArchivo(const char* path);
void ingresarOpcionMenu(int* cod);

int validarMiembro(t_Miembro*);
int guardarRegistroError(const char* nombreArch, const void* reg, size_t tamElem, const char* mensaje);
char* obtenerMensajeError(int ret);
void ingresoFechaProceso(t_Fecha* fProceso);
char* concatenarFechaConArchivo(char* nombreArch, const t_Fecha* fProceso, const char* extension);
int convertirTxtABin(const char* nombreArchTxt, const char* nombreArchBin, const char* nombreArchError, size_t tamReg, const t_Fecha* fechaP, TxtABin txtABin, EsErrorFatal esErrorFatal);
int buscarEnBin(FILE* pBin, const void* reg);
#endif // FUNCIONES_H_INCLUDED
