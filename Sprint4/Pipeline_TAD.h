#define Num_Instrucoes 256 // Limite
#define Tam_Instrucao 17 // 16 bits + 1 para o \0
#define NUM_REGISTRADORES 8 // Numero de Registradores, 3 bits = 8
#define Tam_Memoria 256
#define MENU_ALTURA 13 
#define MENU_LARGURA 45
#define MSG_WIN_ALTURA 6 

typedef enum {
    TipoR,
    TipoI,
    TipoJ,
    NOP
} TipoInstrucao;

typedef struct {
    char binario[17];
    int op, rs, rt, rd, imm, funct, end;
    TipoInstrucao tipo;
} Memoria;

typedef struct{
    int UlaZero;
    int SinalOverflow;
    int UlaSaida;
}SaidasULA;

typedef struct{
    int EscReg;
    int UlaOP;
    int UlaFonte;
    int RegDst;
    int DVI;
    int DVC;
    int EscMem;
    int MemParaReg;
} SinaisControle;

typedef struct{
    Memoria *instrucao;
    int pc_mais_um;
} BI_DI;

typedef struct{
    Memoria *instrucao;
    SinaisControle sinaiscontrole;
    int dado_lido1;
    int dado_lido2;
    int pc_mais_um;
    int imm;
} DI_EX;

typedef struct{
    Memoria *instrucao;
    int pc_beq;
    SinaisControle sinaiscontrole;
    SaidasULA ula;
    int dado_a_ser_escrito;
} EX_MEM;

typedef struct{
    Memoria *instrucao;
    SinaisControle sinaiscontrole;
    SaidasULA ula;
    int dado_lido;
} MEM_ER;

typedef struct {
    int pc;
    int registradores[NUM_REGISTRADORES];
    int Memoria_Dados[Tam_Memoria];
    SaidasULA ula;
    SinaisControle sinaiscontrole;
    BI_DI bi_di;
    DI_EX di_ex;
    EX_MEM ex_mem;
    MEM_ER mem_er;
} Pipeline;

typedef struct No {
  Pipeline* pipeline;
  struct No *proximo;
} No;

int BinParaDec(char binario[]);
void Decodicador(Memoria *memoria);
void Ler_Arquivo(Memoria *instrucoes, int *qtd_instrucoes,char *nome_arquivo);
SaidasULA ULA(int ulaop, int operando1, int operando2);
void CarregarMemoriaDados(Pipeline *pipeline, char *nome_arquivo);
void SalvarASM(Memoria *instrucoes, int cont,char *nome_arquivo);
void SalvarMemoriaDados(int memoria[],char *nome_arquivo);
void Controle(Memoria *instrucao, Pipeline *pipeline);
void empilhar(No **topo, Pipeline* pipeline_atual);
void desempilhar(No **topo, Pipeline* pipeline);
void draw_button(WINDOW *win, int y, int x, const char *label, bool is_selected);
void Painel_Estado(WINDOW *win, Pipeline *pipeline);
void Painel_Pipeline(WINDOW *win, Pipeline *pipeline,Memoria *instrucao);
void Tela_Memoria_Instrucoes(WINDOW *win, Memoria *instrucoes);
void Tela_Memoria_Dados(WINDOW *win, Pipeline *pipeline);
char* identificainstrucao(Memoria *instrucao);