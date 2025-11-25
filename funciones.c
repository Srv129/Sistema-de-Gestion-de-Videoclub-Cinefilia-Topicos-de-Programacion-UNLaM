/// ARCHIVO BINARIO PARTE 1
#include "validacion.h"
#include "funciones.h"


int convertirTxtABin(const char* nombreArchTxt, const char* nombreArchBin, const char* nombreArchError, size_t tamReg, const t_Fecha* fechaProceso, TxtABin txtABin, EsErrorFatal esErrorFatal) {
    FILE* pTxt = fopen(nombreArchTxt, "rt");

    if(!pTxt) {
        return ERR_ARCHIVO;
    }

    FILE* pBin = fopen(nombreArchBin, "w+b");

    if(!pBin) {
        fclose(pTxt);
        return ERR_ARCHIVO;
    }

    void* reg = malloc(tamReg);

    char linea[TAM_LINEA + 1];
    int ret = TODO_OK;
    int retExisteReg = TODO_OK;
    while(!esErrorFatal(ret) && fgets(linea, TAM_LINEA, pTxt)) {
        ret = txtABin(linea, reg, fechaProceso);
        retExisteReg = buscarEnBin(pBin, reg);

        if(ret == TODO_OK && retExisteReg == TODO_OK) {
            fwrite(reg, tamReg, 1, pBin);
        } else {
                if(ret != TODO_OK && ret != ERR_LINEA_LARGA && ret != DUPLICADO) {
                    char mensaje[30];
                    strcpy(mensaje, obtenerMensajeError(ret));

                    guardarRegistroError(nombreArchError, reg, tamReg, mensaje);
                }
            }
    }

    fclose(pTxt);
    fclose(pBin);
    free(reg);

    return TODO_OK;
}

int buscarEnBin(FILE* pBin, const void* reg) {
    const t_Miembro* miembro = reg;
    t_Miembro miembroF;

    rewind(pBin);

    fread(&miembroF, sizeof(t_Miembro), 1, pBin);
    while(!feof(pBin)) {
        if(miembroF.DNI == miembro->DNI) return DUPLICADO;
        fread(&miembroF, sizeof(t_Miembro), 1, pBin);
    }

    return TODO_OK;
}

static int contar_barra(const char* s){
    int c = 0;
    for (; *s; s++) if (*s == '|') c++;
    return c;
}

static int cortar_ultimo_campo(char* linea, char** pCampo){
    char* sep = strrchr(linea, '|');
    if(!sep) return 0;
    *pCampo = sep + 1;
    *sep = '\0';
    return 1;
}

int txtABin(const char* linea_in, void* reg, const t_Fecha* fechaProceso) {
    t_Miembro* m = reg;

    char linea[TAM_LINEA + 1];
    strncpy(linea, linea_in, TAM_LINEA);
    linea[TAM_LINEA] = '\0';

    // Corto \n si está
    char* nl = strchr(linea, '\n');
    if(nl) *nl = '\0';

    // Contar barra para saber si hay email (MENOR) o no (ADULTO)
    int barra = contar_barra(linea);
    // MENOR: DNI|AyN|FN|S|FA|CAT|FUC|EST|PLAN|EMAIL  => 9 pipes
    // ADULTO: DNI|AyN|FN|S|FA|CAT|FUC|EST|PLAN       => 8 pipes
    if(barra < 8) return ERR_LINEA_LARGA; // línea mal formada

    char *campo = NULL;

    // (Opcional) Email tutor: solo si hay 9 pipes
    if(barra == 9) {
        if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
        strncpy(m->EmailTutor, campo, MAX_EMAIL-1);
        m->EmailTutor[MAX_EMAIL-1] = '\0';
    } else {
        // Adulto: no hay email tutor
        m->EmailTutor[0] = '\0';
    }

    // Plan
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    strncpy(m->Plan, campo, MAX_PLAN-1);
    m->Plan[MAX_PLAN-1] = '\0';

    // Estado (1 char)
    if(!cortar_ultimo_campo(linea, &campo) || campo[0] == '\0') return ERR_LINEA_LARGA;
    m->Estado = campo[0];

    // Fecha Ultima Cuota ('-' o '/')
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    if (strchr(campo, '/'))
        sscanf(campo, "%d/%d/%d", &m->FechaUltimaCuotaPaga.dia, &m->FechaUltimaCuotaPaga.mes, &m->FechaUltimaCuotaPaga.anio);
    else
        sscanf(campo, "%d-%d-%d", &m->FechaUltimaCuotaPaga.anio, &m->FechaUltimaCuotaPaga.mes, &m->FechaUltimaCuotaPaga.dia);

    // Categoria
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    strncpy(m->Categoria, campo, MAX_CAT-1);
    m->Categoria[MAX_CAT-1] = '\0';

    // Fecha Afiliacion
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    if (strchr(campo, '/'))
        sscanf(campo, "%d/%d/%d", &m->FechaAfiliacion.dia, &m->FechaAfiliacion.mes, &m->FechaAfiliacion.anio);
    else
        sscanf(campo, "%d-%d-%d", &m->FechaAfiliacion.anio, &m->FechaAfiliacion.mes, &m->FechaAfiliacion.dia);

    // Sexo
    if(!cortar_ultimo_campo(linea, &campo) || campo[0] == '\0') return ERR_LINEA_LARGA;
    m->Sexo = campo[0];

    // Fecha Nacimiento
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    if (strchr(campo, '/'))
        sscanf(campo, "%d/%d/%d", &m->FechaNacimiento.dia, &m->FechaNacimiento.mes, &m->FechaNacimiento.anio);
    else
        sscanf(campo, "%d-%d-%d", &m->FechaNacimiento.anio, &m->FechaNacimiento.mes, &m->FechaNacimiento.dia);

    // AyN
    if(!cortar_ultimo_campo(linea, &campo)) return ERR_LINEA_LARGA;
    if(campo[0] == '\0') return ERR_LINEA_LARGA;
    strncpy(m->ApellidosNombres, campo, MAX_APENOM-1);
    m->ApellidosNombres[MAX_APENOM-1] = '\0';
    normalizar_apellido_nombre(m);

    // DNI
    if(linea[0] == '\0') return ERR_LINEA_LARGA;
    if(sscanf(linea, "%ld", &m->DNI) != 1) return ERR_DNI;

    // Fecha de Proceso
    m->FechaProceso = *fechaProceso;

    // Validación final
    return validarMiembro(m);
}


bool esErrorFatal(int cod)
{
    switch(cod)
    {
        case ERR_LINEA_LARGA:
            return true;

        default:
            return false;
    }
}

char* obtenerMensajeError(int ret) {
   switch(ret) {
    case ERR_DNI:
        return "Dni invalido";

    case ERR_NAC:
        return "Fecha nacimiento invalido";

    case ERR_SEXO:
        return "Sexo invalido";

    case ERR_AFIL:
        return "Fecha afiliacion invalido";

    case ERR_CAT:
        return "Categoria invalido";

    case ERR_ULT_CUOTA:
        return "Fecha ultima cuota invalido";

    case ERR_ESTADO:
        return "Estado invalido";

    case ERR_PLAN:
        return "Plan invalido";

    case ERR_MAIL:
        return "Email invalido";

    case ERR_MAIL_TUTOR:
        return "Email tutor invalido";

    default:
        return "";
   }
}

int validarMiembro(t_Miembro* miembro) {
        if(ValidarDni(miembro->DNI) == FALSE) return ERR_DNI;

        if(Validar_Nacimiento(&miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_NAC;

        if(ValidarSexo(miembro->Sexo) == FALSE) return ERR_SEXO;

        if(Validar_Afiliacion(&miembro->FechaAfiliacion, &miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_AFIL;

        if(Validar_Categoria(miembro->Categoria, &miembro->FechaNacimiento, &miembro->FechaProceso) == FALSE) return ERR_CAT;

        if(Validar_UltimaCuota(&miembro->FechaUltimaCuotaPaga, &miembro->FechaAfiliacion, &miembro->FechaProceso) == FALSE) return ERR_ULT_CUOTA;

        if(ValidarEstado(miembro->Estado) == FALSE) return ERR_ESTADO;

        if(validar_Plan(miembro->Plan) == FALSE) return ERR_PLAN;

        if(strcmpi(miembro->Categoria, "MENOR") == 0) {
            if(validar_Mail(miembro->EmailTutor) == FALSE) return ERR_MAIL;

            if(Validar_MailTutor(miembro->EmailTutor, miembro->Categoria) == FALSE) return ERR_MAIL_TUTOR;
        }

    return TODO_OK;
}

int guardarRegistroError(const char* nombreArch, const void* reg, size_t tamElem, const char* mensaje) {
    FILE* pTxtError = fopen(nombreArch, "a+");

    if(!pTxtError) {
        return ERR_ARCHIVO;
    }

    const t_Miembro* miembro = reg;

    fprintf(pTxtError, "%ld|%s|%d-%d-%d|%c|%d-%d-%d|%s|%d-%d-%d|%c|%s|%s|%s\n", miembro->DNI, miembro->ApellidosNombres, miembro->FechaNacimiento.dia, miembro->FechaNacimiento.mes, miembro->FechaNacimiento.anio, miembro->Sexo, miembro->FechaAfiliacion.dia, miembro->FechaAfiliacion.mes, miembro->FechaAfiliacion.anio, miembro->Categoria, miembro->FechaUltimaCuotaPaga.dia, miembro->FechaUltimaCuotaPaga.mes, miembro->FechaUltimaCuotaPaga.anio, miembro->Estado, miembro->Plan, miembro->EmailTutor, mensaje);

    fclose(pTxtError);

    return TODO_OK;
}

/// Ingreso de fecha proceso
void ingresoFechaProceso(t_Fecha* fProceso) {
    do {
        printf("Ingrese la fecha de proceso (aaaa): ");
        scanf("%d",&fProceso->anio);

        printf("Ingrese la fecha de proceso (mm): ");
        scanf("%d",&fProceso->mes);

        printf("Ingrese la fecha de proceso (dd): ");
        scanf("%d",&fProceso->dia);

    } while(validar_Fecha(fProceso) == FALSE);
}

/// Concatenar fecha proceso con nombre de archivos
char* concatenarFechaConArchivo(char* nombreArch, const t_Fecha* fProceso, const char* extension) {
    char fechaFormatoTexto[20];

    sprintf(fechaFormatoTexto, "%04d%02d%02d", fProceso->anio, fProceso->mes, fProceso->dia);

    // Calcular el tamaño total necesario
    size_t tam = strlen(nombreArch) + strlen(fechaFormatoTexto) + strlen(extension) + 1;

    // Reservar memoria dinámica
    char* nombreArchivo = malloc(tam);
    if (!nombreArchivo) {
        return NULL;
    }

    strcpy(nombreArchivo, nombreArch);
    strcat(nombreArchivo, fechaFormatoTexto);
    strcat(nombreArchivo, extension);

    return nombreArchivo;
}
