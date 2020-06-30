/**
 * #include "terminal.h"
 * ---------------------
 * 
 * Biblioteca responsável por definir/resetar algumas configurações necessárias
 * para a boa interação do usuário com o console. Essa biblioteca utiliza os
 * recursos disponibilizados pela biblioteca <termios.h> que permite que o console
 * tenha os atributos configurados durante a execução do programa.
 * 
 * O motivo principal para a existência dessa bibliteca era o de garantir que caso
 * o usuário estivesse no meio de um input nenhum componente poderia exibir conteúdo
 * na tela até que o input fosse finalizado ou cancelado.
 * 
 * O segundo motivo era o de impedir que o conteúdo digitado pelo usuário interferisse
 * a exibição de alguma mensagem.
 * 
 * Para mais detalhes veja a struct e as funções definidas abaixo.
 * 
 * Referências utilizadas:
 * -  https://www.reddit.com/r/C_Programming/comments/64524q/turning_off_echo_in_terminal_using_c/
 * -  http://man7.org/linux/man-pages/man3/termios.3.html
 */

#ifndef TERMINAL_H
    #define TERMINAL_H 1

    #include <termios.h>    /* struct termios */
    #include <pthread.h>    /* pthread_create, pthread_mutex_t */

    /**
     * Constantes e Macros
     * -------------------
     * 
     * LINE_BUFFER
     * Tamanho máximo que o input de algum usuário pode tomar. Valores 
     * acima disso são simplesmente ignorados e descartados.
     * 
     * MAX_MESSAGE_LENGHT
     * Valor replicado de chat.h apenas para manter uma consistencia dos
     * buffers utilizados ao longo do programa.
     *  
     * BUFFER_SIZE
     * Tamanho máximo que o buffer de output pode ocupar.
     */

        #define MAX_MESSAGE_LENGHT 512
        #define LINE_BUFFER ((MAX_MESSAGE_LENGHT * 5) + 1)
        #define BUFFER_SIZE ((MAX_MESSAGE_LENGHT * 30) + 1)
        #define INPUT_ENABLED 1
        #define INPUT_DISABLED 0

        #define COLORL_GRAY     "\033[0;30m"
        #define COLOR_RED       "\033[0;31m"
        #define COLOR_GREEN     "\033[0;32m"
        #define COLOR_YELLOW    "\033[0;33m"
        #define COLOR_BLUE      "\033[0;34m"
        #define COLOR_MAGENTA   "\033[0;35m"
        #define COLOR_CYAN      "\033[0;36m"
        #define COLOR_WHITE     "\033[0;37m"
        
        #define COLORB_GRAY      "\033[1;30m"
        #define COLORB_RED       "\033[1;31m"
        #define COLORB_GREEN     "\033[1;32m"
        #define COLORB_YELLOW    "\033[1;33m"
        #define COLORB_BLUE      "\033[1;34m"
        #define COLORB_MAGENTA   "\033[1;35m"
        #define COLORB_CYAN      "\033[1;36m"
        #define COLORB_WHITE     "\033[1;37m"

        #define COLOR_RESET      "\033[0m"

        #define PREFIX_NORMAL       "  \033[1;37m[-]\033[0;37m   "
        #define PREFIX_EMPTY_LINE   "  \033[1;37m[ ]\033[0;37m   \n"
        #define PREFIX_MESSAGE      "  \033[1;37m[M]\033[0;37m   "

        #define PREFIX_SUCCESS      "  \033[1;32m[+]   SUCESSO: \033[0;32m"
        #define PREFIX_ERROR        "  \033[1;31m[x]   ERRO: \033[0;31m"
        #define PREFIX_SERVER       "  \033[1;33m[S]   SERVIDOR: \033[0;33m"
        #define PREFIX_CHANNEL      "  \033[1;34m[C]   CANAL: \033[0;34m"

        #define PREFIX_INPUT        "  \033[1;33m[$]:  \033[1;37m"


    /**
     * @struct
     * typedef struct _terminal_control terminal_control
     * -------------------------------------------------
     * 
     * A struct terminal control é utilizada para garantir que input e 
     * output de diferentes threads não serão executadas simultaneamentes
     * no console do usuário, causando bugs e glitches de exibição.
     * 
     * Para isso o terminal conta com 2 buffers, um de input de tamanho mais limitado
     * e outro de output. Além disso, o mesmo conta com um mutex que deve ser utilizado
     * sempre que as threads desejarem realizar alguma operação no mesmo.
     * 
     * Por fim, o terminal_control conta com a funcionalidade de Ativar/Desativar o modo
     * input do usuário.
     * 
     * MODO INPUT_ENABLED: Todos os outputs são pausados e podem ser armazenados em um buffer
     * durante o período de espera. O console fica em foco aguardando a entrada do usuário e salva
     * o conteúdo no buffer de input. 
     * 
     * MODO INPUT_DISABLED: Todos os inputs do usuário são ignorados e as teclas digitadas não são
     * exibidas na tela.
     * 
     * Para alternar entre os modos, bata que o usuário pressiona ENTER.
     * 
     * input_enabled: variável booleana que define o modo atual do terminal.
     * 
     * input: Buffer de input em que é armazenado os dados salvos quando o input está habilitado.
     * 
     * output_buffer: buffer em que os dados podem ser armazenados enquanto o terminal está em 
     *                modo de input.
     * 
     * buffer_size: define o tamanho atual do buffer de output.
     * 
     * terminal_mutex: mutex que deve ser lockado sempre que alguma thread desejar realizar uma
     * ação de print_f no terminal (*)
     * 
     * (*) Ações de leituras não devem ser feitas diretamente, mas sim utilizando o recurso
     *     criado por essa biblioteca (veja: função terminal_input_iteration)
     */
        typedef struct _terminal_control {
            int   input_enabled;
            char  input[LINE_BUFFER];
            char  output_buffer[BUFFER_SIZE];
            int   buffer_size;
            pthread_mutex_t*    terminal_mutex;

        } terminal_control;


        /**
         * @function
         * void echo_enable(&t)
         * --------------------
         * 
         * Ativa o modo ECHO do console, dessa forma todas os caracteres digitados pelo
         * usuário são devidamente exibidos no terminal. Além disso atualiza t->input_enabled
         * para INPUT_ENABLED
         */
        void echo_enable(terminal_control* t);

        /**
         * @function
         * void echo_disable(&t)
         * --------------------
         * 
         * Desativa o modo ECHO do console, dessa forma todas os caracteres digitados pelo
         * usuário são devidamente exibidos no terminal. Além disso atualiza t->input_enabled
         * para INPUT_DISABLED
         */
        void echo_disable(terminal_control* t);

        /**
         * @function
         * int terminal_input_iteration(&t);
         * ---------------------------------
         * 
         * Essa função foi criada para ser inserida na thread que será utilizada como responsável
         * pelo input do usuário e é responsável por garantir o bom funcionamento do console.
         * 
         * Resumidamente, a mesma fica fiscalizando se o usuário tem o interesse de ativar o modo
         * INPUT_ENABLED verificando se o mesmo pressionou a tecla ENTER.
         * Em caso positivo, a função solicita o lock do terminal e reativa o modo ECHO, aguardando
         * o input do usuário. Ao finalizar o input, a função salva os dados em seu buffer de input
         * e retorna o valor 1 para informar que o usuário realizou de fato algum input.
         * 
         * O modo INPUT_ENABLED é desativado logo após o fim da inserção do usuário, devendo ser 
         * reativado na próxima iteração.
         * 
         * return: 1 caso haja algum input aguardando para ser processado.
         *         0 caso o usuário não tenha inserido nenhum input durante essa iteração.
         */
        int terminal_input_iteration(terminal_control* t);
        
        /**
         * @custom_message
         * void msg_inicio(&t)
         * -------------------
         * Encapsula uma sequência de printf's exibida assim que o programa é iniciado.
         */
        void msg_inicio(terminal_control* t);

        



#endif