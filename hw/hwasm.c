/*    SPDX-FileCopyrightText: 2024 Maria Júlia De Grandi <maju.degrandi@usp.br>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 512
#define MAX_LABELS 10

// Estrutura para armazenar o código binário gerado
typedef struct {
    unsigned char *data;
    size_t size;
    size_t capacity;
} BinaryData;

// Estrutura para armazenar rótulos e seus endereços
typedef struct {
    char label[50];
    size_t address;
} LabelEntry;

LabelEntry labels[MAX_LABELS];
int num_labels = 0;

void process_jump(char *line, BinaryData *bin_data);
void process_fill(char *line, BinaryData *bin_data);
void process_word(char *line, BinaryData *bin_data);

// Função para inicializar a estrutura de dados binários
BinaryData *init_binary_data() {
    BinaryData *bin_data = (BinaryData *)malloc(sizeof(BinaryData));
    if (bin_data == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    bin_data->size = 0;
    bin_data->capacity = BUFFER_SIZE;
    bin_data->data = (unsigned char *)malloc(bin_data->capacity);
    if (bin_data->data == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    return bin_data;
}

void add_byte(BinaryData *bin_data, unsigned char byte) {
    if (bin_data->size >= bin_data->capacity) {
        bin_data->capacity *= 2;
        bin_data->data = (unsigned char *)realloc(bin_data->data, bin_data->capacity);
        if (bin_data->data == NULL) {
            perror("Memory reallocation failed");
            exit(EXIT_FAILURE);
        }
    }
    bin_data->data[bin_data->size++] = byte;
}

void process_jump(char *line, BinaryData *bin_data) {
    // Implementação do processamento de saltos
    // Adiciona um byte fixo 0xeb
    printf("Processing jump instruction: %s\n", line);
    add_byte(bin_data, 0xeb);

    char *label_jmp = strtok(line, " "); 

    size_t dest_address = 0;
    for (int i = 0; i < num_labels; i++) {
        if (strcmp(labels[i].label, label_jmp) == 0) {
            dest_address = labels[i].address;
            break;
        }
    }

    // Calcula o deslocamento como um valor negativo
    int displacement = (int)(dest_address - bin_data->size - 1);
    add_byte(bin_data, displacement);

    printf("Jump instruction processed. Current size: %zu\n", bin_data->size);
}

void process_fill(char *line, BinaryData *bin_data) {
    char *token;
    token = strtok(line, ",");
    char *repeat_expr = token;

    token = strtok(NULL, ",");
    int size = atoi(token);

    token = strtok(NULL, " ");
    int value = atoi(token);

    printf("Processing fill directive: repeat=%s, size=%d, value=%d\n", repeat_expr, size, value);

    // Processa a expressão de repetição
    int repeat = 0;
    int current_address = bin_data->size;

    // Verifica se a expressão contém uma operação com labels
    token = strtok(repeat_expr, " ");
    if (token != NULL) {
        char *op = strtok(NULL, " ");
        char *label1 = strtok(NULL, " ");
        char *label2, *op2;
        if(label1[0] == '(')
        {
            op2 = strtok(NULL, " ");
            label2 = strtok(NULL, ")");
            label1 = strtok(label1+1, " ");
        }
        else label2 = strtok(NULL, " ");

        int addr1 = -1;
        int addr2 = -1;

        if(strcmp(label1, ".") == 0) addr1 = current_address;
        if(strcmp(label2, ".") == 0) addr2 = current_address;
    
        // Busca os endereços dos labels na estrutura de dados
        for (int i = 0; i < num_labels; i++) {
            if (addr1 != current_address && strcmp(labels[i].label, label1) == 0) {
                addr1 = labels[i].address;
            }
            if (addr2 != current_address && strcmp(labels[i].label, label2) == 0) {
                addr2 = labels[i].address;
            }
        }

        // Calcula o valor da expressão
        if (strncmp(op, "-", 1) == 0) {
            if (strncmp(op2, "-", 1) == 0) repeat = atoi(token) - (addr1 - addr2);
            if (strncmp(op2, "+", 1) == 0) repeat = atoi(token) - (addr1 + addr2);
        } else if (strncmp(op, "+", 1) == 0) {
            if (strncmp(op2, "-", 1) == 0) repeat = atoi(token) + (addr1 - addr2);
            if (strncmp(op2, "+", 1) == 0) repeat = atoi(token) + (addr1 + addr2);
        }
    } else {
        // Caso a expressão seja um valor fixo
        repeat = atoi(repeat_expr);
    }

    printf("Processing fill directive: repeat=%d, size=%d, value=%d\n", repeat, size, value);

    // Preenche o binário com os valores especificados
    for (int i = 0; i < repeat * size; i++) {
        add_byte(bin_data, value);
    }

    printf("Fill directive processed. Current size: %zu\n", bin_data->size);
}


void process_word(char *line, BinaryData *bin_data) {
    // Extrai o valor da palavra (16 bits)
    unsigned int word = strtol(line, NULL, 16);

    printf("Processing word directive: value=%04x\n", word);

    // Adiciona os bytes da palavra ao código binário
    add_byte(bin_data, word & 0xFF);         // Byte menos significativo
    add_byte(bin_data, (word >> 8) & 0xFF);  // Byte mais significativo

    printf("Word directive processed. Current size: %zu\n", bin_data->size);
}

void process_directive(char *line, BinaryData *bin_data) {
    // Avança além do ponto para o início do nome da diretiva
    line++;

    // Remove os espaços em branco iniciais
    while (*line == ' ') line++;

    // Verifica o tipo de diretiva
    if (strncmp(line, "code16", 6) == 0) {
        // Neste caso, não há ação específica além de reconhecer a diretiva
        fprintf(stderr,"eu li code16\n");
    } else if (strncmp(line, "global", 6) == 0) {
        // Diretiva .global - Ignora para fins deste assembler simples
        fprintf(stderr,"eu li global\n");
        // Não precisa de ação específica além de reconhecer a diretiva
    } else if (strncmp(line, "fill", 4) == 0) {
        line += 5;
        process_fill(line, bin_data);
    } else if (strncmp(line, "word", 4) == 0) {
        line += 5;
        process_word(line, bin_data);
    } else {
        // Diretiva desconhecida - Reportar erro
        fprintf(stderr, "Error: Unknown directive: %s\n", line);
        exit(EXIT_FAILURE);
    }
}

void process_label(char *line, BinaryData *bin_data) {
    // Remove o caractere ':' do rótulo
    line[strlen(line) - 1] = '\0';

    // Salva o rótulo e o endereço atual
    strncpy(labels[num_labels].label, line, sizeof(labels[num_labels].label));
    labels[num_labels].address = bin_data->size;

    num_labels++;
    printf("Label processed: %s\n", line);
}

void process_instruction(char *line, BinaryData *bin_data) {
    if (strncmp(line, "movb", 4) == 0) {
        line += 5;  // Avança até o primeiro argumento
        // Extrai os argumentos
        char *token;
        token = strtok(line, ",");  // Separador é ',' ou ' '
        if (token[0] == '$') {
            unsigned char byte;
            if (token[1] == '\'') {  // Verifica se é um caractere literal
                byte = (unsigned char)token[2];  // Converte o caractere literal
            } else {
                byte = (unsigned char)strtol(token + 1, NULL, 0);  // Converte o byte em número
            }
            token = strtok(NULL, ", ");
            unsigned char reg = token[2] == 'h' ? 0xb4 : 0xb0;  // Verifica se é 'ah' ou 'al'
            add_byte(bin_data, reg);
            add_byte(bin_data, byte);
        } else {
            // Caso o token não seja um valor imediato
            printf("Erro: Operando imediato esperado.\n");
        }
    } else if (strncmp(line, "int", 3) == 0) {
        line += 4;  // Avança até o valor da interrupção
        unsigned char int_val = (unsigned char)strtol(line + 1, NULL, 0);
        add_byte(bin_data, 0xcd);  // Opcode para interrupção
        add_byte(bin_data, int_val);
    } else if (strncmp(line, "jmp", 3) == 0) {
        line += 4;  // Avança até o rótulo
        // Extrai o rótulo e processa o salto relativo
        process_jump(line, bin_data);
    } else if (strncmp(line, "hlt", 3) == 0) {
        add_byte(bin_data, 0xf4);  // Código da instrução HLT
    } else {
        // Ignora linhas que não são diretivas, rótulos ou instruções
    }
}

// Função para montar o arquivo assembly em código binário
void assemble(const char *input_file, BinaryData *bin_data) {
    FILE *fp = fopen(input_file, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, fp) != NULL) {
        // Remove o caractere de nova linha no final da linha lida
        line[strcspn(line, "\n")] = '\0';

        // Faz uma cópia da linha para manipulação
        char *line_copy = line;

        // Remove os espaços em branco iniciais
        while (*line_copy == ' ') line_copy++;

        // Verifica se a linha não está vazia após remover espaços em branco
        if (*line_copy != '\0') {
            // Verifica se é um comentário
            if (*line_copy == '/' || *line_copy == '*') continue;
            // Verifica se é uma diretiva
            else if (*line_copy == '.') process_directive(line_copy, bin_data);
            // Verifica se é um rótulo
            else if (line[strlen(line) - 1] == ':') process_label(line, bin_data);
            // Caso contrário, é uma instrução
            else process_instruction(line_copy, bin_data);
        }
    }

    fclose(fp);
}

// Função para escrever o código binário em um arquivo de saída
void write_binary(const char *output_file, BinaryData *bin_data) {
    FILE *fp = fopen(output_file, "wb");
    if (fp == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Preenche o arquivo binário até 512 bytes, se necessário
    while (bin_data->size < BUFFER_SIZE) add_byte(bin_data, 0x00);

    // Escreve os dados no arquivo
    fwrite(bin_data->data, 1, bin_data->size, fp);

    fclose(fp);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    BinaryData *bin_data = init_binary_data();
    assemble(argv[1], bin_data);
    write_binary(argv[2], bin_data);

    free(bin_data->data);
    free(bin_data);
    return EXIT_SUCCESS;
}
