#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include "Pipeline_TAD.h"

int BinParaDec(char binario[]){
    int resultado;
    int tam_bin = strlen(binario);
    if(binario[0] == '1'){
        resultado = strtol(binario, NULL, 2) - pow(2,tam_bin);
    }else{
        resultado = strtol(binario, NULL, 2);
    }
    return resultado;
}

void Decodicador(Memoria *memoria){
    char auxiliar[16];
    strncpy(auxiliar, memoria->binario, 4);
    auxiliar[4] = '\0';
    memoria->op = strtol(auxiliar, NULL, 2);
    switch(memoria->op){
    case 0: //Tipo R
      memoria->tipo = TipoR;
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // RS
      strncpy(auxiliar, memoria->binario + 4, 3);
      auxiliar[3] = '\0';
      memoria->rs = strtol(auxiliar, NULL, 2);
      // RT
      strncpy(auxiliar, memoria->binario + 7, 3);
      auxiliar[3] = '\0';
      memoria->rt = strtol(auxiliar, NULL, 2);
      // RD
      strncpy(auxiliar, memoria->binario + 10, 3);
      auxiliar[3] = '\0';
      memoria->rd = strtol(auxiliar, NULL, 2);
      //Funct
      strncpy(auxiliar, memoria->binario + 13, 3);
      auxiliar[3] = '\0';
      memoria->funct = strtol(auxiliar, NULL, 2);
      //outros campos 0
      memoria->imm = 0;
      memoria->end = 0;
      if(memoria->rd == 0 &&  memoria->rs == 0 && memoria->rt == 0){
                    memoria->tipo =NOP;
                break;
        }
      break;
    case 2: //Tipo J
      memoria->tipo = TipoJ;
      // OP
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // END
      strncpy(auxiliar, memoria->binario + 4, 12);
      auxiliar[12] = '\0';
      memoria->end = strtol(auxiliar, NULL, 2);
      //outros campos 0
      memoria->rs = 0;
      memoria->rt = 0;
      memoria->rd = 0;
      memoria->imm = 0;
      memoria->funct = 0;
      break;
    case 8: //Tipo I (BEQ)
      memoria->tipo = TipoI;
      // OP
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // RS
      strncpy(auxiliar, memoria->binario + 4, 3);
      auxiliar[3] = '\0';
      memoria->rs = strtol(auxiliar, NULL, 2);
      // RT
      strncpy(auxiliar, memoria->binario + 7, 3);
      auxiliar[3] = '\0';
      memoria->rt = strtol(auxiliar, NULL, 2);
      // IMM
      strncpy(auxiliar, memoria->binario + 10, 6);
      auxiliar[6] = '\0';
      memoria->imm = strtol(auxiliar, NULL, 2);
      break;
    case 4: 
      memoria->tipo = TipoI;
      // OP
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // RS
      strncpy(auxiliar, memoria->binario + 4, 3);
      auxiliar[3] = '\0';
      memoria->rs = strtol(auxiliar, NULL, 2);
      // RT
      strncpy(auxiliar, memoria->binario + 7, 3);
      auxiliar[3] = '\0';
      memoria->rt = strtol(auxiliar, NULL, 2);
      // IMM
      strncpy(auxiliar, memoria->binario + 10, 6);
      auxiliar[6] = '\0';
      memoria->imm = BinParaDec(auxiliar);
      break;
      case 11: 
      memoria->tipo = TipoI;
      // OP
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // RS
      strncpy(auxiliar, memoria->binario + 4, 3);
      auxiliar[3] = '\0';
      memoria->rs = strtol(auxiliar, NULL, 2);
      // RT
      strncpy(auxiliar, memoria->binario + 7, 3);
      auxiliar[3] = '\0';
      memoria->rt = strtol(auxiliar, NULL, 2);
      // IMM
      strncpy(auxiliar, memoria->binario + 10, 6);
      auxiliar[6] = '\0';
      memoria->imm = BinParaDec(auxiliar);
      break;
      case 15: 
      memoria->tipo = TipoI;
      // OP
      strncpy(auxiliar, memoria->binario, 4);
      auxiliar[4] = '\0';
      memoria->op = strtol(auxiliar, NULL, 2);
      // RS
      strncpy(auxiliar, memoria->binario + 4, 3);
      auxiliar[3] = '\0';
      memoria->rs = strtol(auxiliar, NULL, 2);
      // RT
      strncpy(auxiliar, memoria->binario + 7, 3);
      auxiliar[3] = '\0';
      memoria->rt = strtol(auxiliar, NULL, 2);
      // IMM
      strncpy(auxiliar, memoria->binario + 10, 6);
      auxiliar[6] = '\0';
      memoria->imm = BinParaDec(auxiliar);
      break;
    }
}

void Ler_Arquivo(Memoria *instrucoes, int *qtd_instrucoes, char *nome_arquivo) {
    
    char line[Tam_Instrucao+1];
    int cont_local = 0;

    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char buffer[5];
        strncpy(buffer, line, 4);
        buffer[4] = '\0';

        strncpy(instrucoes[cont_local].binario, line, 16);
            instrucoes[cont_local].binario[16] = '\0';
            Decodicador(&instrucoes[cont_local]);
            cont_local++;

        if (cont_local >= Num_Instrucoes){ 
            break;
        }
    }

    fclose(file);
    *qtd_instrucoes = cont_local;
}

SaidasULA ULA(int ulaop, int operando1, int operando2){
    SaidasULA resultado;
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

void CarregarMemoriaDados(Pipeline *pipeline,char *nome_arquivo){
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        return;
    }
    fread(pipeline->Memoria_Dados,sizeof(int), 256, arquivo);
    fclose(arquivo);
}

void SalvarASM(Memoria *instrucoes, int cont,char *nome_arquivo){
    
    FILE *arquivo = fopen(nome_arquivo, "w");

    if (!arquivo) {
        return;
    }

    for (int i = 0; i < cont; i++) {

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
            case NOP:
                fprintf(arquivo, "NOP\n");
                        break;
                   
            default:
                fprintf(arquivo, "Instrucao invalida\n");
                break;
        }
    }

    fclose(arquivo);
     //printf("Arquivo .asm salvo com sucesso!\n");
}

void SalvarMemoriaDados(int memoria[],char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");

    if (!arquivo) {
        return;
    }

   fwrite(memoria, sizeof(int), 256, arquivo);
   
    fclose(arquivo);
}

void Controle(Memoria *instrucao, Pipeline *pipeline){
    // Escrita
    if (pipeline->mem_er.instrucao != NULL){
        
        if(pipeline->mem_er.sinaiscontrole.EscReg == 1){
            if(pipeline->mem_er.sinaiscontrole.MemParaReg == 1){
                if(pipeline->mem_er.sinaiscontrole.RegDst == 1){
                    pipeline->registradores[pipeline->mem_er.instrucao->rd] = pipeline->mem_er.ula.UlaSaida;
                }
                else{
                    pipeline->registradores[pipeline->mem_er.instrucao->rt] = pipeline->mem_er.ula.UlaSaida;
                }
            }
            else{
                pipeline->registradores[pipeline->mem_er.instrucao->rt] = pipeline->mem_er.dado_lido;
            }
        }
    }

    // Memória
    if (pipeline->ex_mem.instrucao != NULL){
        pipeline->mem_er.dado_lido = pipeline->Memoria_Dados[pipeline->ex_mem.ula.UlaSaida];
        if(pipeline->ex_mem.sinaiscontrole.EscMem == 1){
            pipeline->Memoria_Dados[pipeline->ex_mem.ula.UlaSaida] = pipeline->ex_mem.dado_a_ser_escrito;
        }
        if(pipeline->ex_mem.sinaiscontrole.DVI == 1){
            pipeline->pc = pipeline->ex_mem.instrucao->end;
        }
        else{
            if((pipeline->ex_mem.sinaiscontrole.DVC & pipeline->ex_mem.ula.UlaZero) == 1){
                pipeline->pc = pipeline->ex_mem.pc_beq;
            }
        }
        pipeline->mem_er.instrucao = pipeline->ex_mem.instrucao;
        pipeline->mem_er.sinaiscontrole = pipeline->ex_mem.sinaiscontrole;
        pipeline->mem_er.ula = pipeline->ex_mem.ula;
    }

    // Execução
    if (pipeline->di_ex.instrucao != NULL){
        pipeline->ex_mem.instrucao = pipeline->di_ex.instrucao;
        pipeline->ex_mem.pc_beq = pipeline->di_ex.pc_mais_um + pipeline->di_ex.imm;
        pipeline->ex_mem.dado_a_ser_escrito = pipeline->di_ex.dado_lido2;
        if (pipeline->di_ex.sinaiscontrole.UlaFonte == 0){
            pipeline->ula = ULA(pipeline->di_ex.sinaiscontrole.UlaOP, pipeline->di_ex.dado_lido1, pipeline->di_ex.dado_lido2);
        }
        else{
            pipeline->ula = ULA(pipeline->di_ex.sinaiscontrole.UlaOP, pipeline->di_ex.dado_lido1, pipeline->di_ex.imm);
        }
        pipeline->ex_mem.ula = pipeline->ula;
        pipeline->ex_mem.sinaiscontrole = pipeline->di_ex.sinaiscontrole;
    }

    // Decodificação
    if (pipeline->bi_di.instrucao != NULL){
        pipeline->di_ex.instrucao = pipeline->bi_di.instrucao;
        pipeline->di_ex.pc_mais_um = pipeline->bi_di.pc_mais_um;
        switch(pipeline->bi_di.instrucao->op){ // Slide 54
            case 0: //0000 tipo r
                pipeline->di_ex.dado_lido1 = pipeline->registradores[pipeline->bi_di.instrucao->rs];
                pipeline->di_ex.dado_lido2 = pipeline->registradores[pipeline->bi_di.instrucao->rt];
                pipeline->di_ex.sinaiscontrole.RegDst =     1;
                pipeline->di_ex.sinaiscontrole.UlaOP =      pipeline->bi_di.instrucao->funct;
                pipeline->di_ex.sinaiscontrole.UlaFonte =   0;
                pipeline->di_ex.sinaiscontrole.DVC =        0;
                pipeline->di_ex.sinaiscontrole.DVI =        0;
                pipeline->di_ex.sinaiscontrole.EscMem =     0;
                pipeline->di_ex.sinaiscontrole.EscReg =     1;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 1;
            break;
            case 11: //1011 lw
                pipeline->di_ex.sinaiscontrole.RegDst =     0;
                pipeline->di_ex.sinaiscontrole.UlaOP =      0;
                pipeline->di_ex.sinaiscontrole.UlaFonte =   1;
                pipeline->di_ex.sinaiscontrole.DVC =        0;
                pipeline->di_ex.sinaiscontrole.DVI =        0;
                pipeline->di_ex.sinaiscontrole.EscMem =     0;
                pipeline->di_ex.sinaiscontrole.EscReg =     1;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 0;
            break;
            case 15: //1111 sw
                pipeline->di_ex.imm = pipeline->bi_di.instrucao->imm;
                pipeline->di_ex.dado_lido1 = pipeline->registradores[pipeline->bi_di.instrucao->rs];
                pipeline->di_ex.dado_lido2 = pipeline->registradores[pipeline->bi_di.instrucao->rt];
                pipeline->di_ex.sinaiscontrole.RegDst =     0; //x
                pipeline->di_ex.sinaiscontrole.UlaOP =      0;
                pipeline->di_ex.sinaiscontrole.UlaFonte =   1;
                pipeline->di_ex.sinaiscontrole.DVC =        0;
                pipeline->di_ex.sinaiscontrole.DVI =        0;
                pipeline->di_ex.sinaiscontrole.EscMem =     1;
                pipeline->di_ex.sinaiscontrole.EscReg =     0;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 0; //x
            break;
            case 8: //1000 beq
                pipeline->di_ex.imm = pipeline->bi_di.instrucao->imm;
                pipeline->di_ex.dado_lido1 = pipeline->registradores[pipeline->bi_di.instrucao->rs];
                pipeline->di_ex.dado_lido2 = pipeline->registradores[pipeline->bi_di.instrucao->rt];
                pipeline->di_ex.sinaiscontrole.RegDst =     0; //x
                pipeline->di_ex.sinaiscontrole.UlaOP =      2; //sub
                pipeline->di_ex.sinaiscontrole.UlaFonte =   0;
                pipeline->di_ex.sinaiscontrole.DVC =        1;
                pipeline->di_ex.sinaiscontrole.DVI =        0;
                pipeline->di_ex.sinaiscontrole.EscMem =     0;
                pipeline->di_ex.sinaiscontrole.EscReg =     0;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 0; //x
            break;
            case 4: //0100 addi
                pipeline->di_ex.imm = pipeline->bi_di.instrucao->imm;
                pipeline->di_ex.dado_lido1 = pipeline->registradores[pipeline->bi_di.instrucao->rs];
                pipeline->di_ex.dado_lido2 = pipeline->registradores[pipeline->bi_di.instrucao->rt];
                pipeline->di_ex.sinaiscontrole.RegDst =     0;
                pipeline->di_ex.sinaiscontrole.UlaOP =      0;
                pipeline->di_ex.sinaiscontrole.UlaFonte =   1;
                pipeline->di_ex.sinaiscontrole.DVC =        0;
                pipeline->di_ex.sinaiscontrole.DVI =        0;
                pipeline->di_ex.sinaiscontrole.EscMem =     0;
                pipeline->di_ex.sinaiscontrole.EscReg =     1;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 1;
            break;
            case 2: //0010 j
                pipeline->di_ex.sinaiscontrole.RegDst =     0; //x
                pipeline->di_ex.sinaiscontrole.UlaOP =      0; //x
                pipeline->di_ex.sinaiscontrole.UlaFonte =   0; //x
                pipeline->di_ex.sinaiscontrole.DVC =        0;
                pipeline->di_ex.sinaiscontrole.DVI =        1;
                pipeline->di_ex.sinaiscontrole.EscMem =     0;
                pipeline->di_ex.sinaiscontrole.EscReg =     0;
                pipeline->di_ex.sinaiscontrole.MemParaReg = 0; //x
            break;
        }
    }

    // Busca
    if (instrucao != NULL) {
        pipeline->bi_di.instrucao = instrucao;
        pipeline->bi_di.pc_mais_um = pipeline->pc + 1;
        pipeline->pc = pipeline->pc + 1;
    }
}
//Pilha
void empilhar(No **topo, Pipeline* pipeline) {
    No *novoNo = (No *)malloc(sizeof(No));
    if (novoNo == NULL) {
        return;
    } 
    novoNo->pipeline = (Pipeline *)malloc(sizeof(Pipeline));
    Pipeline *salvo = novoNo->pipeline;

    salvo->pc = pipeline->pc;

    for (int i = 0; i < NUM_REGISTRADORES; i++) {
        salvo->registradores[i] = pipeline->registradores[i];
    }

    for (int i = 0; i < Tam_Memoria; i++) {
        salvo->Memoria_Dados[i] = pipeline->Memoria_Dados[i];
    }

    salvo->ula = pipeline->ula;
    salvo->sinaiscontrole = pipeline->sinaiscontrole;
    salvo->bi_di = pipeline->bi_di;
    salvo->di_ex = pipeline->di_ex;
    salvo->ex_mem = pipeline->ex_mem;
    salvo->mem_er = pipeline->mem_er;

    novoNo->proximo = *topo;
    *topo = novoNo;
}

void desempilhar(No **topo, Pipeline* pipeline) {
    if (*topo == NULL) {
        return; 
    }

    No *temp = *topo;
    pipeline->pc = temp->pipeline->pc;

    for (int i = 0; i < NUM_REGISTRADORES; i++) {
        pipeline->registradores[i] = temp->pipeline->registradores[i];
    }


    for (int i = 0; i < Tam_Memoria; i++) {
        pipeline->Memoria_Dados[i] = temp->pipeline->Memoria_Dados[i];
    }

    pipeline->ula = temp->pipeline->ula;
    pipeline->sinaiscontrole = temp->pipeline->sinaiscontrole;
    pipeline->bi_di = temp->pipeline->bi_di;
    pipeline->di_ex = temp->pipeline->di_ex;
    pipeline->ex_mem = temp->pipeline->ex_mem;
    pipeline->mem_er = temp->pipeline->mem_er;


    *topo = temp->proximo;

    free(temp); 
}

//funções do Ncurses
void draw_button(WINDOW *win, int y, int x, const char *label, bool is_selected) {
    if (is_selected) {
        wattron(win, A_REVERSE);
    }
    mvwprintw(win, y, x, "[%s]", label);
    wattroff(win, A_REVERSE);
}

void Painel_Estado(WINDOW *win, Pipeline *pipeline) {
    werase(win);
    box(win, 0, 0);

    int linha_atual = 1;

    //Cabeçalho
    wattron(win, A_BOLD);
    mvwprintw(win, linha_atual++, 2, "PAINEL DE ESTADO PIPELINE");
    wattroff(win, A_BOLD);
    linha_atual++;
    //PC
    wattron(win, A_BOLD);
    mvwprintw(win, linha_atual++, 2, "PC: %d", pipeline->pc);
    wattroff(win, A_BOLD);
    linha_atual++;
    //Registradores
    wattroff(win, A_BOLD);
    mvwprintw(win, linha_atual++, 2, "Registradores:");
    wattroff(win, A_BOLD);
    for (int i = 0; i < 4; i++) {
        mvwprintw(win, linha_atual, 2 + (i*10), "$%d: %d", i, pipeline->registradores[i]);
    }
    linha_atual++;
    for (int i = 0; i < 4; i++) {
        mvwprintw(win, linha_atual, 2 + (i*10), "$%d: %d", i+4, pipeline->registradores[i+4]);
    }
    linha_atual += 2;
    
    wattron(win, A_BOLD);
    mvwprintw(win,linha_atual, 2 ,"Registradores de PIPELINE");
    wattroff(win, A_BOLD);
    linha_atual+=2;
    
    //BI/DI
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual++, 2, "BI/DI");
    wattroff(win, A_BOLD );
    if(pipeline->bi_di.instrucao->binario!=NULL){
        mvwprintw(win, linha_atual++, 2, "Instrucao[%s] PC+1[%d] ", pipeline->bi_di.instrucao->binario, pipeline->bi_di.pc_mais_um);
    }else{
        mvwprintw(win, linha_atual++, 2, "Instrucao[0000000000000000] PC+1[%d] ", pipeline->bi_di.pc_mais_um);
    }
    linha_atual++;
    //BI/EX
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual++, 2, "DI/EX");
    wattroff(win, A_BOLD );
    if(pipeline->di_ex.instrucao->binario!=NULL){
        mvwprintw(win, linha_atual++, 2, "Instrucao[%s] PC+1[%d] ", pipeline->di_ex.instrucao->binario, pipeline->di_ex.pc_mais_um);
    }else{
        mvwprintw(win, linha_atual++, 2, "Instrucao[0000000000000000] PC+1[%d] ", pipeline->di_ex.pc_mais_um);
    }
    mvwprintw(win, linha_atual++, 2, "Dado Lido 1[%d] Dado Lido 2[%d] Imediato[%d]",
        pipeline->di_ex.dado_lido1,
        pipeline->di_ex.dado_lido2,
        pipeline->di_ex.imm
    );
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual, 2, "Sinais de controle");
    wattroff(win, A_BOLD );
    linha_atual++;
    mvwprintw(win, linha_atual++, 2, "RegDst[%d] UlaOP[%d] UlaFonte[%d] DVC[%d] DVI[%d]",
        pipeline->di_ex.sinaiscontrole.RegDst,
        pipeline->di_ex.sinaiscontrole.UlaOP,
        pipeline->di_ex.sinaiscontrole.UlaFonte,
        pipeline->di_ex.sinaiscontrole.DVC,
        pipeline->di_ex.sinaiscontrole.DVI
    );
    mvwprintw(win,linha_atual++,2,"EscMem[%d] MemParaReg[%d]",
        pipeline->di_ex.sinaiscontrole.EscMem,
        pipeline->di_ex.sinaiscontrole.MemParaReg
    );
    linha_atual++;
    
    //EX/MEM
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual++, 2, "EX/MEM");
    wattroff(win, A_BOLD);
    if(pipeline->ex_mem.instrucao->binario!=NULL){
        mvwprintw(win, linha_atual++, 2, "Instrucao[%s] PC BEQ[%d]", pipeline->ex_mem.instrucao->binario,pipeline->ex_mem.pc_beq);
    }else{
        mvwprintw(win, linha_atual++, 2, "Instrucao[0000000000000000] PC BEQ[%d]",pipeline->ex_mem.pc_beq);
    }
    mvwprintw(win, linha_atual++, 2, "ULA Overflow[%d] ULA Zero[%d] ULA Saida[%d] Dado a escrever[%d]",
        pipeline->ex_mem.ula.SinalOverflow,
        pipeline->ex_mem.ula.UlaZero,
        pipeline->ex_mem.ula.UlaSaida,
        pipeline->ex_mem.dado_a_ser_escrito
    );
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual, 2, "Sinais de controle");
    wattroff(win, A_BOLD );
    linha_atual++;
    mvwprintw(win, linha_atual++, 2, "RegDst[%d] UlaOP[%d] UlaFonte[%d] DVC[%d] DVI[%d]",pipeline->ex_mem.sinaiscontrole.RegDst,
        pipeline->ex_mem.sinaiscontrole.UlaOP,
        pipeline->ex_mem.sinaiscontrole.UlaFonte,
        pipeline->ex_mem.sinaiscontrole.DVC,
        pipeline->ex_mem.sinaiscontrole.DVI
    );
    mvwprintw(win,linha_atual++,2,"EscMem[%d] MemParaReg[%d]",
        pipeline->ex_mem.sinaiscontrole.EscMem,
        pipeline->ex_mem.sinaiscontrole.MemParaReg
    );
    linha_atual++;

    //MEM/ER
    wattron(win, A_BOLD) ;
    mvwprintw(win, linha_atual++, 2, "MEM/ER");
    wattroff(win, A_BOLD);
    if(pipeline->mem_er.instrucao->binario != NULL){
        mvwprintw(win, linha_atual++, 2, "Instrucao[%s]", pipeline->mem_er.instrucao->binario);
    }else{
        mvwprintw(win, linha_atual++, 2, "Instrucao[0000000000000000]");
    }
    mvwprintw(win, linha_atual++, 2, "ULA Overflow[%d] ULA Zero[%d] ULA Saida[%d] Dado lido[%d]",
        pipeline->mem_er.ula.SinalOverflow,
        pipeline->mem_er.ula.UlaZero,
        pipeline->mem_er.ula.UlaSaida,
        pipeline->mem_er.dado_lido
    );
    wattron(win, A_BOLD );
    mvwprintw(win, linha_atual, 2, "Sinais de controle");
    wattroff(win, A_BOLD );
    linha_atual++;
    mvwprintw(win, linha_atual++, 2, "RegDst[%d] UlaOP[%d] UlaFonte[%d] DVC[%d] DVI[%d]",pipeline->mem_er.sinaiscontrole.RegDst,
        pipeline->mem_er.sinaiscontrole.UlaOP,
        pipeline->mem_er.sinaiscontrole.UlaFonte,
        pipeline->mem_er.sinaiscontrole.DVC,
        pipeline->mem_er.sinaiscontrole.DVI
    );
    mvwprintw(win,linha_atual++,2,"EscMem[%d] MemParaReg[%d]",
        pipeline->mem_er.sinaiscontrole.EscMem,
        pipeline->mem_er.sinaiscontrole.MemParaReg
    );
    
    wrefresh(win);

}

void Painel_Pipeline(WINDOW *win, Pipeline *pipeline,Memoria *instrucao) {
    char auxiliar[20];

    werase(win);
    box(win, 0, 0);
    
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 2, "ESTAGIOS PIPELINE");
    wattroff(win, A_BOLD);

    //Busca
    wattron(win, A_BOLD);
    mvwprintw(win, 3, 2, "BI:");
    wattroff(win, A_BOLD);
    if (instrucao->binario != NULL && instrucao->tipo != NOP) {
        strcpy(auxiliar,identificainstrucao(instrucao)); 
        mvwprintw(win, 3, 7, "%s", auxiliar);
    } else {
        mvwprintw(win, 3, 7, "NOP");
    }

    //Decodificação
    wattron(win, A_BOLD);
    mvwprintw(win, 4, 2, "DI:");
    wattroff(win, A_BOLD);
    if(pipeline->pc == 0){
        mvwprintw(win, 4, 7, "NOP");
    }else if (pipeline->bi_di.instrucao != NULL && pipeline->bi_di.instrucao->tipo != NOP) {
        strcpy(auxiliar,identificainstrucao(pipeline->bi_di.instrucao)); 
        mvwprintw(win, 4, 7, "%s", auxiliar);
    } else {
        mvwprintw(win, 4, 7, "NOP");
    }

    //Execução
    wattron(win, A_BOLD);
    mvwprintw(win, 5, 2, "EX:");
    wattroff(win, A_BOLD);
    if (pipeline->di_ex.instrucao != NULL && pipeline->di_ex.instrucao->tipo != NOP) {
        strcpy(auxiliar,identificainstrucao(pipeline->di_ex.instrucao)); 
        mvwprintw(win, 5, 7, "%s", auxiliar);
    } else {
        mvwprintw(win, 5, 7, "NOP");
    }

    //Memória
    wattron(win, A_BOLD);
    mvwprintw(win, 6, 2, "MEM:");
    wattroff(win, A_BOLD);
    if (pipeline->ex_mem.instrucao != NULL && pipeline->ex_mem.instrucao->tipo != NOP) {
        strcpy(auxiliar,identificainstrucao(pipeline->ex_mem.instrucao)); 
        mvwprintw(win, 6, 7, "%s", auxiliar);
    } else {
        mvwprintw(win, 6, 7, "NOP");
    }

    //Escrita
    wattron(win, A_BOLD);
    mvwprintw(win, 7, 2, "ER:");
    wattroff(win, A_BOLD);
    if (pipeline->mem_er.instrucao != NULL && pipeline->mem_er.sinaiscontrole.EscReg == 1 && pipeline->mem_er.instrucao->tipo != NOP) {
        strcpy(auxiliar,identificainstrucao(pipeline->mem_er.instrucao)); 
        mvwprintw(win, 7, 7, "%s", auxiliar);
    } else {
        mvwprintw(win, 7, 7, "NOP");
    }
    wrefresh(win);
}

void Tela_Memoria_Instrucoes(WINDOW *win, Memoria *instrucoes) {
   werase(win);
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);//Coletando tamanho da tela

    // Título
    wattron(win, A_BOLD);
    mvwprintw(win, 0, (max_x - 25) / 2, "MEMORIA DE INSTRUCOES");
    wattroff(win, A_BOLD);

    int y_inicial = 1;
    int x_inicial = 1;
    int largura_coluna = 22;            //Espaço para [000]:0000000000000000
    int linhas_por_coluna = max_y - 2;  //A altura da coluna

    for (int i = 0; i < Num_Instrucoes; i++) {
        
        //Calcula a posição
        int coluna_alvo = i / linhas_por_coluna;
        int linha_alvo  = i % linhas_por_coluna;

        //Calcula as coordenadas X e Y na tela
        int pos_x = x_inicial + (coluna_alvo * largura_coluna);
        int pos_y = y_inicial + linha_alvo;

        //Já não cabe mais na tela.
        if (pos_x + largura_coluna > max_x) {
            break;
        }

        //Desenha
        mvwprintw(win, pos_y, pos_x, "[%d]:%s", i, instrucoes[i].binario);
    }

    //Espera pelo Enter
    wattron(win, A_BOLD);
    mvwprintw(win, max_y - 1, (max_x - 39) / 2, " Pressione ENTER para voltar ao menu ");
    wattroff(win, A_BOLD);
    wrefresh(win);
    while (wgetch(win) != 10 && wgetch(win) != KEY_ENTER) {}
}

void Tela_Memoria_Dados(WINDOW *win, Pipeline *pipeline) {
    werase(win);
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    // Título
    wattron(win, A_BOLD);
    mvwprintw(win, 1, (max_x - 18) / 2, "MEMORIA DE DADOS");
    wattroff(win, A_BOLD);

    int y_inicial = 3;
    int x_inicial = 2;
    int largura_coluna = 10;            //Espaço para [000]:0000
    int linhas_por_coluna = max_y - 4;  //Define a altura da coluna

    for (int i = 0; i < Tam_Memoria; i++) {
        
        //Calcula a posição
        int coluna_alvo = i / linhas_por_coluna;
        int linha_alvo  = i % linhas_por_coluna;

        //Calcula as coordenadas X e Y na tela
        int pos_x = x_inicial + (coluna_alvo * largura_coluna);
        int pos_y = y_inicial + linha_alvo;

        //Já não cabe mais na tela
        if (pos_x + largura_coluna > max_x) {
            break;
        }

        //Desenha
        mvwprintw(win, pos_y, pos_x, "[%d]: %d", i, pipeline->Memoria_Dados[i]);
    }

    //Espera pelo enter
    wattron(win, A_BOLD);
    mvwprintw(win, max_y - 1, (max_x - 39) / 2, " Pressione ENTER para voltar ao menu ");
    wattroff(win, A_BOLD);
    wrefresh(win);
    while (wgetch(win) != 10 && wgetch(win) != KEY_ENTER) {}
}

char* identificainstrucao(Memoria *instrucao) {
    char *auxiliar = malloc(50);  // Aloca memória suficiente para a string
    if (!auxiliar){
        return NULL;  
    } 

    if (instrucao->op == 0) {
        switch (instrucao->funct) {
            case 0:
                sprintf(auxiliar, "add $%d, $%d, $%d", instrucao->rd, instrucao->rs, instrucao->rt);
                break;
            case 2:
                sprintf(auxiliar, "sub $%d, $%d, $%d", instrucao->rd, instrucao->rs, instrucao->rt);
                break;
            case 4:
                sprintf(auxiliar, "and $%d, $%d, $%d", instrucao->rd, instrucao->rs, instrucao->rt);
                break;
            case 5:
                sprintf(auxiliar, "or $%d, $%d, $%d", instrucao->rd, instrucao->rs, instrucao->rt);
                break;
            default:
                sprintf(auxiliar, "funct desconhecido (%d)", instrucao->funct);
                break;
        }
        return auxiliar;
    }

    switch (instrucao->op) {
        case 4:
            sprintf(auxiliar, "addi $%d, $%d, %d", instrucao->rt, instrucao->rs, instrucao->imm);
            break;
        case 2:
            sprintf(auxiliar, "j %d", instrucao->end);
            break;
        case 8:
            sprintf(auxiliar, "beq $%d, $%d, %d", instrucao->rt, instrucao->rs, instrucao->imm);
            break;
        case 11:
            sprintf(auxiliar, "lw $%d, %d($%d)", instrucao->rt, instrucao->imm, instrucao->rs);
            break;
        case 15:
            sprintf(auxiliar, "sw $%d, %d($%d)", instrucao->rt, instrucao->imm, instrucao->rs);
            break;
        default:
            sprintf(auxiliar, "opcode desconhecido (%d)", instrucao->op);
            break;
    }

    return auxiliar;
}