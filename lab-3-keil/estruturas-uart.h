#include <stdint.h>

#define CR 0x0D
#define LF 0x0A

typedef enum TipoEntrada
{
	PASSOU_POR_ANDAR = 0,
	BOTAO_INTERNO,
	BOTAO_EXTERNO,
	PORTAS
} TipoEntrada;

typedef enum EstadoPortas
{
	ABERTAS = 0,
	FECHADAS
} EstadoPortas;

typedef enum DirecaoBotao
{
	SUBIDA = 0,
	DESCIDA
} DirecaoBotao;

typedef struct
{
	TipoEntrada tipo;
	char dados[2];
} EventoEntrada;

// ---------------------------

typedef enum TipoSaida
{
	MOVIMENTO = 0,
	LUZES
} TipoSaida;

typedef enum TipoMovimento
{
	INICIALIZA = 0,
	ABRE_PORTAS,
	FECHA_PORTAS,
	SOBE,
	DESCE,
	PARA
} TipoMovimento;

typedef struct
{
	TipoSaida tipo;
	char dados[2];
	char elevador;
} EventoSaida;