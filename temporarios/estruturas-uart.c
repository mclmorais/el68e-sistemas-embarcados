typedef struct
{
	TipoEntrada tipo;
	char dados[2];
} EventoEntrada;

typedef enum
{
	PASSOU_POR_ANDAR = 0,
	BOTAO_INTERNO,
	BOTAO_EXTERNO,
	PORTAS
} TipoEntrada;

typedef enum
{
	ABERTAS = 0,
	FECHADAS
} EstadoPortas;

typedef enum
{
	SUBIDA = 0,
	DESCIDA
} DirecaoBotao;

// ---------------------------

typedef struct
{
	TipoSaida tipo;
	char dados[2];
	char elevador;
} EventoSaida;

typedef enum
{
	MOVIMENTO = 0,
	LUZES
} TipoSaida;

typedef enum
{
	INICIALIZA = 0,
	ABRE_PORTAS,
	FECHA_PORTAS,
	SOBE,
	DESCE,
	PARA
} TipoMovimento;