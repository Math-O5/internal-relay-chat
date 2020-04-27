#ifndef TERMINAL_H
    #define TERMINAL_H 1

    #include <termios.h>    /* struct termios */
    #include <pthread.h>    /* pthread_create, pthread_mutex_t */

    /**
     * @ terminal_control
     * 
     * responsável por encapsular as operações de input/output com o 
     * terminal do usuário.
     * 
     * Ref.: https://www.reddit.com/r/C_Programming/comments/64524q/turning_off_echo_in_terminal_using_c/
     */

        #define LINE_BUFFER 8192 // 8 KB

        typedef struct _terminal_control {
            char                input[LINE_BUFFER];
            pthread_mutex_t*    terminal_mutex;
            int                 input_enabled;
        } terminal_control;


        /**
         * Configurações e Funções Principais 
         */

            // Habilita a propriedade ECHO do terminal.
            void echo_enable(terminal_control* t);

            // Disabilita a propriedade ECHO do terminal.
            void echo_disable(terminal_control* t);

            // Handler responsável por lidar com o terminal.
            void terminal_loop_handler(terminal_control* t);

        /**
         * Mensagens e outputs formatados
         */

            // Mensagem enviada assim que o programa inicia, dando os comandos básicos.
            void msg_inicio(terminal_control* t);




#endif