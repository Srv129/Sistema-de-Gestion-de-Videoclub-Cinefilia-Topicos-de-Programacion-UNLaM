#include "menu.h"

char menu(const char m[][TAM_MENU], const char *txt)
{
    printf("\n");
    char opcion;
    opcion = elegir_opcion(m, txt, "Elegir opcion: ");

    while(!strchr(m[0], opcion))
        opcion = elegir_opcion(m, txt, "Opcion erronea. Elegir nuevamente: ");

    return opcion;
}

char elegir_opcion(const char m[][TAM_MENU], const char *txt, const char *msj)
{
    char opcion;
    int i;

    system("CLS");
    printf("\n %s \n", txt);
    for ( i=1 ; i <= strlen(m[0]) ; i++)
        printf("\n %c - %s", m[0][i-1], m[i]);

    printf("\n\n%s", msj);
    fflush(stdin);
    scanf(" %c", &opcion);

    return toupper(opcion);
}
/**********************************************************
*                FUNCIONES ABM
***********************************************************/
void generar_Alta (long dniABM, t_indice* indice, const char* path, t_Fecha* fecha_proceso, int(*cmp)(const void*, const void*)){
    FILE* ab = fopen(path,"r+b");
    if(!ab){
        printf("\nERROR AL ABRIR EL ARCHIVO PARA GENERAR EL ALTA.");
        return;
    }
    t_Miembro miembro;
    miembro.DNI=dniABM;
    if(pedir_datos_miembro(&miembro,fecha_proceso)!=TRUE){
        printf("\nError al ingresar los datos.");
        fclose(ab);
        return;
    }
    /// Guardar en archivo al final
    fseek(ab,0L,SEEK_END);
    long nro_reg=ftell(ab)/sizeof(t_Miembro);
    fwrite(&miembro,sizeof(t_Miembro),1,ab);

    t_reg_indice nuevo_reg = {nro_reg,miembro.DNI};
    if(indice_insertar(indice,&nuevo_reg,sizeof(t_reg_indice),cmp)!=OK){
        printf("\nError al insertar en el indice.");
        fclose(ab);
        return;
    }
    fclose(ab);
    printf("Usuario %ld dado de alta con exito.\n", dniABM);
}
int pedir_datos_miembro (t_Miembro* miembro, t_Fecha* fechaproceso){
    int c;
    char buffer[100];

    // --- APELLIDO Y NOMBRE ---
    printf("Ingrese Apellido y Nombre: ");
    while ((c = getchar()) != '\n' && c != EOF);
    fgets(miembro->ApellidosNombres, sizeof(miembro->ApellidosNombres), stdin);
    miembro->ApellidosNombres[strcspn(miembro->ApellidosNombres, "\n")] = '\0';
    if(!tiene_nombre_y_apellido(miembro->ApellidosNombres)){
        printf("Error: No se ingresaron un nombre y apellido validos.\n");
        return FALSE;
    }
    normalizar_apellido_nombre(miembro);

    // --- FECHA NACIMIENTO ---
    printf("Ingrese Fecha de Nacimiento (DD-MM-AAAA): ");
    ingresarFecha(&miembro->FechaNacimiento);
    if (!Validar_Nacimiento(&miembro->FechaNacimiento, fechaproceso)) {
        printf("Error: Fecha de nacimiento invalida.\n");
        return FALSE;
    }
    fflush(stdin);

    // --- SEXO ---
    printf("Ingrese Sexo (M/F): ");
    scanf(" %c", &miembro->Sexo);
    miembro->Sexo = toupper(miembro->Sexo);
    if (!ValidarSexo(miembro->Sexo)) {
        printf("Error: Sexo invalido.\n");
        return FALSE;
    }

    // --- FECHA DE AFILIACION ---
    printf("Ingrese Fecha de Afiliacion (DD-MM-AAAA): ");
    ingresarFecha(&miembro->FechaAfiliacion);
    if (!Validar_Afiliacion(&miembro->FechaAfiliacion, &miembro->FechaNacimiento, fechaproceso)) {
        printf("Error: Fecha de afiliación invalida.\n");
        return FALSE;
    }

    // --- CATEGORIA ---
    if(es_menor_de_edad(&miembro->FechaNacimiento, fechaproceso)){
        strcpy(miembro->Categoria, "MENOR");
    }
    else{
        strcpy(miembro->Categoria, "ADULTO");
    }

    // --- FECHA DE ULTIMA CUOTA ---
    printf("Ingrese Fecha de ultima cuota (DD-MM-AAAA): ");
    ingresarFecha(&miembro->FechaUltimaCuotaPaga);
    if (!Validar_UltimaCuota(&miembro->FechaUltimaCuotaPaga, &miembro->FechaAfiliacion, fechaproceso)) {
        printf("Error: Fecha deultima cuota invalida.\n");
        return FALSE;
    }

    miembro->Estado='A';

    // --- PLAN ---
    while ((c = getchar()) != '\n' && c != EOF);
    printf("Ingrese el Plan (BASIC / PREMIUM / VIP / FAMILY): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(miembro->Plan, buffer);
    str_toupper(miembro->Plan);
    if (!validar_Plan(miembro->Plan)) {
        printf("Error: Plan invalido.\n");
        return FALSE;
    }

    // --- MAIL DEL TUTOR (si corresponde) ---
    if (es_menor_de_edad(&miembro->FechaNacimiento, fechaproceso)) {
        printf("El miembro es menor de edad. Ingrese Email del Tutor: ");
        fgets(miembro->EmailTutor, sizeof(miembro->EmailTutor), stdin);
        miembro->EmailTutor[strcspn(miembro->EmailTutor, "\n")] = '\0';

        if (!Validar_MailTutor(miembro->EmailTutor, "MENOR")) {
            printf("Error: Email del tutor invalido.\n");
            return FALSE;
        }
    } else {
        strcpy(miembro->EmailTutor, "");
    }
    printf("\nDatos del miembro cargados correctamente.\n");
    return TRUE;
}
void ingresarFecha(t_Fecha *fProceso){
    do{
        fflush(stdin);
        int cantidad = scanf("%d-%d-%d", &fProceso->dia, &fProceso->mes, &fProceso->anio);
        if(cantidad != 3){
            printf("Formato invalido. Ingrese nuevamente:  ");
            while(getchar() != '\n');
            continue;
        }
    }while(!validar_Fecha(fProceso));
}
int es_menor_de_edad(const t_Fecha* nacimiento, const t_Fecha* fecha_actual) {
    t_Fecha edad=difFechas(nacimiento, fecha_actual);
    return edad.anio<18;
}
int validar_miembro_alta(t_Miembro* miembro,const t_Fecha* fecha_proceso){
    if (ValidarDni(miembro->DNI)==FALSE){
        printf("\nERROR: DNI invalido.");
        return FALSE;
    }
    if (validarStringAndTwo(miembro->ApellidosNombres,60)==0) {
        printf("\nERROR: Apellido y nombre invalidos.");
        return FALSE;
    }
    t_Fecha aux;
    aux = difFechas(&miembro->FechaNacimiento, fecha_proceso);
    if (validar_Fecha(&miembro->FechaNacimiento)==FALSE||aux.anio<10) {
        printf("\nERROR: Fecha de nacimiento invalida.");
        return FALSE;
    }
    if (ValidarSexo(miembro->Sexo)==FALSE) {
        printf("\nERROR: Sexo invalido. Debe ser M o F.");
        return FALSE;
    }

    if (validar_Fecha(&miembro->FechaAfiliacion)==FALSE){
        printf("\nERROR: Fecha de afiliacion invalida.");
        return FALSE;
    }
    if(!(cmpFecha(miembro->FechaAfiliacion,*fecha_proceso)<=0&&cmpFecha(miembro->FechaAfiliacion, miembro->FechaNacimiento)>0)){
    printf("\nError: La fecha de afiliacion debe ser posterior a la fecha de nacimiento y no mayor a la fecha de proceso.");
    return FALSE;
    }

    if (Validar_Categoria(miembro->Categoria,&miembro->FechaNacimiento,fecha_proceso)==FALSE) {
        printf("\nERROR: Categoria invalida.");
        return FALSE;
    }
    if(validar_Fecha(&miembro->FechaUltimaCuotaPaga)==FALSE){
        printf("\nError: Fecha de ultima cuota invalida.");
        return FALSE;
    }
    if(cmpFecha(miembro->FechaUltimaCuotaPaga, miembro->FechaAfiliacion)<=0||cmpFecha(miembro->FechaUltimaCuotaPaga, *fecha_proceso)>0){
        printf("\nError: La fecha de ultima cuota debe ser posterior a la de afiliacion y no mayor a la fecha de proceso.");
        return FALSE;
    }
    if (validar_Plan(miembro->Plan)==FALSE) {
        printf("\nERROR: Plan invalido.");
        return FALSE;
    }
    if (es_menor_de_edad(&miembro->FechaNacimiento, fecha_proceso)){
        if (Validar_MailTutor(miembro->EmailTutor,miembro->Categoria)==FALSE){
            printf("\nERROR: Email del tutor invalido.");
            return FALSE;
        }
    }
    return ValidarEstado(miembro->Estado);;
}

int mostrarArchivoBinario(const char* nombreArchivoBin) {
    FILE* pBin = fopen(nombreArchivoBin, "rb");

    if(!pBin) {
        return ERR_ARCHIVO;
    }

    t_Miembro miembro;
    fread(&miembro, sizeof(t_Miembro), 1, pBin);

    printf("\n ******** MIEMBROS *********** \n");
    while(!feof(pBin)) {
        printf("%ld|%s|%d-%d-%d|%c|%d-%d-%d|%s|%d-%d-%d|%c|%s|%s\n", miembro.DNI, miembro.ApellidosNombres, miembro.FechaNacimiento.dia, miembro.FechaNacimiento.mes, miembro.FechaNacimiento.anio, miembro.Sexo, miembro.FechaAfiliacion.dia, miembro.FechaAfiliacion.mes, miembro.FechaAfiliacion.anio, miembro.Categoria, miembro.FechaUltimaCuotaPaga.dia, miembro.FechaUltimaCuotaPaga.mes, miembro.FechaUltimaCuotaPaga.anio, miembro.Estado, miembro.Plan, miembro.EmailTutor);
        fread(&miembro, sizeof(t_Miembro), 1, pBin);
    }

    fclose(pBin);

    return TODO_OK;
}
/// --------------------------------------------
/// Checkea la existencia del archivo
int existeArchivo(const char* path){
    FILE* f = fopen(path, "rb");
    if(!f) return 0;
    fclose(f);
    return 1;
}

void ingresarOpcionMenu(int* cod) {
    do {
       printf("Ingrese la opcion que desea: ");
       scanf("%d", cod);
    } while(*cod != 1 && *cod != 2);
}

// ====================================================================
//                 FUNCIONES DE ARCHIVO
// ====================================================================

//ABRE EL ARCHIVO EN EL MODO QUE SE LE MANDE
FILE *abrir_archivo(const char *nombre_archivo, const char *modo)
{
    FILE *fp = fopen(nombre_archivo, modo);
    if (!fp)
    {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo: %s en modo %s\n", nombre_archivo, modo);
    }
    return fp;
}


int leer_registro_archivo(FILE *fp, unsigned nro_reg, t_Miembro *reg)
{
    long offset = nro_reg * sizeof(t_Miembro);

    if (fseek(fp, offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: Fallo fseek en leer_registro_archivo para %u\n", nro_reg);
        return ERROR;
    }
    if (fread(reg, sizeof(t_Miembro), 1, fp) != 1)
    {
        fprintf(stderr, "ERROR: Fallo fread en leer_registro_archivo para %u\n", nro_reg);
        return ERROR;
    }
    return OK;
}

int escribir_registro_archivo(FILE *fp, unsigned nro_reg, const t_Miembro *reg)
{
    long offset = nro_reg * sizeof(t_Miembro);

    if (fseek(fp, offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: Fallo fseek en escribir_registro_archivo para reg %u\n", nro_reg);
        return ERROR;
    }
    if (fwrite(reg, sizeof(t_Miembro), 1, fp) != 1)
    {
        fprintf(stderr, "ERROR: Fallo fwrite en escribir_registro_archivo para reg %u\n", nro_reg);
        return ERROR;
    }

    fflush(fp);
    return OK;
}

int validar_miembro(const t_Miembro *miembro)
{
    if (miembro->DNI < 1000000)
        return ERROR;
    // asumo que la validación es exitosa si pasa un chequeo básico xd.
    return OK;
}

void miembro_mostrar_datos(const t_Miembro *miembro)
{
    printf("\n--- Informacion del Miembro ---\n");
    printf("DNI: %ld\n", miembro->DNI);
    printf("Nombre: %s\n", miembro->ApellidosNombres);
    printf("F. Nacimiento: %02d/%02d/%d\n", miembro->FechaNacimiento.dia, miembro->FechaNacimiento.mes, miembro->FechaNacimiento.anio);
    printf("Plan: %s\n", miembro->Plan);
    printf("Estado: %c (%s)\n", miembro->Estado, miembro->Estado == 'A' ? "Activo" : "Baja");
    printf("Email Tutor: %s\n", miembro->EmailTutor);
    printf("-------------------------------\n");
}

// ====================================================================
//                 FUNCIONES DEL MENU
// ====================================================================

int miembro_modificar(t_indice *indice, FILE *fp, t_Fecha* fechaproceso)
{
    long dni_a_modificar;
    t_reg_indice reg_busqueda, *reg_indice_encontrado;
    t_Miembro miembro_actual;
    int pos_indice;

    char buffer_temp[MAX_APENOM];
    size_t len;
    char opcion;
    int modificado = 0;

    printf("\n--- MODIFICACION DE MIEMBRO ---\n");
    printf("Ingrese DNI del miembro a modificar: ");

    if (scanf("%ld", &dni_a_modificar) != 1) {
        printf("Error: Entrada invalida.\n");
        while (getchar() != '\n');
        return ERROR;
    }
    while (getchar() != '\n');

    reg_busqueda.dni = dni_a_modificar;
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni);

    if (pos_indice == NO_EXISTE) {
        printf("ERROR: El DNI %ld NO existe en el indice.\n", dni_a_modificar);
        return ERROR;
    }

    reg_indice_encontrado = (t_reg_indice*)((char*)indice->vindice + pos_indice * sizeof(t_reg_indice));
    unsigned nro_reg_archivo = reg_indice_encontrado->nro_reg;

    if (leer_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK) {
        printf("ERROR: No se pudo leer el registro %u del archivo.\n", nro_reg_archivo);
        return ERROR;
    }

    printf("\n--- DATOS ACTUALES del DNI %ld ---", dni_a_modificar);
    printf("\nNombre actual: %s", miembro_actual.ApellidosNombres);
    printf("\nEmail actual: %s", miembro_actual.EmailTutor);
    printf("\nFecha Ultima Cuota actual: %02d/%02d/%d", miembro_actual.FechaUltimaCuotaPaga.dia, miembro_actual.FechaUltimaCuotaPaga.mes, miembro_actual.FechaUltimaCuotaPaga.anio);
    printf("\nPlan actual: %s\n", miembro_actual.Plan);

    // ==========================================================
    // 1. MODIFICAR NOMBRE Y APELLIDO
    // ==========================================================
  printf("\n¿Desea modificar el nombre y apellido? (S/N): ");
opcion = getchar();
while (getchar() != '\n');

if (opcion == 'S' || opcion == 's') {
    do {
        printf("Ingrese NUEVO Nombre y Apellido (Max %d chars): ", MAX_APENOM - 1);
        fgets(buffer_temp, MAX_APENOM, stdin);

        len = strlen(buffer_temp);
        if (len > 0 && buffer_temp[len - 1] == '\n')
            buffer_temp[len - 1] = '\0';
        else if (len == MAX_APENOM - 1)
            while (getchar() != '\n');

        if (strlen(buffer_temp) == 0) {
            printf("ADVERTENCIA: El nombre no puede estar vacío.\n");
            continue;
        }

        if (strlen(buffer_temp) > 60) {
            printf("El nombre no puede tener más de 60 caracteres.\n");
            continue;
        }

        strcpy(miembro_actual.ApellidosNombres, buffer_temp);
        normalizar_apellido_nombre(&miembro_actual);
        modificado = 1;
        printf("Nombre normalizado: %s\n", miembro_actual.ApellidosNombres);
        break;
    } while (1);
}

    // ==========================================================
    // 2. MODIFICAR EMAIL TUTOR (Solo si es menor)
    // ==========================================================
    if (strcmp(miembro_actual.Categoria, "MENOR") == 0) {
        printf("\n¿Desea modificar el Email Tutor? (S/N): ");
        opcion = getchar();
        while (getchar() != '\n');

        if (opcion == 'S' || opcion == 's') {
            do {
                printf("Ingrese NUEVO Email Tutor (Max %d chars): ", MAX_EMAIL - 1);
                fgets(buffer_temp, MAX_EMAIL, stdin);

                len = strlen(buffer_temp);
                if (len > 0 && buffer_temp[len - 1] == '\n')
                    buffer_temp[len - 1] = '\0';
                else if (len == MAX_EMAIL - 1)
                    while (getchar() != '\n');

                if (strlen(buffer_temp) == 0) {
                    printf("El email no puede estar vacío. Intente nuevamente.\n");
                    continue;
                }

                if (!validar_Mail(buffer_temp)) {
                    printf("Formato de email inválido. Intente nuevamente.\n");
                    continue;
                }

                strcpy(miembro_actual.EmailTutor, buffer_temp);
                modificado = 1;
                break;
            } while (1);
        }
    } else {
        printf("\nEl miembro es MAYOR (%s). No se modifica Email Tutor.\n", miembro_actual.Categoria);
    }

    // ==========================================================
    // 3. MODIFICAR FECHA ULTIMA CUOTA PAGA
    // ==========================================================
    printf("\n¿Desea modificar la Fecha Ultima Cuota Paga? (S/N): ");
    opcion = getchar();
    while (getchar() != '\n');

    if (opcion == 'S' || opcion == 's') {
        t_Fecha nuevaFecha;

        do {
             printf("Ingrese NUEVA Fecha (DD/MM/AAAA): ");

            if (scanf("%d/%d/%d", &nuevaFecha.dia, &nuevaFecha.mes, &nuevaFecha.anio) != 3) {
                printf("Formato invalido. Intente nuevamente.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');

            if (!validar_Fecha(&nuevaFecha)) {
                printf("Fecha inválida. Intente nuevamente.\n");
                continue;
            }
            else if (cmpFecha(nuevaFecha, miembro_actual.FechaNacimiento) <= 0) {
                printf("La fecha debe ser posterior a la fecha de nacimiento.\n");
                continue;
            }
            else if (cmpFecha(nuevaFecha, miembro_actual.FechaUltimaCuotaPaga) <= 0) {
                printf("La nueva fecha debe ser posterior a la última cuota anterior.\n");
                continue;
            }
            else if (cmpFecha(nuevaFecha, *fechaproceso) > 0) {
                printf("La fecha no puede ser posterior a la fecha de proceso.\n");
                continue;
            }

            miembro_actual.FechaUltimaCuotaPaga = nuevaFecha;
            modificado = 1;
            printf("Fecha actualizada correctamente.\n");
            break;
        } while (1);
    }

    // ==========================================================
    // 4. MODIFICAR PLAN
    // ==========================================================
    printf("\n¿Desea modificar el Plan? (S/N): ");
    opcion = getchar();
    while (getchar() != '\n');

    if (opcion == 'S' || opcion == 's') {
        do {
            printf("Ingrese NUEVO Plan (BASIC, VIP, PREMIUM, FAMILY): ");
            fgets(buffer_temp, MAX_PLAN, stdin);

            len = strlen(buffer_temp);
            if (len > 0 && buffer_temp[len - 1] == '\n')
                buffer_temp[len - 1] = '\0';
            else if (len == MAX_PLAN - 1)
                while (getchar() != '\n');

            if (!validar_Plan(buffer_temp)) {
                printf("ERROR: Plan inválido. Intente nuevamente.\n");
                continue;
            }

            strcpy(miembro_actual.Plan, buffer_temp);
            modificado = 1;
            printf("Plan actualizado correctamente.\n");
            break;
        } while (1);
    }

    // ==========================================================
    // ESCRITURA FINAL
    // ==========================================================
    if (!modificado) {
        printf("No se realizó ninguna modificación.\n");
        return OK;
    }

    if (validar_miembro(&miembro_actual) != OK) {
        printf("ERROR: Los nuevos datos no son validos. Modificación cancelada.\n");
        return ERROR;
    }

    if (escribir_registro_archivo(fp, nro_reg_archivo, &miembro_actual) != OK) {
        printf("ERROR: No se pudo escribir el registro.\n");
        return ERROR;
    }

    printf("\nEXITO: El miembro %ld fue modificado correctamente.\n", dni_a_modificar);
    return OK;
}
void miembro_dar_baja(t_reg_indice* reg, t_indice *indice, const char* path, int(*cmp)(const void*, const void*)){
    FILE *ab = fopen(path,"r+b");
    if(!ab){
        printf("No se pudo abrir el archivo para generar la BAJA.");
        return;
    }

    /// Posicionar el puntero en el registro correspondiente
    fseek(ab,reg->nro_reg*sizeof(t_Miembro),SEEK_SET);
    t_Miembro miembro;
    fread(&miembro, sizeof(t_Miembro),1,ab);

    /// Cambiamos el estado a 'B'
    miembro.Estado = 'B';

    /// Volver a posicionarse para sobreescribir con el nuevo estado
    fseek(ab, reg->nro_reg*sizeof(t_Miembro),SEEK_SET);
    fwrite(&miembro, sizeof(t_Miembro),1,ab);
    fclose(ab);

    /// Eliminamos del índice
    if (indice_eliminar(indice,reg,sizeof(t_reg_indice),cmp)==ERROR){
        printf("\nNo se pudo eliminar del índice.");
    }
    else{
        indice->cantidad_elementos_actual--; /// bajamos el registro del indice
        printf("\nBaja del usuario %ld realizada con exito.",reg->dni);
    }
}
int miembro_mostrar_info(const t_indice *indice, FILE *fp)
{
    long dni_a_mostrar;
    t_reg_indice reg_busqueda, *reg_indice_encontrado;
    t_Miembro miembro_info;
    int pos_indice;

    printf("\n--- MOSTRAR INFORMACION DE MIEMBRO ---\n");
    printf("Ingrese DNI del miembro a mostrar: ");
    if (scanf("%ld", &dni_a_mostrar) != 1)
    {
        printf("ERRORR Entrada invalida.\n");
        while (getchar() != '\n');
        return ERROR;
    }
    while (getchar() != '\n');
    reg_busqueda.dni = dni_a_mostrar;
    pos_indice = indice_buscar(indice, &reg_busqueda, sizeof(t_reg_indice), comparar_dni);
    if (pos_indice == NO_EXISTE)
    {
        printf("ERROR: El DNI %ld NO existe en el indice. (Miembro Inactivo o no existe)\n", dni_a_mostrar);
        return ERROR;
    }
    reg_indice_encontrado = (t_reg_indice*)((char*)indice->vindice + pos_indice * sizeof(t_reg_indice));
    unsigned nro_reg_archivo = reg_indice_encontrado->nro_reg;
    if (leer_registro_archivo(fp, nro_reg_archivo, &miembro_info) != OK)
    {
        printf("ERROR: No se pudo leer el registro %u del archivo.\n", nro_reg_archivo);
        return ERROR;
    }
    miembro_mostrar_datos(&miembro_info);
    return OK;
}

// ====================================================================
//                 FUNCIONES DE LISTADOS
// ====================================================================
void mostrarMiembrosIndice(FILE *pf, const t_indice *indice, int soloActivos)
{
    t_Miembro miembro;
    t_reg_indice *reg;
    unsigned i;

    printf("%-8s | %-25s | %-4s | %-10s | %-8s | %-20s | %-7s\n",
           "DNI", "APELLIDO Y NOMBRE", "S", "PLAN", "CATEGORIA", "MAIL TUTOR", "ESTADO");
    printf("-----------------------------------------------------------------------------------------------\n");

    for (i = 0; i < indice->cantidad_elementos_actual; i++)
    {
        reg = (t_reg_indice *)((char *)indice->vindice + i * sizeof(t_reg_indice));

        if (leer_registro_archivo(pf, reg->nro_reg, &miembro) == OK)
        {
            if (soloActivos && miembro.Estado != 'A')
                continue;

            const char *estadoTexto = (miembro.Estado == 'A') ? "Activo" : "Baja";

            printf("%08ld | %-25s | %c | %-10s | %-8s | %-20s | %-7s\n",
                   miembro.DNI, miembro.ApellidosNombres,
                   miembro.Sexo, miembro.Plan,
                   miembro.Categoria, miembro.EmailTutor,
                   estadoTexto);
        }
    }
}


int ListadoOrdenadoDNI(const char *archivo, t_indice *indice)
{
    FILE *pf = abrir_archivo(archivo, "rb");
    if (!pf)
        return FALSE;

    printf("\n=== LISTADO DE MIEMBROS ORDENADOS POR DNI ===\n");
    mostrarMiembrosIndice(pf, indice, 1); //1: solo activos

    fclose(pf);
    return TRUE;
}

int ListadoAgrupadoPlan(const char *archivo, int (*cmp)(const void *, const void *))
{
    FILE *pf = abrir_archivo(archivo, "rb");
    if (!pf)
        return FALSE;

    t_Miembro miembro;
    unsigned nro_reg = 0;
    t_indice iBasic, iPremium, iVip, iFamily;
    t_reg_indice nuevo;

    //se crea un indice para cada plan
    indice_crear(&iBasic, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iPremium, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iVip, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&iFamily, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));

    while (fread(&miembro, sizeof(t_Miembro), 1, pf) == 1) //leo el archivo miembros
    {
        //if(miembro.Estado=='A'){
            nuevo.nro_reg = nro_reg;
            nuevo.dni = miembro.DNI;
            //se copian los datos a un indice nuevo, filtrando por tipo de plan
            if (strcmp(miembro.Plan, "BASIC") == 0)
                indice_insertar(&iBasic, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "PREMIUM") == 0)
                indice_insertar(&iPremium, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "VIP") == 0)
                indice_insertar(&iVip, &nuevo, sizeof(t_reg_indice), comparar_dni);
            else if (strcmp(miembro.Plan, "FAMILY") == 0)
                indice_insertar(&iFamily, &nuevo, sizeof(t_reg_indice), comparar_dni);
        //}
        nro_reg++;
    }

    //muestro listado usando indices creados
    printf("\n=== LISTADO DE MIEMBROS AGRUPADOS POR PLAN ===\n");
    rewind(pf);

    printf("\n--- PLAN BASIC ---\n");
    mostrarMiembrosIndice(pf, &iBasic, 0);

    printf("\n--- PLAN PREMIUM ---\n");
    mostrarMiembrosIndice(pf, &iPremium, 0);

    printf("\n--- PLAN VIP ---\n");
    mostrarMiembrosIndice(pf, &iVip, 0);

    printf("\n--- PLAN FAMILY ---\n");
    mostrarMiembrosIndice(pf, &iFamily, 0);

    //vacio cada indice creado previamente
    indice_vaciar(&iBasic);
    indice_vaciar(&iPremium);
    indice_vaciar(&iVip);
    indice_vaciar(&iFamily);

    fclose(pf);
    return TRUE;
}
