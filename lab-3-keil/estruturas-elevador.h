#include <stdint.h>
#include <stdbool.h>

enum EstadoPortas;

typedef enum EstadoElevador
{
	INICIALIZANDO = 0,
	PRONTO,
	FECHANDO_PORTAS,
	ABRINDO_PORTAS,
	DESCENDO,
	SUBINDO,
	AGUARDANDO
} EstadoElevador;

typedef struct StructElevador
{
	EstadoElevador estadoAtual;
  EstadoElevador estadoAnterior;
  uint8_t andarAtual;
  uint8_t andarAlvo;
	uint16_t pendentesSubida;
	uint16_t pendentesDescida;
  uint16_t pendentesInterno;
	bool mensagemEnviada;
} StructElevador;
