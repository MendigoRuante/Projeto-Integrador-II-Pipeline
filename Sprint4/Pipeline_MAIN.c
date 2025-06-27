#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "Pipeline_TAD.h"

int main() {
    //Incialização do Simulador
    int cont = 0;
    Memoria* instrucoes = malloc(sizeof(Memoria) * Num_Instrucoes);
    Pipeline *pipeline = malloc(sizeof(Pipeline));
    No *Pilha = NULL;
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
        instrucoes[i].tipo = NOP;
        instrucoes[i].funct = 0;
        instrucoes[i].op = 0;
        instrucoes[i].rs = 0;
        instrucoes[i].rt = 0;
        instrucoes[i].rd = 0;
    }
    //Inicialização do Ncurses
    initscr(); //Inicia o Ncurses
    cbreak(); //Ativa Teclas especiais
    noecho(); //Não permite entrada do teclado
    curs_set(0); //Esconde o Cursor

    //Coordenadas do Menu
    int menu_y = ((LINES - MENU_ALTURA - MSG_WIN_ALTURA) / 2) - 10;
    int menu_x = ((COLS - MENU_LARGURA) / 2)-20;
    
    //Janela do menu
    WINDOW *menu_win = newwin(MENU_ALTURA, MENU_LARGURA, menu_y, menu_x);
    keypad(menu_win, TRUE);//Habilita as setas

    //Inicia Janela de mensagens simulando um terminal
    int msg_win_y = menu_y + MENU_ALTURA;
    WINDOW *msg_win = newwin(MSG_WIN_ALTURA, MENU_LARGURA, msg_win_y, menu_x);

    //Inicia o Painel dos estados
    int reg_startx = menu_x + MENU_LARGURA + 5;
    WINDOW *Regis_win  = newwin(MENU_ALTURA + 22, 62, menu_y, reg_startx);
    //Inicia o Painel do Pipeline
    int pipe_starty = msg_win_y + 6;
    WINDOW *Pipe_win = newwin(10, 35,pipe_starty, menu_x);

    Painel_Pipeline(Pipe_win,pipeline,&instrucoes[pipeline->pc]);
    Painel_Estado(Regis_win, pipeline);

    int choice = -1;
    int highlight = 0;
    int ch;

    while (1) {
        wclear(msg_win); // Limpa a janela de mensagens 
        box(msg_win, 0, 0); // Desenha a borda da janela de mensagens 
        wrefresh(msg_win);//Atualiza

        
        highlight = 0;
        choice = -1;
        
        do {
            wclear(menu_win);
            box(menu_win, 0, 0);//Desenha a borda do menu

            draw_button(menu_win, 1, 2, "Carregar Memoria de Instrucoes (.mem)", (highlight == 0));
            draw_button(menu_win, 2, 2, "Carregar Memoria de Dados (.dat)", (highlight == 1));
            draw_button(menu_win, 3, 2, "Mostrar Memoria de Instrucoes", (highlight == 2));
            draw_button(menu_win, 4, 2, "Mostrar Memoria de Dados", (highlight == 3));
            draw_button(menu_win, 5, 2, "RUN", (highlight == 4));
            draw_button(menu_win, 6, 2, "STEP", (highlight == 5));
            draw_button(menu_win, 7, 2, "STEP-BACK", (highlight == 6));
            draw_button(menu_win, 8, 2, "Salvar .asm", (highlight == 7));
            draw_button(menu_win, 9, 2, "Salvar .dat", (highlight == 8));
            draw_button(menu_win, 11, 2, "Sair", (highlight == 9));


            wrefresh(menu_win);
            ch = wgetch(menu_win);

            switch(ch) {
                case KEY_UP:    
                    if (highlight > 0){
                    highlight--; 
                    } 
                        
                    break;
                case KEY_DOWN:  
                if (highlight < 9){ 
                    highlight++;
                    }
                break;
                case '\n':      
                    choice = highlight; 
                    break;
            }
        } while (choice == -1);
        
        // Limpa a janela de mensagens
        wclear(msg_win);
        box(msg_win, 0, 0);

        switch (choice) {
            case 0: {
                char nome_arquivo[100];
                echo();
                curs_set(1);
                mvwprintw(msg_win, 1, 2, "Digite o nome do arquivo .mem:");
                wmove(msg_win, 2, 2);
                wrefresh(msg_win);
                wgetstr(msg_win, nome_arquivo); 
                noecho();
                curs_set(0);
                
                Ler_Arquivo(instrucoes, &cont, nome_arquivo);
                
                wclear(msg_win);
                box(msg_win, 0, 0);
                mvwprintw(msg_win, 2, 2, "Arquivo carregado. %d instrucoes.", cont);
                wrefresh(msg_win);
                napms(2000); // Espera 2 segundos
                break;
            }
            case 1:
                char nome_arquivo[100];
                echo();
                curs_set(1);
                mvwprintw(msg_win, 1, 2, "Digite o nome do arquivo .dat:");
                wmove(msg_win, 2, 2);
                wrefresh(msg_win);
                wgetstr(msg_win, nome_arquivo); 
                noecho();
                curs_set(0);

                CarregarMemoriaDados(pipeline,nome_arquivo);

                wclear(msg_win);
                box(msg_win,0,0);
                mvwprintw(msg_win, 2, 2, "Memoria de dados carregada.");
                wrefresh(msg_win);
                napms(2000);
                break;
            case 2:
                Tela_Memoria_Instrucoes(stdscr, instrucoes);
                clear();
                refresh();
                break;
            case 3:
                Tela_Memoria_Dados(stdscr, pipeline);
                clear();
                refresh();
                break;
            case 4: // RUN
                mvwprintw(msg_win, 1, 2, "Executando RUN");
                wrefresh(msg_win);
                while (pipeline->pc < cont+5) {
                    empilhar(&Pilha,pipeline);
                    
                    Controle(&instrucoes[pipeline->pc], pipeline);
                   
                }
                mvwprintw(msg_win, 2, 2, "Execução concluida.");
                wrefresh(msg_win);
                napms(1000);
                break;

            case 5: // STEP
                empilhar(&Pilha,pipeline);
                Controle(&instrucoes[pipeline->pc], pipeline);
                Painel_Estado(Regis_win, pipeline);
                break;
            case 6: //BACK
                if (pipeline->pc == 0){
                    mvwprintw(msg_win, 1, 2, "Pilha Vazia");
                    break;
                }
                desempilhar(&Pilha,pipeline);
                Painel_Pipeline(Pipe_win, pipeline,&instrucoes[pipeline->pc]);
                Painel_Estado(Regis_win, pipeline);
                break;
            case 7:
                char nome_arquivo_asm[100];
                echo();
                curs_set(1);
                mvwprintw(msg_win, 1, 2, "Digite um nome para o arquivo .asm:");
                wmove(msg_win, 2, 2);
                wrefresh(msg_win);
                wgetstr(msg_win, nome_arquivo_asm); 
                noecho();
                curs_set(0);

                SalvarASM(instrucoes,cont,nome_arquivo_asm);

                wclear(msg_win);
                box(msg_win,0,0);
                mvwprintw(msg_win, 2, 2, "Arquivo ASM salvo.");
                wrefresh(msg_win);
                napms(2000);
                break;
            case 8:
                char nome_arquivo_dat[100];
                echo();
                curs_set(1);
                mvwprintw(msg_win, 1, 2, "Digite um nome para o arquivo .dat:");
                wmove(msg_win, 2, 2);
                wrefresh(msg_win);
                wgetstr(msg_win, nome_arquivo_dat); 
                noecho();
                curs_set(0);

                SalvarMemoriaDados(pipeline->Memoria_Dados,nome_arquivo_dat);
        
                wclear(msg_win);
                box(msg_win,0,0);
                mvwprintw(msg_win, 2, 2, "Memoria de dados salva.");
                wrefresh(msg_win);
                napms(2000);
                break;
            case 9:
                endwin();
                free(instrucoes);
                free(pipeline);
                free(Pilha);
                printf("Simulador encerrado.\n");
                return 0;
            }
            Painel_Estado(Regis_win, pipeline);
            Painel_Pipeline(Pipe_win, pipeline,&instrucoes[pipeline->pc]);
            
    }

    endwin();
    return 0;
}