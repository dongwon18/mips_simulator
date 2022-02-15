#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


typedef struct R_type{
    int func;
    int type;
    char oper[10];
} R_type;

typedef struct I_type{
    int opcode;
    int type;
    char oper[10];
} I_type;

typedef struct J_type{
    int opcode;
    char oper[10];
} J_type;

// function declare
int get_command(char **command, char **file_name);
int inst_count(FILE *bin_file);
int hextobin(char hex_code[][9], int count, int hex_to_bin[][32]);
void get_hex_file(FILE *bin_file, int count, char hex_code[][9]);
int get_opcode(int hex_to_bin[][32], int i);
int R_type_print(int hex_to_bin[][32], int i, R_type R_opcode[], char hex_code[][9]);
void I_type_print(int hex_to_bin[][32], int i, int opcode, I_type I_opcode[], char hex_code[][9]);
void J_type_print(int hex_to_bin[][32], int i, int opcode, J_type J_opcode[], char hex_code[][9]);

int main(){
    R_type R_opcode[27];

    R_opcode[0].func = 32;
    R_opcode[0].type = 1;
    strcpy(R_opcode[0].oper, "add");

    R_opcode[1].func = 33;
    R_opcode[1].type = 1;
    strcpy(R_opcode[1].oper, "addu");

    R_opcode[2].func = 36;
    R_opcode[2].type = 1;
    strcpy(R_opcode[2].oper, "and");

    R_opcode[3].func = 26;
    R_opcode[3].type = 2;
    strcpy(R_opcode[3].oper, "div");

    R_opcode[4].func = 27;
    R_opcode[4].type = 2;
    strcpy(R_opcode[4].oper, "divu");

    R_opcode[5].func = 9;
    R_opcode[5].type = 3;
    strcpy(R_opcode[5].oper, "jalr");

    R_opcode[6].func = 8;
    R_opcode[6].type = 4;
    strcpy(R_opcode[6].oper, "jr");

    R_opcode[7].func = 16;
    R_opcode[7].type = 5;
    strcpy(R_opcode[7].oper, "mfhi");

    R_opcode[8].func = 18;
    R_opcode[8].type = 5;
    strcpy(R_opcode[8].oper, "mflo");

    R_opcode[9].func = 17;
    R_opcode[9].type = 4;
    strcpy(R_opcode[9].oper, "mthi");

    R_opcode[10].func = 19;
    R_opcode[10].type = 4;
    strcpy(R_opcode[10].oper, "mtlo");

    R_opcode[11].func = 24;
    R_opcode[11].type = 2;
    strcpy(R_opcode[11].oper, "mult");

    R_opcode[12].func = 25;
    R_opcode[12].type = 2;
    strcpy(R_opcode[12].oper, "multu");

    R_opcode[13].func = 39;
    R_opcode[13].type = 1;
    strcpy(R_opcode[13].oper, "nor");

    R_opcode[14].func = 37;
    R_opcode[14].type = 1;
    strcpy(R_opcode[14].oper, "or");

    R_opcode[15].func = 0;
    R_opcode[15].type = 6;
    strcpy(R_opcode[15].oper, "sll");

    R_opcode[16].func = 4;
    R_opcode[16].type = 8;
    strcpy(R_opcode[16].oper, "sllv");

    R_opcode[17].func = 42;
    R_opcode[17].type = 1;
    strcpy(R_opcode[17].oper, "slt");

    R_opcode[18].func = 43;
    R_opcode[18].type = 1;
    strcpy(R_opcode[18].oper, "sltu");

    R_opcode[19].func = 3;
    R_opcode[19].type = 6;
    strcpy(R_opcode[19].oper, "sra");

    R_opcode[20].func = 7;
    R_opcode[20].type = 8;
    strcpy(R_opcode[20].oper, "srav");

    R_opcode[21].func = 2;
    R_opcode[21].type = 6;
    strcpy(R_opcode[21].oper, "srl");

    R_opcode[22].func = 6;
    R_opcode[22].type = 8;
    strcpy(R_opcode[22].oper, "srlv");

    R_opcode[23].func = 34;
    R_opcode[23].type = 1;
    strcpy(R_opcode[23].oper, "sub");

    R_opcode[24].func = 35;
    R_opcode[24].type = 1;
    strcpy(R_opcode[24].oper, "subu");

    R_opcode[25].func = 12;
    R_opcode[25].type = 7;
    strcpy(R_opcode[25].oper, "syscall");

    R_opcode[26].func = 38;
    R_opcode[26].type = 1;
    strcpy(R_opcode[26].oper, "xor");


    I_type I_opcode[18];

    I_opcode[0].opcode = 8;
    I_opcode[0].type = 1;
    strcpy(I_opcode[0].oper, "addi");

    I_opcode[1].opcode = 9;
    I_opcode[1].type = 1;
    strcpy(I_opcode[1].oper, "addiu");

    I_opcode[2].opcode = 12;
    I_opcode[2].type = 1;
    strcpy(I_opcode[2].oper, "andi");

    I_opcode[3].opcode = 4;
    I_opcode[3].type = 3;
    strcpy(I_opcode[3].oper, "beq");

    I_opcode[4].opcode = 5;
    I_opcode[4].type = 3;
    strcpy(I_opcode[4].oper, "bne");

    I_opcode[5].opcode = 32;
    I_opcode[5].type = 4;
    strcpy(I_opcode[5].oper, "lb");

    I_opcode[6].opcode = 36;
    I_opcode[6].type = 4;
    strcpy(I_opcode[6].oper, "lbu");

    I_opcode[7].opcode = 33;
    I_opcode[7].type = 4;
    strcpy(I_opcode[7].oper, "lh");

    I_opcode[8].opcode = 37;
    I_opcode[8].type = 4;
    strcpy(I_opcode[8].oper, "lhu");

    I_opcode[9].opcode = 15;
    I_opcode[9].type = 2;
    strcpy(I_opcode[9].oper, "lui");

    I_opcode[10].opcode = 35;
    I_opcode[10].type = 4;
    strcpy(I_opcode[10].oper, "lw");

    I_opcode[11].opcode = 13;
    I_opcode[11].type = 1;
    strcpy(I_opcode[11].oper, "ori");

    I_opcode[12].opcode = 40;
    I_opcode[12].type = 4;
    strcpy(I_opcode[12].oper, "sb");

    I_opcode[13].opcode = 10;
    I_opcode[13].type = 1;
    strcpy(I_opcode[13].oper, "slti");

    I_opcode[14].opcode = 11;
    I_opcode[14].type = 1;
    strcpy(I_opcode[14].oper, "sltiu");

    I_opcode[15].opcode = 41;
    I_opcode[15].type = 4;
    strcpy(I_opcode[15].oper, "sh");

    I_opcode[16].opcode = 43;
    I_opcode[16].type = 4;
    strcpy(I_opcode[16].oper, "sw");

    I_opcode[17].opcode = 14;
    I_opcode[17].type = 1;
    strcpy(I_opcode[17].oper, "xori");

    J_type J_opcode[2];

    J_opcode[0].opcode = 2;
    strcpy(J_opcode[0].oper, "j");

    J_opcode[1].opcode = 3;
    strcpy(J_opcode[1].oper, "jal");

    //  pointer string to save user's command and file name
    char *command = (char *) malloc(sizeof(char)*100);
    char *file_name = (char *) malloc(sizeof(char)*100);
    int exit;  //  to check if user's input is exit

    int count;  //  no. of command in bin file

    FILE *bin_file;

    exit = get_command(&command, &file_name);

    int opcode;  //  store decimal opcode
    int exist_command;  //  store return value of R_type_print()
    int hextobin_error_check;

    //  run until user inputs 'exit'
    while(exit == 0){
        // file opening error check
        bin_file = fopen(file_name, "rb");
        while(bin_file == NULL){
            printf("file opening failed.\n");
            exit = get_command(&command, &file_name);
            //if(strcmp(file_name, "no_file") != 0){
               // printf("strcmp : %d", strcmp(file_name, "no_file"));

           // }
            if(exit == 1)
                break;
            bin_file = fopen(file_name, "rb");
        }
        if(exit == 1)
            break;

        count = inst_count(bin_file);

        char hex_code[count][9];  // store hex code

        get_hex_file(bin_file, count, hex_code);

        int hex_to_bin[count][32];  //  store bin code

        hextobin_error_check = hextobin(hex_code, count, hex_to_bin);

        if(hextobin_error_check == 1){
            printf("error while reading bin file\n");
        }

        for(int i = 0; i < count; i++){
            opcode = get_opcode(hex_to_bin, i);
            printf("inst %d: ", i);
            if(opcode == 0){
                // r type save and print
                // if no func, no command
                exist_command = R_type_print(hex_to_bin, i, R_opcode, hex_code);
                if(exist_command == 1){
                    printf("%s unknown instruction\n", hex_code[i]);
                }
            }else if((opcode == 2) || (opcode == 3)){
                // j type save and print func
                J_type_print(hex_to_bin, i, opcode, J_opcode, hex_code);
            }else if((opcode == 4) || (opcode == 5) || ((opcode >= 8) && (opcode <= 10)) || ((opcode >= 11) && (opcode <= 15)) || (opcode == 32)
                     || ((opcode >= 35) && (opcode <= 37)) || (opcode == 40) || (opcode == 41) || (opcode == 43)){
                // i type save and print func
                I_type_print(hex_to_bin, i, opcode, I_opcode, hex_code);
            }else{
                // no command
                printf("unknown instruction\n");
            }
        }


        fclose(bin_file);

        exit = get_command(&command, &file_name);
        if(exit == 1)
            break;
    }

    free(command);
    free(file_name);
    return 0;
}

/**
 * input : pointer string command and file_name's address
 * input/output : none
 * output :
 *    1) assign string's address to command and file_name
 *    2) return 1 when exit is entered
 *    3) return 0 when exit is not entered
 *
 * get user command containing black characters
 * divide command and file_name
 * it will not give error sign if user enter neither read nor exit
 */
int get_command(char **command, char **file_name){
    char user_input[100];
    char *token;

    printf("mips-sim> ");
    fgets(user_input, sizeof(user_input), stdin);  //  get user's keyboard input
    user_input[strlen(user_input) - 1] = '\0';  //  mark end of the input

    //  divide the input by space
    token = strtok(user_input, " ");
    strcpy(*command, token);
    if(strcmp(*command, "exit") == 0){
        //  to prevent segmentation error
        strcpy(*file_name, "no_file");
        return 1;
    }
    token = strtok(NULL, " ");
    strcpy(*file_name, token);

    return 0;
}

/**
 * input : file pointer bin_file
 * input/output : none
 * output : return int count
 *
 * get the number of command in the bin file
 */
int inst_count(FILE *bin_file){
    fseek(bin_file, 0, SEEK_END);
    int count = ftell(bin_file) / 4;  //  4byte machine
    fseek(bin_file, 0, SEEK_SET);  //  set to first

    return count;
}

/**
 * input :
 *     1) 2d array of hex_bin that contains character type hexa code
 *     2) int count that contains no. of command in the bin file
 * input/output : 2d array to save int bin code
 * output :
 *    1) return int 1 if the input hexa code contains unexpected char
 *       expected char : 0 ~ 9, a ~ f
 *    2) return int 0 if no error
 *
 * change a char hex code to 4 bit int code
 * ex.) a -> 1010
 */
int hextobin(char hex_code[][9], int count, int hex_to_bin[][32]){
        for(int i = 0; i <count; i++){
            for(int j = 0; j < 8; j++){
                char hexa = hex_code[i][j];

                if(hexa == '0'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == '1'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == '2'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == '3'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == '4'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == '5'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == '6'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == '7'){
                    hex_to_bin[i][4*j] = 0;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == '8'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == '9'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == 'a'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == 'b'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 0;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == 'c'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == 'd'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 0;
                    hex_to_bin[i][4*j + 3] = 1;
                }else if(hexa == 'e'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 0;
                }else if(hexa == 'f'){
                    hex_to_bin[i][4*j] = 1;
                    hex_to_bin[i][4*j + 1] = 1;
                    hex_to_bin[i][4*j + 2] = 1;
                    hex_to_bin[i][4*j + 3] = 1;
                }else{
                    return 1;
                }
        }
    }
    return 0;
}

/**
 * input :
 *    1) file pointer that points bin file to open
 *    2) int count that has no. of command in the bin file
 * input/output : 2d char array to save hexa code
 * output : none
 *
 * get raw data from the file and save it in hex_code as hexa code
 */
void get_hex_file(FILE *bin_file, int count, char hex_code[][9]){
    int ch;
    char buff[3];

    for(int i = 0; i < count; i++){
        //  get data as 2 hexa code
        for(int j = 0; j <4; j++){
            if(ch != EOF){
                ch = fgetc(bin_file);
                sprintf(buff, "%02x", ch);
                hex_code[i][2*j] = buff[0];
                hex_code[i][2*j + 1] = buff[1];
            }else{
                break;
            }
        }
        hex_code[i][8] = '\0';  //  mark the end of the string
    }
}

/**
 * input :
 *    1) 2d int array that contains binary code
 *    2) index for getting opcode of hex_to_bin[i]
 * input/output : none
 * output : return int opcode
 *
 * compute binary to decimal opcode
 */
int get_opcode(int hex_to_bin[][32], int i){
    int opcode;

    opcode = hex_to_bin[i][0]*(int)pow(2, 5) + hex_to_bin[i][1]*(int)pow(2, 4) + hex_to_bin[i][2]*(int)pow(2, 3)
        + hex_to_bin[i][3]*(int)pow(2, 2) + hex_to_bin[i][4]*(int)pow(2, 1) + hex_to_bin[i][5]*(int)pow(2, 0);

    return opcode;
}

/**
 * input :
 *    1) 2d int array that contains binary code
 *    2) index to be printed
 *    3) R_opcode - to compare and find the operator, type and function
 *    4) hex_code[][9] - to print hexa code
 * input/output : none
 * output :
 *    1) return 1 when there is no such function in R_type
 *    2) return 0 if there is no error
 *    3) print hexa code, operator, register no.
 */
int R_type_print(int hex_to_bin[][32], int i, R_type *R_opcode, char hex_code[][9]){
    // if return 0 no prob if 1 return no command
    int rs;
    int rt;
    int rd;
    int sa;
    int func;
    int type;
    char oper[10];
    int find;

    //  change binary code to deciaml value
    rs = hex_to_bin[i][6]*(int)pow(2, 4) + hex_to_bin[i][7]*(int)pow(2, 3) + hex_to_bin[i][8]*(int)pow(2, 2) + hex_to_bin[i][9]*(int)pow(2, 1) + hex_to_bin[i][10]*(int)pow(2, 0);
    rt = hex_to_bin[i][11]*(int)pow(2, 4) + hex_to_bin[i][12]*(int)pow(2, 3) + hex_to_bin[i][13]*(int)pow(2, 2) + hex_to_bin[i][14]*(int)pow(2, 1) + hex_to_bin[i][15]*(int)pow(2, 0);
    rd = hex_to_bin[i][16]*(int)pow(2, 4) + hex_to_bin[i][17]*(int)pow(2, 3) + hex_to_bin[i][18]*(int)pow(2, 2) + hex_to_bin[i][19]*(int)pow(2, 1) + hex_to_bin[i][20]*(int)pow(2, 0);
    sa = hex_to_bin[i][21]*(int)pow(2, 4) + hex_to_bin[i][22]*(int)pow(2, 3) + hex_to_bin[i][23]*(int)pow(2, 2) + hex_to_bin[i][24]*(int)pow(2, 1) + hex_to_bin[i][25]*(int)pow(2, 0);
    func = hex_to_bin[i][26]*(int)pow(2, 5) + hex_to_bin[i][27]*(int)pow(2, 4) + hex_to_bin[i][28]*(int)pow(2, 3)
        + hex_to_bin[i][29]*(int)pow(2, 2) + hex_to_bin[i][30]*(int)pow(2, 1) + hex_to_bin[i][31]*(int)pow(2, 0);

    //  find func that matches in R_type
    //  if there is such function, assign 1 to find
    for(int j = 0; j < 27; j++){
        if(func == R_opcode[j].func){
            type = R_opcode[j].type;
            strcpy(oper, R_opcode[j].oper);
            find = 1;
        }
    }
    //  if find is not 1(function is not found), alert error
    if(find != 1){
        return 1;
    }

    //  print hexa code and register as defined.
    if(type == 1){
        printf("%s %s $%d, $%d, $%d\n", hex_code[i], oper, rd, rs, rt);
        return 0;
    }else if(type == 2){
        printf("%s %s $%d, $%d\n", hex_code[i], oper, rs, rt);
        return 0;
    }else if(type == 3){
        printf("%s %s $%d, $%d\n", hex_code[i], oper, rd, rs);
        return 0;
    }else if(type == 4){
        printf("%s %s $%d\n", hex_code[i], oper, rs);
        return 0;
    }else if(type == 5){
        printf("%s %s $%d\n", hex_code[i], oper, rd);
        return 0;
    }else if(type == 6){
        printf("%s %s $%d, $%d, %d\n", hex_code[i], oper, rd, rt, sa);
        return 0;
    }else if(type == 7){
        printf("%s %s\n", hex_code[i], oper);
        return 0;
    }else{
        // type == 8
        printf("%s %s $%d, $%d, $%d\n", hex_code[i], oper, rd, rt, rs);
        return 0;
    }
}

/**
 * input :
 *    1) hex_to_bin[][32] - int array that contains binary code
 *    2) i - index to be printed
 *    3) opcode - opcode(decimal) of hex_to_bin[i]
 *    4) I_opcode - to compare and find operator, type
 *    5) hex_code[][9] - to print hexa code
 * input/output : none
 * output : print hexa code, operator, register or immediate
 *
 * get decimal values of rs, rt, im
 * find matching opcode in I_opcode
 * print the values as defined
 * immediate is 2's complement signed integer
 */
void I_type_print(int hex_to_bin[][32], int i, int opcode, I_type *I_opcode, char hex_code[][9]){
    int rs;
    int rt;
    int im = 0;
    int type;
    char oper[10];

    for(int j = 0; j < 18; j++){
        if(opcode == I_opcode[j].opcode){
            type = I_opcode[j].type;
            strcpy(oper, I_opcode[j].oper);
        }
    }

    //  if msb == 1, which means negative value
    //      im = im - pow(2, 16)
    rs = hex_to_bin[i][6]*(int)pow(2, 4) + hex_to_bin[i][7]*(int)pow(2, 3) + hex_to_bin[i][8]*(int)pow(2, 2) + hex_to_bin[i][9]*(int)pow(2, 1) + hex_to_bin[i][10]*(int)pow(2, 0);
    rt = hex_to_bin[i][11]*(int)pow(2, 4) + hex_to_bin[i][12]*(int)pow(2, 3) + hex_to_bin[i][13]*(int)pow(2, 2) + hex_to_bin[i][14]*(int)pow(2, 1) + hex_to_bin[i][15]*(int)pow(2, 0);
    if(hex_to_bin[i][16] == 1){
        for(int k = 0; k < 16; k++){
            im += hex_to_bin[i][k + 16] * (int)pow(2, 15-k);
        }
        im = im - (int)pow(2, 16);
    }else{
        for(int k = 0; k < 16; k++){
            im += hex_to_bin[i][k + 16] * (int)pow(2, 15-k);
        }
    }

    if(type == 1){
        printf("%s %s $%d, $%d, %d\n", hex_code[i], oper, rt, rs, im);
    }else if(type == 2){
        printf("%s %s $%d, %d\n", hex_code[i], oper, rt, im);
    }else if(type == 3){
        printf("%s %s $%d, $%d, %d\n", hex_code[i], oper, rs, rt, im);
    }else{
        //  type == 4
        printf("%s %s $%d, %d($%d)\n", hex_code[i], oper, rt, im, rs);
    }
}

/**
 * input :
 *    1) hex_to_bin[][32] - int array that contains binary code
 *    2) i - index to be printed
 *    3) J_opcode - to find operator
 *    4) hex_code[][9] - to print hexa code
 * input/output : none
 * output : print operator, target
 *
 * change binary value to decimal value
 * find matching opcode form the J_opcode
 * print the operator and target
 * target is 2's complement signed integer
 */
void J_type_print(int hex_to_bin[][32], int i, int opcode, J_type *J_opcode, char hex_code[][9]){
    int target = 0;
    char oper[10];

    for(int j = 0; j < 2; j++){
        if(opcode == J_opcode[j].opcode){
            strcpy(oper, J_opcode[j].oper);
        }
    }

    if(hex_to_bin[i][6] == 1){
        for(int k = 0; k < 26; k++){
            target += hex_to_bin[i][k + 6] * (int)pow(2, 25-k);
        }
        target = target - (int)pow(2, 26);
    }else{
        for(int k = 0; k < 26; k++){
            target += hex_to_bin[i][k + 6] * (int)pow(2, 25-k);
        }
    }
    printf("%s %s %d\n", hex_code[i], oper, target);
}
