#include "indice.h"
#include "validacion.h"
/**
void indice_crear (t_indice*):
toma memoria para 100 elementos e inicializa la estructura a índice vacío.
**/
void indice_crear (t_indice *indice, size_t nmemb, size_t tamanyo){
    indice->vindice=malloc(tamanyo*nmemb);
    if(!indice->vindice){
        printf("ERROR AL CREAR EL INDICE...");
        return;
    }
    indice->cantidad_elementos_maxima = nmemb;
    indice->cantidad_elementos_actual=0;
}
/**
Aumenta la capacidad del indice un 30 por ciento
**/
void indice_redimensionar (t_indice *indice, size_t nmemb, size_t tamanyo){
    void *temp;
    size_t inc = (indice->cantidad_elementos_maxima*nmemb);
    temp = realloc(indice->vindice,tamanyo*inc);
    if(!temp){
        printf("ERROR AL REDIMENSIONAR EL INDICE...");
        return;
    }
    indice->cantidad_elementos_maxima=inc;
    indice->vindice=temp;
}
/**
int indice_insertar (t_indice*, const t_reg_indice*):
inserta en orden según el dni. Si no hay lugar en el array,
toma memoria para un 30 % más de lo que tenía
**/
int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    int pos=indice_buscar(indice,registro,tamanyo,cmp);
    if(pos!=-1)
        return ERROR; //ya existe, no se puede insertar
    if(indice_lleno(indice)){
        indice_redimensionar(indice, INCREMENTO, tamanyo);
    }
    char* base=(char*)indice->vindice;
    size_t i = indice->cantidad_elementos_actual;
    while(i>0&&cmp(registro,base+(i-1)*tamanyo)<0){
        memcpy(base+(i)*tamanyo,base+(i-1)*tamanyo,tamanyo);
        i--;
    }
    memcpy(base+i*tamanyo,registro,tamanyo);
    indice->cantidad_elementos_actual++;
    return OK;
}
/**int indice_buscar (const t_indice*, t_reg_indice* reg_indice):
si el DNI existe deja el registro en reg_ind.
**/
int indice_buscar (const t_indice* indice, const void* registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    if(indice->cantidad_elementos_actual==0)
        return NO_EXISTE;
    size_t low = 0, high = indice->cantidad_elementos_actual-1, mid;
    int comp_result;
    while(low <= high){
        mid = (low+high)/2;
        void *actual = (char *)indice->vindice + mid * tamanyo;
        comp_result = cmp(registro, actual);
        if(comp_result == 0){
            return (int)mid;
        }
        else if(comp_result<0){
            if(mid==0)break;
            high=mid-1;
        }else{
            low=mid+1;
        }
    }
    return NO_EXISTE;
}
/**
int indice_eliminar (t_indice*, t_reg_indice*):
elimina el DNI informado.
**/
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void*, const void*)){
    int pos=indice_buscar(indice,registro,tamanyo,cmp);
    if(pos==-1)
        return ERROR;
    char* base=(char*)indice->vindice;
    memmove(base+pos*tamanyo,base+(pos+1)*tamanyo,(indice->cantidad_elementos_actual-pos-1)*tamanyo);
    indice->cantidad_elementos_actual--;
    return OK;
}
/**int indice_vacio(const t_indice *): indica se el índice está vacío.
**/
int indice_vacio(const t_indice *indice){
    if(indice->cantidad_elementos_actual==0)
        return OK;
    return VACIO;
}
/**
int indice_lleno(const t_indice *, unsigned tamDato):
indica si ya no se pueden añadir más registros.
**/
int indice_lleno(const t_indice *indice){
    if(indice->cantidad_elementos_actual==indice->cantidad_elementos_maxima)
        return OK;
    return LLENO;
}
/**
void indice_vaciar(t_indice*):
libera la memoria utilizada por el índice.
**/
void indice_vaciar(t_indice* indice){
    if(indice->vindice!=NULL){
        free(indice->vindice);
        indice->vindice=NULL;
    }
    indice->cantidad_elementos_actual=0;
    indice->cantidad_elementos_maxima=0;
}
/**
int indice_cargar (t_indice*, const char* path):
Carga el índice desde un archivo.
**/
int indice_cargar(const char* path, t_indice *indice, size_t tamanyo, int(*cmp)(const void*, const void*)){
    FILE* archbin = fopen(path,"rb");
    if(!archbin){
        printf("ERROR LA ABRIR EL ARCHIVO BINARIO...");
        return ERROR;
    }

    t_Miembro miembro;
    size_t i=0;

    if(!fread(&miembro,sizeof(t_Miembro),1,archbin)){
        printf("EL ARCHIVO NO CONTIENE DATOS...");
        return ERROR;
    }

    t_reg_indice registro;
    registro.nro_reg = 0;

    while(!feof(archbin)){
        registro.dni = miembro.DNI;

        if(indice_lleno(indice)){
           indice_redimensionar(indice,INCREMENTO,tamanyo);
        }

        if(indice_buscar(indice, &registro, tamanyo, cmp) != NO_EXISTE) {
            // Si es duplicado entonces ...
            fread(&miembro, sizeof(t_Miembro), 1, archbin); // ... lee el siguiente registro
        } else {
            ((t_reg_indice*)indice->vindice)[i].nro_reg=i; // Setea el nro_reg en 0 al principio del while
            ((t_reg_indice*)indice->vindice)[i].dni=miembro.DNI; // El primer DNI leido es almacenado en el vector indice
            registro.dni = miembro.DNI; // Almaceno en una variable el DNI para luego buscarlo
            i++; // Iteracion
            indice->cantidad_elementos_actual++; // Aumenta la cantidad de elementos

            fread(&miembro, sizeof(t_Miembro), 1, archbin); // Lee el siguiente registro
        }
    }

    Bubble_Sort_Gen(indice->vindice,indice->cantidad_elementos_actual,tamanyo,cmp); // Ordena el vector indice por DNI
    fclose(archbin);
    return OK;
}
/**
Ordenamiento burbuja
**/
void Bubble_Sort_Gen (void *v, size_t ce, size_t tam, int (*cmp)(const void*,const void*))
{
    int i, j;
    for(i=0; i<ce-1; i++)
    {
        for(j=0; j<ce-1-i; j++)
        {
            if(cmp(v+(j*tam),v+((j+1)*tam))>0)
                IntercambioDinamicoGenerico(v+(j*tam),v+((j+1)*tam),tam);
        }
    }
}
/**
Intercambia dos datos
**/
void IntercambioDinamicoGenerico (void *a, void *b, size_t tam)
{
    char *aux = (char*)malloc(tam);
    if(aux==NULL)
    {
        printf("Error.");
        return;
    }
    memcpy(aux,a,tam);
    memcpy(a,b,tam);
    memcpy(b,aux,tam);

    free(aux);
}
/**********************************************************
*                       COMPARACION                       *
***********************************************************/
int comparar_dni (const void* a, const void* b){
    t_reg_indice* ra = (t_reg_indice*)a;
    t_reg_indice* pb = (t_reg_indice*)b;
    return ra->dni - pb->dni;
}











