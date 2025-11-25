#include <stdio.h>
#include <stdlib.h>
#include "validacion.h"
#include "funciones.h"
#include "indice.h"
#include "menu.h"

int main()
{
    t_Fecha* fechaProceso = malloc(sizeof(t_Fecha));
    t_Miembro* miembro = malloc(sizeof(t_Miembro));
    t_indice* indice = malloc(sizeof(t_indice));
    if(!fechaProceso || !miembro || !indice) {
        return ERR_MEMORIA;
    }

    long dniABM;
    int cod = 0;
    unsigned pos;

    printf("************** ELEGIR OPCIONES **************\n\n");
    printf("Opcion 1: Generar nuevo archivo\n");
    printf("Opcion 2: Buscar archivo con fecha de proceso\n");
    printf("\n*********************************************\n");

    ingresarOpcionMenu(&cod);

    ingresoFechaProceso(fechaProceso);
    char* nombreArchivoError = concatenarFechaConArchivo("error-VC-", fechaProceso, ".txt");
    char* nombreArchivoBin = concatenarFechaConArchivo("miembros-VC-", fechaProceso, ".dat");

    if(cod == 1) {
        printf("\nSe generara un nuevo archivo .dat\n");
        convertirTxtABin("miembros-VC.txt", nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
    } else {
        if(!existeArchivo(nombreArchivoBin)) {
            printf("No existe archivo con la fecha ingresada. Se generara un nuevo archivo .dat\n");
            convertirTxtABin("miembros-VC.txt", nombreArchivoBin, nombreArchivoError, sizeof(t_Miembro), fechaProceso, txtABin, esErrorFatal);
        }
    }

    mostrarArchivoBinario(nombreArchivoBin);

    system("PAUSE");

    indice_crear(indice,CANTIDAD_ELEMENTOS,sizeof(t_reg_indice));
    int flag=indice_cargar(nombreArchivoBin, indice, sizeof(t_reg_indice), comparar_dni);
    if(flag==ERROR)
        system("PAUSE");

    t_reg_indice* regAMB=(t_reg_indice*)indice->vindice;
    t_reg_indice buffer;
    for(int i=0; i<indice->cantidad_elementos_actual; i++)
    {
        printf("DNI: %-8ld\tPOS:%-5i\n", regAMB[i].dni, regAMB[i].nro_reg);
    }

    const char opciones [][TAM_MENU] = {"ABCDEFG",
                                        "Alta de miembro",
                                        "Baja de miembro",
                                        "Modificar miembro",
                                        "Mostrar informacion de un miembro",
                                        "Listado de miembros activos ordenados por DNI",
                                        "Listado de todos los socios agrupados por plan",
                                        "Salir"
                                       };

    char opc;
    do{
        opc=menu(opciones, "Menu Principal");
        switch(opc){
        case 'A':
            printf("\nIngresar DNI para el ALTA: ");
            scanf("%ld", &dniABM);
            if(ValidarDni(dniABM)){
                buffer.dni=dniABM;
                if(indice_buscar(indice,&buffer,sizeof(t_reg_indice),comparar_dni)!=NO_EXISTE){
                    printf("El DNI ya existe en el archivo.\n");
                }
                else{
                    generar_Alta(dniABM,indice,nombreArchivoBin,fechaProceso,comparar_dni);
                }
            }
            else{
                printf("El DNI ingresado no es valido.\n");
            }
            system("PAUSE");
            break;
        case 'B':
            printf("\nIngresar DNI para la BAJA: ");
            fflush(stdout);
            scanf("%ld", &dniABM);
            if(ValidarDni(dniABM)){
                buffer.dni=dniABM;
                pos=indice_buscar(indice,&buffer,sizeof(t_reg_indice),comparar_dni);
                if(pos!=NO_EXISTE){
                    t_reg_indice *arr = (t_reg_indice *)indice->vindice;
                    buffer.nro_reg = arr[pos].nro_reg;
                    miembro_dar_baja(&buffer,indice,nombreArchivoBin,comparar_dni);
                }
                else{
                   printf("El DNI no existe en el archivo.\n");
                }
            }
            else{
                printf("El DNI ingresado no es valido.\n");
            }
            system("PAUSE");
            break;
        case 'C':
            if (indice_vacio(indice) == OK)
                printf("ERROR: El indice esta vacio, no hay miembros para modificar.\n");
            else {
                FILE *fp_c = abrir_archivo(nombreArchivoBin, "r+b");
                if (fp_c) {
                    miembro_modificar(indice, fp_c,fechaProceso);
                    fclose(fp_c);
                    //printf("Archivo binario cerrado despues de la modificacion.\n");
                } else {
                    fprintf(stderr, "ERROR: No se pudo abrir el archivo %s para la modificacion.\n", nombreArchivoBin);
                }
            }
            system("PAUSE");
            break;
        case 'D':
            if (indice_vacio(indice) == OK) { // Corregido: se pasa el puntero 'indice' directamente
                printf("ERROR: El indice esta vacio, no hay miembros para mostrar.\n");
            } else {
                FILE *fp_show = abrir_archivo(nombreArchivoBin, "rb"); // Uso de nombreArchivoBin

                if (fp_show) {
                    miembro_mostrar_info(indice, fp_show); // Se pasa el puntero 'indice'
                    fclose(fp_show);
                    //printf("Archivo binario cerrado despues de la consulta.\n");
                } else {
                    fprintf(stderr, "ERROR: No se pudo abrir el archivo %s para mostrar la informacion.\n", nombreArchivoBin);
                }
            }
            system("PAUSE");
            break;
        case 'E':
            printf("\n LISTADO ORDENADO POR DNI\n");
            ListadoOrdenadoDNI(nombreArchivoBin,indice);
            system("PAUSE");
            break;
        case 'F':
            printf("\n LISTADO AGRUPADO POR PLAN\n");
            ListadoAgrupadoPlan(nombreArchivoBin,comparar_dni);
            system("PAUSE");
            break;
        }
    }while(opc!='G');

    free(fechaProceso);
    free(miembro);
    return 0;
}
