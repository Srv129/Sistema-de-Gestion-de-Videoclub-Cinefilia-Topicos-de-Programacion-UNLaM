#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "indice.h"
#include "validacion.h"
#include "funciones.h"

/**********************************************************
*                FUNCIONES MENU                           *
***********************************************************/
char menu(const char m[][TAM_MENU], const char *txt);
char elegir_opcion(const char m[][TAM_MENU], const char *txt, const char *msj);
void generar_Alta (long dniABM, t_indice* indice, const char* path, t_Fecha* fecha_proceso, int(*cmp)(const void*, const void*));
void ingresarFecha(t_Fecha *fProceso);
int es_menor_de_edad(const t_Fecha* nacimiento, const t_Fecha* fecha_actual);
int validar_miembro_alta(t_Miembro* miembro,const t_Fecha* fecha_proceso);
int pedir_datos_miembro (t_Miembro* miembro, t_Fecha* fechaproceso);

// FUNCIONES QUE USO PARA MODIFICAR/LEER EL ARCHIVO
FILE *abrir_archivo(const char *nombre_archivo, const char *modo);
int leer_registro_archivo(FILE *fp, unsigned nro_reg, t_Miembro *reg);
int escribir_registro_archivo(FILE *fp, unsigned nro_reg, const t_Miembro *reg);
int validar_miembro(const t_Miembro *miembro);

//FUNCIONES QUE SI VAN EN MENU
int miembro_modificar(t_indice *indice, FILE *fp, t_Fecha* fecha_proceso);
void miembro_dar_baja(t_reg_indice* reg, t_indice *indice, const char* path, int(*cmp)(const void*, const void*));
int miembro_mostrar_info(const t_indice *indice, FILE *fp);
void miembro_mostrar_datos(const t_Miembro *miembro);

//FUNCIONES DE LISTADOS /e,f
void mostrarMiembrosIndice(FILE *pf, const t_indice *indice, int soloActivos);
int ListadoOrdenadoDNI(const char *nombreArchivo, t_indice *indice);
int ListadoAgrupadoPlan(const char *nombreArchivo, int (*cmp)(const void*, const void*));

#endif // MENU_H_INCLUDED
