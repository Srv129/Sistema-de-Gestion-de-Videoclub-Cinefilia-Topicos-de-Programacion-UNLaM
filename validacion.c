#include "validacion.h"
/**********************************************************
*                VALIDACION   DATOS                       *
***********************************************************/
int validar_Plan (char plan[10]){
    char *opc[] = {"BASIC", "PREMIUM", "VIP", "FAMILY"};
    int i;
    str_toupper(plan);
    for(i=0;i<4;i++){
        if(strcmp(plan,opc[i])==0)
            return TRUE;
    }
    return FALSE;
}
int ValidarDni(long dni){
    return (dni>10000 && dni<100000000)?TRUE:FALSE;
}
int ValidarSexo(char s){
    s = toUpper(s);
    return (s=='F' || s=='M')? TRUE:FALSE;
}
int ValidarEstado(char e){
    e= toUpper(e);
    return (e=='A' || e=='B')? TRUE:FALSE;
}
int validar_Mail(const char* mail){
    if(mail == NULL || strlen(mail) == 0) return FALSE;
    if(strchr(mail,' ') || strchr(mail,'\t') || strchr(mail,'\n')) return FALSE;
    const char* arroba = strchr(mail,'@');
    if(!arroba || arroba == mail || *(arroba+1) == '\0') return FALSE;
    if(strchr(arroba+1,'@')) return FALSE;
    const char* punto = strrchr(arroba,'.');
    if(!punto || punto < arroba+2 || *(punto+1) == '\0') return FALSE;
    return TRUE;
}
int Validar_Categoria(const char *categ, const t_Fecha *fechaNac, const t_Fecha *fechaProc){
    t_Fecha edad = difFechas(fechaNac, fechaProc);

    if(edad.anio<18)
        return(strcmp(categ,"MENOR")==0)? TRUE:FALSE;
    else
        return(strcmp(categ,"ADULTO")==0)? TRUE:FALSE;
}
int Validar_MailTutor(const char* mail, const char* charcateg){

    if(strcmp(charcateg,"MENOR")==0){
        return validar_Mail(mail);
    }

    if(mail != NULL && strlen(mail) > 0){
        return validar_Mail(mail);
    }
    return TRUE;
}
int Validar_Afiliacion(const t_Fecha *fechaAfil, const t_Fecha *fechaNac, const t_Fecha *fechaProc){

    if(!validar_Fecha(fechaAfil))
        return FALSE;
    if(difFechas(fechaAfil,fechaProc).anio<0)
        return FALSE;
    if(difFechas(fechaAfil,fechaNac).anio>=0)
        return FALSE;

    return TRUE;
}

int Validar_UltimaCuota(const t_Fecha *fechaUltCuota, const t_Fecha *fechaAfil, const t_Fecha *fechaProc){

    if(difFechas(fechaUltCuota,fechaAfil).anio>=0)
        return FALSE;
    if(difFechas(fechaUltCuota,fechaProc).anio<0)
        return FALSE;

    return TRUE;
}

int Validar_Nacimiento(const t_Fecha *fechaNac, const t_Fecha *fechaProc){
    if(!validar_Fecha(fechaNac))
        return FALSE;
    if(difFechas(fechaNac,fechaProc).anio<10)
        return FALSE;

    return TRUE;
}

/**********************************************************
**                FUNCIONES STRING                        *
***********************************************************/
int tiene_nombre_y_apellido(const char* str) {
    int palabras = 0;
    int en_palabra = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isspace(str[i])) {
            if (!en_palabra) {
                palabras++;
                en_palabra = 1;
            }
        } else {
            en_palabra = 0;
        }
    }

    // Retorna TRUE si hay al menos dos palabras
    return (palabras >= 2);
}
int validarStringAndTwo(char *str, int tam){
    int i = 0;
    int found = 0;
    int first = 0;
    int second = 0;
    int isspace = 1;
    while(i < tam && found!= 1){
        if(second== 0 && ES_LETRA(str[i])){
            if(first ==0){
                isspace = 0;
                first = 1;
            }
            else{
                if(isspace == 1){
                    second =1;
                }
            }
        }
        else{
            if(str[i] == '\0'){
                found = 1;
            }
            else{
                isspace =1;
            }
        }
        i++;
    }
    return (second ==1 && found ==1)?TRUE:FALSE;
}
void str_toupper(char* str){
    while(*str){
        *str = toUpper(*str);
        str++;
    }
}

void normalizar_apellido_nombre(t_Miembro *miembro_ptr)
{
    if (!miembro_ptr || miembro_ptr->ApellidosNombres[0] == '\0')
        return;

    char buffer[MAX_APENOM];
    strncpy(buffer, miembro_ptr->ApellidosNombres, MAX_APENOM - 1);
    buffer[MAX_APENOM - 1] = '\0';

    // Convertir todo a minúsculas
    for (int i = 0; buffer[i]; i++)
        buffer[i] = tolower((unsigned char)buffer[i]);

    // Eliminar espacios iniciales/finales
    char *ini = buffer;
    while (isspace(*ini)) ini++;
    char *fin = ini + strlen(ini) - 1;
    while (fin > ini && isspace(*fin)) *fin-- = '\0';

    // Insertar coma si no hay
    if (!strchr(ini, ',')) {
        char *esp = strchr(ini, ' ');
        if (esp) {
            memmove(esp + 2, esp, strlen(esp) + 1);
            *esp = ',';
            *(esp + 1) = ' ';
        }
    }

    // Mayus a cada palabra después de coma o espacio
    int nueva_palabra = 1;
    for (char *p = ini; *p; p++) {
        if (isspace(*p) || *p == ',')
            nueva_palabra = 1;
        else if (nueva_palabra) {
            *p = toupper((unsigned char)*p);
            nueva_palabra = 0;
        }
    }

    strncpy(miembro_ptr->ApellidosNombres, ini, MAX_APENOM - 1);
    miembro_ptr->ApellidosNombres[MAX_APENOM - 1] = '\0';
}


/**********************************************************
*                VALIDACION   FECHA                       *
***********************************************************/
int anioBisiesto(int anio)
{
    return ((anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0)?TRUE:FALSE;
}
t_Fecha difFechas(const t_Fecha *fecIni, const t_Fecha *fecFin){
    static const char dias[2][13] = {
        { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};
    t_Fecha edad;
    edad = *fecFin;
    if((edad.dia -= fecIni->dia)<0){
        edad.mes--;
        edad.dia += dias[anioBisiesto(edad.anio)][edad.mes];
    }
    if((edad.mes -= fecIni->mes)<0){
        edad.mes +=12;
        edad.anio--;
    }
    edad.anio -= fecIni->anio;
    return edad;
}
int validar_Fecha(const t_Fecha *fecha){
    static const char dias[2][13] = {
        { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

    if(fecha->anio<1900 || fecha->anio>9999)
        return FALSE;
    if(fecha->mes<1 || fecha->mes>12)
        return FALSE;
    if(fecha->dia<1 || fecha->dia> dias[anioBisiesto(fecha->anio)][fecha->mes])
        return FALSE;

    return TRUE;
}
int cmpFecha(const t_Fecha f1, const t_Fecha f2){
    if(f1.anio != f2.anio){
        return (f1.anio>f2.anio)?1:-1;
    }
    if(f1.mes != f2.mes){
        return (f1.mes>f2.mes)?1:-1;
    }
    if(f1.dia != f2.dia){
        return (f1.dia > f2.dia)?1:-1;
    }
    return 0;
}
