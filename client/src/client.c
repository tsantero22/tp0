#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */
	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if (config == NULL){
		// No se creó el config correctamente, abortar
		abort();
	}
	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	bool key_exists = config_has_property(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");


	// Loggeamos el valor de config
	
	if (key_exists){
		valor = config_get_string_value(config, "CLAVE");
		log_info(logger, "La CLAVE de config existe y es: %s", valor);
	} else {
    	log_warning(logger, "La CLAVE no existe en el archivo de configuración.");
	}

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Hago HANDSHAKE
	size_t bytes;

	int32_t handshake = 1;
	int32_t result;

	bytes = send(conexion, &handshake, sizeof(int32_t), 0);
	bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if (result == 0) {
		log_info(logger, "HANDSHAKE OK");
	} else {
		log_error(logger, "HANDSHAKE ERROR");
	}

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);
	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create("client.log", "Client", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

    while (1)
    {
        leido = readline("> ");

        // Sale del bucle con línea vacía
        if (strcmp(leido, "") == 0)
        {
            free(leido);
            break;
        }

        // Se lee lo escrito
        log_info(logger, leido);

        // Libero memoria
        free(leido);
    }
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
    while (1) // Copio el funcionamiento de leer_consola y lo adapto
    {
        leido = readline("> ");

        // Sale del bucle con línea vacía
        if (strcmp(leido, "") == 0)
        {
            free(leido);
            break;
        }

        // Se agrega la línea al paquete
        agregar_a_paquete(paquete, leido, strlen(leido) + 1);

        // Libero memoria
        free(leido);
    }

	enviar_paquete(paquete, conexion);
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_warning(logger, "TERMINANDO EL PROGRAMA");
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
