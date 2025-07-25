#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Num_Instrucoes 256 // Limite
#define Tam_Instrucao 17 // 16 bits + 1 para o \0
#define NUM_REGISTRADORES 8 // Numero de Registradores, 3 bits = 8
#define Tam_Memoria 256

typedef enum {
    TipoR,
    TipoI,
    TipoJ,
    Outro
} TipoInstrucao;

typedef enum{
    BI,
    DI,
    EX,
    MEM,
    ER
} Etapa;

typedef struct {
    char binario[17];
    int op, rs, rt, rd, imm, funct, end;
    char sop[5], srs[4], srt[4], srd[4], simm[7], sfunct[4], send[13]; //tirar
    TipoInstrucao tipo;
    //Etapa etapa;
} Memoria;

typedef struct{
    int UlaZero;
    int SinalOverflow;
    int UlaSaida;
}SinaisULA;

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
    SinaisControle sinaiscontrole;
    int dado_lido1;
    int dado_lido2;
    int pc_mais_um;
    //int imm;
} DI_EX;

typedef struct {
    int pc;
    int registradores[NUM_REGISTRADORES];
    int Memoria_Dados[Tam_Memoria];
    SinaisULA ula;
    SinaisControle sinaiscontrole;
    BI_DI bi_di;
    DI_EX di_ex;
} Pipeline;

//typedef struct {
//    int topo;
//    Monociclo Estados[Tam_Memoria];
//} Pilha_de_Estados;

int converteOpCode(char *line) {
    int decimal = 0;
     for (int i = 0; i < 4; i++) {
        //Vefirica se é 1 e soma o valor em potência de dois
        if (line[i] == '1') {
            decimal += pow(2, 4 - 1 - i);
        } else if (line[i] != '0') {
             printf("Erro: Entrada inválida. Use apenas '0' e '1'.\n");
            return -1;
        }
    }
    return decimal;
}

//Quase a mesma função que a de cima, mas essa tem o tamanho
int binParaDec(char *line) {   
    int decimal = 0;
    int tamanho = strlen(line);
     for (int i = 0; i < tamanho; i++) {
        if (line[i] == '1') {
            decimal += pow(2, tamanho - 1 - i);
        } else if (line[i] != '0') {
             printf("Erro: Entrada inválida. Use apenas '0' e '1'.\n");
            return -1;
        }
    }
    return decimal;
}

//Função que separa a instrução em op/rs/rt/rd/funct
Memoria recebeInstrucaoR(char *line) {
    Memoria instrucao;
    
    instrucao.imm = -1;
    instrucao.end = -1;

    strcpy(instrucao.binario, line);
    
    strncpy(instrucao.sop, instrucao.binario, 4);
    instrucao.sop[4] = '\0';
    instrucao.op = binParaDec(instrucao.sop);

    strncpy(instrucao.srs, instrucao.binario + 4, 3);
    instrucao.srs[3] = '\0';
    instrucao.rs = binParaDec(instrucao.srs);

    strncpy(instrucao.srt, instrucao.binario + 7, 3);
    instrucao.srt[3] = '\0';
    instrucao.rt = binParaDec(instrucao.srt);

    strncpy(instrucao.srd, instrucao.binario + 10, 3);
    instrucao.srd[3] = '\0';
    instrucao.rd = binParaDec(instrucao.srd);
    
    strncpy(instrucao.sfunct, instrucao.binario + 13, 3);
    instrucao.sfunct[3] = '\0';
    instrucao.funct = binParaDec(instrucao.sfunct);

    instrucao.tipo = TipoR;
    return instrucao;
}

//Função que separa a instrução em op/rs/rt/imm
Memoria recebeInstrucaoI(char *line) {
    Memoria instrucao;
    
    instrucao.rd=-1;
    instrucao.funct=-1;
    instrucao.end=-1;

    strcpy(instrucao.binario, line);
    
    strncpy(instrucao.sop, instrucao.binario, 4);
    instrucao.sop[4] = '\0';
    instrucao.op = binParaDec(instrucao.sop);

    strncpy(instrucao.srs, instrucao.binario + 4, 3);
    instrucao.srs[3] = '\0';
    instrucao.rs = binParaDec(instrucao.srs);

    strncpy(instrucao.srt, instrucao.binario + 7, 3);
    instrucao.srt[3] = '\0';
    instrucao.rt = binParaDec(instrucao.srt);

    strncpy(instrucao.simm, instrucao.binario + 10, 6);
    instrucao.simm[6] = '\0';
    instrucao.imm = binParaDec(instrucao.simm);
    
    instrucao.tipo = TipoI;
    
     return instrucao;
}

//Função que separa a instrução em op/address
Memoria recebeInstrucaoJ(char *line) {
    Memoria instrucao;
    
    instrucao.rt =-1;
    instrucao.rs =-1;
    instrucao.rd =-1;
    instrucao.funct = -1;
    instrucao.imm =-1;
    
    strcpy(instrucao.binario, line);
    
    strncpy(instrucao.sop, instrucao.binario, 4);
    instrucao.sop[4] = '\0';
    instrucao.op = binParaDec(instrucao.sop);
    
    strncpy(instrucao.send, instrucao.binario + 4, 12);
    instrucao.send[12] = '\0';
    instrucao.end = binParaDec(instrucao.send);
    
    instrucao.tipo = TipoJ;
     return instrucao;
}

void Ler_Arquivo(Memoria *instrucoes, int *qtd_instrucoes) {
    char nome_arquivo[100];
    char line[Tam_Instrucao+1];
    int cont_local = 0;

    printf("Digite o nome do arquivo a ser carregado (.mem): ");
    scanf("%99s", nome_arquivo);

    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        //return;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char buffer[5];
        strncpy(buffer, line, 4);
        buffer[4] = '\0';

        int opcode = converteOpCode(buffer);

        if (opcode == 0) {
            instrucoes[cont_local] = recebeInstrucaoR(line);
        } else if (opcode == 2) {
            instrucoes[cont_local] = recebeInstrucaoJ(line);
        } else if (opcode == 4 || opcode == 8 || opcode == 11 || opcode == 15) {
            instrucoes[cont_local] = recebeInstrucaoI(line);
        } else {
            printf("Instrução inválida ignorada: %s\n", line);
            continue;
        }
        cont_local++;
        if (cont_local >= Num_Instrucoes){ 
            break;
        }
    }

    fclose(file);
    printf("Arquivo carregado com sucesso. %d instruções lidas.\n", cont_local);
    *qtd_instrucoes = cont_local;
}

SinaisULA ULA(int ulaop, int operando1, int operando2){
    SinaisULA resultado;
    resultado.UlaSaida = 0;
    resultado.UlaZero = 0;
    resultado.SinalOverflow = 0;
    switch (ulaop) {
        case 0: //ADD
            resultado.UlaSaida = operando1 + operando2;
            if(resultado.UlaSaida >127 || resultado.UlaSaida <-128){
                resultado.SinalOverflow = 1;
            }else{
                resultado.SinalOverflow = 0;
            }
            return resultado;
        break;
        case 2: //SUB
            resultado.UlaSaida = operando1 - operando2;
            if(resultado.UlaSaida >127 || resultado.UlaSaida<-128){
                resultado.SinalOverflow = 1;
            }else{
                resultado.SinalOverflow = 0;
            }
            if (resultado.UlaSaida == 0){
                resultado.UlaZero = 1;
            }else if (resultado.UlaSaida != 0){
                resultado.UlaZero = 0;
            }
            
            return resultado;
        break;
        case 4: //AND
            resultado.UlaSaida = operando1 & operando2;
            return resultado;
        break;
        case 5: //OR
            resultado.UlaSaida = operando1 | operando2;
            return resultado;
        break;
    }
}

void imprimir_registradores(Pipeline *pipeline) {
    printf("=====Banco de Registradores=====\n");
    for (int i = 0; i < NUM_REGISTRADORES; i++) {
        printf("R%d: %d\n", i, pipeline->registradores[i]);
    }
    printf("================================\n");
}

void imprimeInstrucao(Memoria *instrucao){
    printf("==========\n");
    printf("\nBinario da instrucao: %s\n", instrucao->binario);
     switch (instrucao->tipo) {
        case TipoR:
            printf("opcode: %d \n",  instrucao->op);
            printf("rs: %d \n",  instrucao->rs); 
            printf("rt: %d \n",  instrucao->rt);
            printf("rd: %d \n",  instrucao->rd);
        break;
        case TipoI:
            printf("opcode: %d \n",  instrucao->op);
            printf("rs: %d \n",  instrucao->rs); 
            printf("rt: %d \n",  instrucao->rt);
            printf("imediato : %d \n",  instrucao->imm);
        break;
        case TipoJ:
            printf("opcode: %d \n",  instrucao->op);
            printf("endereço: %d \n",  instrucao->end);
        break;
        default:
            printf("Outro\n");
        break;
    }
    //printf("==========\n");
}

void CarregarMemoriaDados(Pipeline *pipeline){
    char nome_arquivo[100];
    printf("Digite o nome do arquivo .dat para carregar: ");
    scanf("%99s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    fread(pipeline->Memoria_Dados,sizeof(int), 256, arquivo);
    fclose(arquivo);
    printf("Memória carregada de %s !\n", nome_arquivo);
}

void SalvarMemoriaDados(int memoria[]) {
    FILE *arquivo = fopen("memoria.dat", "wb");

    if (!arquivo) {
        perror("Erro ao criar memoria.dat");
        return;
    }

   fwrite(memoria, sizeof(int), 256, arquivo);
   
    fclose(arquivo);
    printf("Memória salva com sucesso em memoria.dat!\n");
}

void SalvarASM(Memoria *instrucoes, int cont){
    
    FILE *arquivo = fopen("Arquivo_ASM.asm", "w");

    if (!arquivo) {
        perror("Erro ao criar arquivo .asm");
        return;
    }

    for (int i = 0; i < cont; i++) {
        //auxilia para não ter confusões de simbolos

        switch (instrucoes[i].tipo) {
            case TipoR:
                switch (instrucoes[i].funct) {
                    case 0: // ADD
                        if(instrucoes[i].rd == 0 && instrucoes[i].rs == 0 && instrucoes[i].rt == 0){
                            fprintf(arquivo, "NOP\n");
                            break;
                        }
                        fprintf(arquivo, "add $%d, $%d, $%d\n", instrucoes[i].rd, instrucoes[i].rs, instrucoes[i].rt);
                        break;
                    case 2: // SUB
                        fprintf(arquivo, "sub $%d, $%d, $%d\n", instrucoes[i].rd, instrucoes[i].rs, instrucoes[i].rt);
                        break;
                    case 4: // AND
                        fprintf(arquivo, "and $%d, $%d, $%d\n", instrucoes[i].rd, instrucoes[i].rs, instrucoes[i].rt);
                        break;
                    case 5: // OR
                        fprintf(arquivo, "or $%d, $%d, $%d\n", instrucoes[i].rd, instrucoes[i].rs, instrucoes[i].rt);
                        break;
                    default:
                        fprintf(arquivo, "funct invalido: %d\n", instrucoes[i].funct);
                        break;
                }
                break;

            case TipoI:
                switch (instrucoes[i].op) {
                    case 4: // ADDI
                        fprintf(arquivo, "addi $%d, $%d, %d\n", instrucoes[i].rt, instrucoes[i].rs, instrucoes[i].imm);
                        break;
                    case 8: // BEQ
                        fprintf(arquivo, "beq $%d, $%d, %d\n", instrucoes[i].rt, instrucoes[i].rs, instrucoes[i].imm);
                        break;
                    case 11: // LW
                        fprintf(arquivo, "lw $%d, %d($%d)\n", instrucoes[i].rt, instrucoes[i].imm, instrucoes[i].rs);
                        break;
                    case 15: // SW
                        fprintf(arquivo, "sw $%d, %d($%d)\n", instrucoes[i].rt, instrucoes[i].imm, instrucoes[i].rs);
                        break;
                    default:
                        fprintf(arquivo, "opcode invalido: %d\n", instrucoes[i].op);
                        break;
                }
                break;

            case TipoJ:
                switch (instrucoes[i].op) {
                    case 2: // JUMP
                        fprintf(arquivo, "j %d\n", instrucoes[i].end);
                        break;
                    default:
                        fprintf(arquivo, "opcode invalido: %d\n", instrucoes[i].op);
                        break;
                }
                break;
            default:
                fprintf(arquivo, "Instrucao invalida\n");
                break;
        }
    }

    fclose(arquivo);
    printf("Arquivo .asm salvo com sucesso!\n");
}

void Controle(Memoria *instrucao, Pipeline *pipeline){//Contrário?
    printf("==========\n");
    // Execução
    //pipeline->ex_mem.resultadoula=resultadoula 

    // Decodificação
    if (pipeline->bi_di.instrucao != NULL) {
        printf("Decodificando instrução: \t%s\n", pipeline->bi_di.instrucao->binario);
        pipeline->di_ex.pc_mais_um = pipeline->bi_di.pc_mais_um;
        pipeline->di_ex.dado_lido1 = pipeline->bi_di.instrucao->rs; //dentro do switch
        pipeline->di_ex.dado_lido1 = pipeline->bi_di.instrucao->rt; //dentro do switch
        switch(pipeline->bi_di.instrucao->op){
            case 0: //0000
            break;
        }
    } else {
        printf("Decodificando instrução: \tNenhuma\n");
    }

    // Busca
    if (instrucao != NULL) {
        pipeline->bi_di.instrucao = instrucao;
        pipeline->bi_di.pc_mais_um = pipeline->pc + 1;
        pipeline->pc = pipeline->pc + 1;
        printf("Buscando instrução: \t\t%s\n", instrucao->binario);
    }
}


int main() {
    int cont = 0;
    Memoria* instrucoes = malloc(sizeof(Memoria) * Num_Instrucoes);
    Pipeline *pipeline = malloc(sizeof(Pipeline));
    pipeline->pc = 0;
    pipeline->ula.UlaSaida = 0;
    pipeline->ula.UlaZero = 0;

    for(int i=0; i<NUM_REGISTRADORES;i++){
        pipeline->registradores[i] = 0;
    }
    for(int i=0; i<Tam_Memoria;i++){
        pipeline->Memoria_Dados[i] = 0;
    }
    for(int i=0; i<Num_Instrucoes; i++){
        strcpy(instrucoes[i].binario, "0000000000000000");
        instrucoes[i].tipo = Outro;
    }
    int op_menuprincipal=1;

    Ler_Arquivo(instrucoes,&cont);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    Controle(&instrucoes[pipeline->pc], pipeline);
    return 0;
}