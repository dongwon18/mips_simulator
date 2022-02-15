#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
typedef struct inst{
    char hex_code[9];
    int binary_code[32];
    int opcode;
    int rs;
    int rt;
    int rd;
    int sa;
    int func;
    int imm;
    int exist;
} Inst_mem;

typedef struct regi{
    int binary_code[32];
    int value;
} Regi;

typedef struct mem{
    char hex_code[9];
    int binary_code[32];
    int value;
    int len;
} Mem;

// function declare
int get_command(char **command, char **file_name);
int inst_count(FILE *bin_file);
int hextobin(Inst_mem inst[], int count);
int hextobin_mem(Mem mem[], int count);
void get_hex_file(FILE *bin_file, int count, Inst_mem inst[]);
void get_hex_file_mem(FILE *bin_file, int count, Mem mem[]);
int get_opcode(Inst_mem inst[], int i);
void R_type_save(Inst_mem inst[], int i);
void I_type_save(Inst_mem inst[], int i);
void initialization(Inst_mem inst[], Regi regi[]);
void initialize_regi(Regi regi[]);
void initialize_mem(Mem mem[]);
void dectobin(int decimal, int bin[32]);
void dectobin_64(int64_t decimal, int bin[64]);
int bintodec(int bin[32]);
void print_regi(Regi regi[32], int pc);
int simulate(Inst_mem inst[], int execute, Regi regi[], Mem mem[], int *pc);
int hextodec(char hex);

int main(){
     //  max no. instruction : 16 * 1024
   // Inst_mem inst[16384];
    Inst_mem *inst = (Inst_mem *)malloc(sizeof(Inst_mem)*16384);
    Mem *mem = (Mem *)malloc(sizeof(Mem)*16384);
    Regi regi[34]; // 33: Lo, 34: Hi

    initialization(inst, regi);

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
    int pc = 0;


    //  run until user inputs 'exit'
    while(exit == 0 || exit == 2){
        if(strcmp(command, "loadinst") == 0){
            initialization(inst, regi);
            pc = 0;
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

            get_hex_file(bin_file, count, inst);

            hextobin_error_check = hextobin(inst, count);

            if(hextobin_error_check == 1){
                printf("error while reading bin file\n");
            }

            for(int i = 0; i < count; i++){
                inst[i].opcode = get_opcode(inst, i);
//                printf("inst %d opcode %d: ", i, inst[i].opcode);
                if(inst[i].opcode == 0){
                    // r type save and print
                    // if no func, no command
                    R_type_save(inst, i);
                }else if(((inst[i].opcode >= 2) && (inst[i].opcode <= 5))|| ((inst[i].opcode >= 8) && (inst[i].opcode <= 15))||
                         (inst[i].opcode == 32) || (inst[i].opcode == 33)
                         ||(inst[i].opcode == 35) || (inst[i].opcode == 36)
                         || (inst[i].opcode == 37) || (inst[i].opcode == 40)
                         || (inst[i].opcode == 41) || (inst[i].opcode == 43)){
                    // i type save and print func
                    I_type_save(inst, i);
                    inst[i].exist = 1;
                }else{
                    // no command
                    inst[i].exist = 0;
                }
            }
            fclose(bin_file);
        }else if(strcmp(command, "loaddata") == 0){
            initialize_mem(mem);
            bin_file = fopen(file_name, "rb");
            while(bin_file == NULL){
                printf("data file opening failed.\n");
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
            get_hex_file_mem(bin_file, count, mem);

            hextobin_error_check = hextobin_mem(mem, count);
            if(hextobin_error_check == 1){
                printf("error while reading data bin file\n");
            }
            //printf("cnt data : %d\n",count);
            for(int i = 0; i < count; i++){
                mem[i].value = bintodec(mem[i].binary_code);
               // printf("i mem value: %08x\n", mem[i].value);
            }
            fclose(bin_file);
        }else if (strcmp(command, "run") == 0){
            initialize_regi(regi);
            int N = atoi(file_name);
            int type_error;

            int execute = 0;
            int index = 0;

            while(inst[index].exist == 1 && execute < N){
                // simulate func
//                printf("exist : %d\n", inst[execute].exist);
                index = pc / 4;
                type_error = simulate(inst, index, regi, mem, &pc);
                //printf("%d, %d %d %d %d %d /", inst[index].opcode, inst[index].rs, inst[index].rt, inst[index].rd, inst[index].func, inst[index].imm);
                //printf("%d index %d PC : %d\n", execute, index, pc);
                execute++;
                if(type_error == 0){
                    pc = pc + 4;
                }else if(type_error == 1 || type_error == 2){
                    pc = pc + 4;
                    break;
                }else if(type_error == 5){
                    pc = pc + 4;
                    break;
                }
            }

            if(inst[index].exist == 0 && execute != N){
                //pc += 4;
                //execute += 1;
                //index = pc / 4;
                printf("unknown instruction\n");
               // printf("%d, %d %d %d %d %d\n", inst[execute].opcode, inst[execute].rs, inst[execute].rt, inst[execute].rd, inst[execute].func, inst[execute].imm);
            }

            printf("Executed %d instructions\n", execute);
            execute = 0;

        }else if(exit == 2){
            print_regi(regi, pc);
            pc = 0;
        }

        exit = get_command(&command, &file_name);
        if(exit == 1)
            break;
    }

    free(command);
    free(file_name);
    free(inst);
    free(mem);
    return 0;
}

/**
 * input : pointer string command and file_name's address
 * input/output : none
 * output :
 *    1) assign string's address to command and file_name
 *    2) return 1 when exit is entered
 *    3) return 0 when exit is not entered
 *    4) return 2 when register is entered
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
    }else if(strcmp(*command, "registers") == 0){
        strcpy(*file_name, "print_regi");
        return 2;
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
    int count;
    fseek(bin_file, 0, SEEK_END);

    int byte_num = ftell(bin_file);  //  4byte machine
    //printf("byte_num :%d\n", byte_num);
//    printf("byte_num%4 : %d", (int)byte_num%4);
    int mul_4 = 4 - (byte_num % 4);
   // printf("mul_4 : %d\n", mul_4);
    if(byte_num % 4 == 0){
        count = byte_num / 4;
    }else{
        byte_num = byte_num + mul_4;
      //  printf("byte_num :%d\n", byte_num);
        count = byte_num / 4;
    }
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
int hextobin(Inst_mem inst[], int count){
    int hex_to_bin[32];
        for(int i = 0; i <count; i++){
            for(int j = 0; j < 8; j++){
                char hexa = inst[i].hex_code[j];

                if(hexa == '0'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '1'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '2'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '3'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '4'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '5'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '6'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '7'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '8'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '9'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'a'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'b'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'c'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'd'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'e'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'f'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else{
                    return 1;
                }
        }
        for(int k = 0; k < 32; k++){
            inst[i].binary_code[k] = hex_to_bin[k];
        }
    }
    return 0;
}

int hextobin_mem(Mem mem[], int count){
    int hex_to_bin[32];
        for(int i = 0; i <count; i++){
            for(int j = 0; j < 8; j++){
                char hexa = mem[i].hex_code[j];

                if(hexa == '0'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '1'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '2'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '3'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '4'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '5'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '6'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '7'){
                    hex_to_bin[4*j] = 0;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == '8'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == '9'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'a'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'b'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 0;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'c'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'd'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 0;
                    hex_to_bin[4*j + 3] = 1;
                }else if(hexa == 'e'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 0;
                }else if(hexa == 'f'){
                    hex_to_bin[4*j] = 1;
                    hex_to_bin[4*j + 1] = 1;
                    hex_to_bin[4*j + 2] = 1;
                    hex_to_bin[4*j + 3] = 1;
                }else{
                    return 1;
                }
        }
        for(int k = 0; k < 32; k++){
            mem[i].binary_code[k] = hex_to_bin[k];
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
void get_hex_file(FILE *bin_file, int count, Inst_mem inst[]){
    int ch;
    char buff[3];

    for(int i = 0; i < count; i++){
        //  get data as 2 hexa code
        for(int j = 0; j <4; j++){
            if(ch != EOF){
                ch = fgetc(bin_file);
                sprintf(buff, "%02x", ch);
                inst[i].hex_code[2*j] = buff[0];
                inst[i].hex_code[2*j + 1] = buff[1];
            }else{
                break;
            }
        }
        inst[i].hex_code[8] = '\0';  //  mark the end of the string
    }
}

void get_hex_file_mem(FILE *bin_file, int count, Mem mem[]){
    int ch;
    char buff[3] = {0};

/*    for(int i = 0; i < count; i++){
        for(int j = 0; j < 8; j++){
            mem[i].hex_code[j] = '0';
        }
    }*/

    for(int i = 0; i < count; i++){
        //  get data as 2 hexa code
        for(int j = 0; j <4; j++){
            if(ch != EOF){
                ch = fgetc(bin_file);
                if(ch == EOF)
                    break;
                sprintf(buff, "%02x", ch);
                mem[i].hex_code[2*j] = buff[0];
                mem[i].hex_code[2*j + 1] = buff[1];
                mem[i].len += 2;
            }else{
                break;
            }
        }
        mem[i].hex_code[8] = '\0';  //  mark the end of the string
        //printf("hex_mem : %s len : %d\n", mem[i].hex_code, mem[i].len);
    }
}

/**
 * input :
 *    1) 2d int array that contains binary code
 *    2) index for getting opcode of inst[i]'s binary_code
 * input/output : Inst_mem inst
 * output : return int opcode
 *
 * compute binary to decimal opcode
 */
int get_opcode(Inst_mem inst[], int i){
    int opcode;

    opcode = inst[i].binary_code[0]*(int)pow(2, 5) + inst[i].binary_code[1]*(int)pow(2, 4) + inst[i].binary_code[2]*(int)pow(2, 3)
        + inst[i].binary_code[3]*(int)pow(2, 2) + inst[i].binary_code[4]*(int)pow(2, 1) + inst[i].binary_code[5]*(int)pow(2, 0);

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
void R_type_save(Inst_mem inst[], int i){
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
    rs = inst[i].binary_code[6]*(int)pow(2, 4) + inst[i].binary_code[7]*(int)pow(2, 3) + inst[i].binary_code[8]*(int)pow(2, 2) + inst[i].binary_code[9]*(int)pow(2, 1) + inst[i].binary_code[10]*(int)pow(2, 0);
    rt = inst[i].binary_code[11]*(int)pow(2, 4) + inst[i].binary_code[12]*(int)pow(2, 3) + inst[i].binary_code[13]*(int)pow(2, 2) + inst[i].binary_code[14]*(int)pow(2, 1) + inst[i].binary_code[15]*(int)pow(2, 0);
    rd = inst[i].binary_code[16]*(int)pow(2, 4) + inst[i].binary_code[17]*(int)pow(2, 3) + inst[i].binary_code[18]*(int)pow(2, 2) + inst[i].binary_code[19]*(int)pow(2, 1) + inst[i].binary_code[20]*(int)pow(2, 0);
    sa = inst[i].binary_code[21]*(int)pow(2, 4) + inst[i].binary_code[22]*(int)pow(2, 3) + inst[i].binary_code[23]*(int)pow(2, 2) + inst[i].binary_code[24]*(int)pow(2, 1) + inst[i].binary_code[25]*(int)pow(2, 0);
    func = inst[i].binary_code[26]*(int)pow(2, 5) + inst[i].binary_code[27]*(int)pow(2, 4) + inst[i].binary_code[28]*(int)pow(2, 3)
        + inst[i].binary_code[29]*(int)pow(2, 2) + inst[i].binary_code[30]*(int)pow(2, 1) + inst[i].binary_code[31]*(int)pow(2, 0);

    //  find func that matches in R_type
    //  if there is such function, assign 1 to find
    if((func == 0) || ((func >= 2) && (func <= 4)) || ((func >= 6) && (func <= 9))
        || ((func >= 32) && (func <= 39)) || ((func >= 42) && (func <= 43)) ||
       ((func >= 24) && (func <= 27)) || ((func >= 16) && (func <= 19)) || (func == 12)){
        inst[i].exist = 1;
    }else{
        inst[i].exist = 0;
    }

    inst[i].rs = rs;
    inst[i].rt = rt;
    inst[i].rd = rd;
    inst[i].sa = sa;
    inst[i].func = func;

//    printf("rs %d rt %d rd %d sa %d func %d\n", inst[i].rs, inst[i].rt, inst[i].rd, inst[i].sa, inst[i].func);
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
void I_type_save(Inst_mem inst[], int i){
    int rs;
    int rt;
    int im = 0;

    //  if msb == 1, which means negative value
    //      im = im - pow(2, 16)
    rs = inst[i].binary_code[6]*(int)pow(2, 4) + inst[i].binary_code[7]*(int)pow(2, 3) + inst[i].binary_code[8]*(int)pow(2, 2) + inst[i].binary_code[9]*(int)pow(2, 1) + inst[i].binary_code[10]*(int)pow(2, 0);
    rt = inst[i].binary_code[11]*(int)pow(2, 4) + inst[i].binary_code[12]*(int)pow(2, 3) + inst[i].binary_code[13]*(int)pow(2, 2) + inst[i].binary_code[14]*(int)pow(2, 1) + inst[i].binary_code[15]*(int)pow(2, 0);
    if(inst[i].binary_code[16] == 1){
        for(int k = 0; k < 16; k++){
            im += inst[i].binary_code[k + 16] * (int)pow(2, 15-k);
        }
        im = im - (int)pow(2, 16);
    }else{
        for(int k = 0; k < 16; k++){
            im += inst[i].binary_code[k + 16] * (int)pow(2, 15-k);
        }
    }

    inst[i].rs = rs;
    inst[i].rt = rt;
    inst[i].imm = im;
//    printf("rs %d rt int *pc%d imm %04x\n", inst[i].rs, inst[i].rt, inst[i].imm);

/**
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
*/
}


void initialization(Inst_mem inst[], Regi regi[]){
    for(int i = 0; i < 16384; i++){
        strcpy(inst[i].hex_code, "ffffffff");
        inst[i].opcode = 63;  //  111111
        inst[i].func = 63;  //  111111
        inst[i].exist = 0;
        for(int j = 0; j < 32; j++){
            inst[i].binary_code[j] = 1;
        }
    }
    for(int i = 0; i < 34; i++){
        regi[i].value = 0;
        for(int j = 0; j < 32; j++){
            regi[i].binary_code[j] = 0;
        }

    }
}

void initialize_regi(Regi regi[]){
    for(int i = 0; i < 34; i++){
        regi[i].value = 0;
        for(int j = 0; j < 32; j++){
            regi[i].binary_code[j] = 0;
        }
    }
}

void initialize_mem(Mem mem[]){
    for(int i = 0; i < 16384; i++){
        strcpy(mem[i].hex_code, "ffffffff");
        mem[i].value = -1;
        mem[i].len = 0;
        for(int j = 0; j < 32; j++){
            mem[i].binary_code[j] = 1;
        }
    }
}

/**
 * return 0 : no error
 * return 1 : address out of range
 * return 2 : address alignment violation
 * return 4 : do not add 4 to pc
 * return 5 : syscall exit
 */
int simulate(Inst_mem inst[], int execute, Regi regi[], Mem mem[], int *pc){
    //execute = *pc / 4;
    int opcode = inst[execute].opcode;
    int rs = inst[execute].rs;
    int rt = inst[execute].rt;
    int rd = inst[execute].rd;
    int sa = inst[execute].sa;
    int func = inst[execute].func;
    int imm = inst[execute].imm;
    int err_case = 0;
    //  and
    //printf("sim %d %d %d %d %d %d \n", inst[execute].opcode, inst[execute].rs, inst[execute].rt, inst[execute].rd, inst[execute].func, inst[execute].imm);
    if(opcode == 0){
        if(func == 32 || func == 33){
            //  add, addu
            regi[rd].value = regi[rs].value + regi[rt].value;
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 36){
            //  and
            for(int i = 0; i < 32; i++){
                regi[rd].binary_code[i] = regi[rs].binary_code[i] && regi[rt].binary_code[i];
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 39){
            //  nor
            for(int i = 0; i < 32; i++){
                regi[rd].binary_code[i] = !(regi[rs].binary_code[i] || regi[rt].binary_code[i]);
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 37){
            //  or
            for(int i = 0; i < 32; i++){
                regi[rd].binary_code[i] = (regi[rs].binary_code[i] || regi[rt].binary_code[i]);
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 0){
            //  sll
            for(int i = 0; i < 32 - sa; i++){
                regi[rd].binary_code[i] = regi[rt].binary_code[sa + i];
            }
            for(int i = 32 - sa; i < 32; i++){
                regi[rd].binary_code[i] = 0;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 4){
            //  sllv
            int shift = regi[rs].binary_code[27]*16 + regi[rs].binary_code[28]*8 + regi[rs].binary_code[29]*4 + regi[rs].binary_code[30]*2 +regi[rs].binary_code[31];
            for(int i = 0; i < 32 - shift; i++){
                regi[rd].binary_code[i] = regi[rt].binary_code[shift + i];
            }
            for(int i = 32 - shift; i < 32; i++){
                regi[rd].binary_code[i] = 0;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 42){
            //  slt -signed
            if(regi[rs].value < regi[rt].value){
                regi[rd].value = 1;
            }else{
                regi[rd].value = 0;
            }
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 43){
            //  sltu -unsigned
            int64_t rs_u;
            int64_t rt_u;
            if(regi[rs].value < 0){
                rs_u = regi[rs].value + (int64_t)pow(2, 32);
            }else{
                rs_u = regi[rs].value;
            }
            if(regi[rt].value < 0){
                rt_u = regi[rt].value + (int64_t)pow(2, 32);
            }else{
                rt_u = regi[rt].value;
            }
            if(rs_u < rt_u){
                regi[rd].value = 1;
            }else{
                regi[rd].value = 0;
            }
            //printf("rs_u %ld rt_u %ld\n", rs_u, rt_u);
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 3){
            //  sra - arithmetic
            int sign = regi[rt].binary_code[0];
            for(int i = 0; i < 32 - sa; i++){
                regi[rd].binary_code[sa + i] = regi[rt].binary_code[i];
            }
            for(int i = 0; i < sa; i++){
                //  sign extension
                regi[rd].binary_code[i] = sign;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 7){
            //  srav - arithmetic
            int sign = regi[rt].binary_code[0];
            int shift = regi[rs].binary_code[27]*16 + regi[rs].binary_code[28]*8 + regi[rs].binary_code[29]*4 + regi[rs].binary_code[30]*2 +regi[rs].binary_code[31];
            for(int i = 0; i < 32 - shift; i++){
                regi[rd].binary_code[shift + i] = regi[rt].binary_code[i];
            }
            for(int i = 0; i < shift; i++){
                //  sign extension
                regi[rd].binary_code[i] = sign;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 2){
            // srl
            for(int i = 0; i < 32 - sa; i++){
                regi[rd].binary_code[sa + i] = regi[rt].binary_code[i];
            }
            for(int i = 0; i < sa; i++){
                //  zero extension
                regi[rd].binary_code[i] = 0;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 6){
            // srlv
            int shift = regi[rs].binary_code[27]*16 + regi[rs].binary_code[28]*8 + regi[rs].binary_code[29]*4 + regi[rs].binary_code[30]*2 +regi[rs].binary_code[31];
            for(int i = 0; i < 32 - shift; i++){
                regi[rd].binary_code[shift + i] = regi[rt].binary_code[i];
            }
            for(int i = 0; i < shift; i++){
                //  zero extension
                regi[rd].binary_code[i] = 0;
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 34 || func == 35){
            //  sub, subu
            regi[rd].value = regi[rs].value - regi[rt].value;
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 38){
            //  xor
            for(int i = 0; i < 32; i++){
                regi[rd].binary_code[i] = regi[rs].binary_code[i] ^ regi[rt].binary_code[i];
            }
            regi[rd].value = bintodec(regi[rd].binary_code);
        }else if(func == 26){
            //  div
            regi[32].value = regi[rs].value / regi[rt].value;
            regi[33].value = regi[rs].value % regi[rt].value;

            dectobin(regi[32].value, regi[32].binary_code);
            dectobin(regi[33].value, regi[33].binary_code);
        }else if(func == 27){
            // divu
            int64_t rs_u;
            int64_t rt_u;
            if(regi[rs].value < 0){
                rs_u = regi[rs].value + (int64_t)pow(2, 32);
            }else{
                rs_u = regi[rs].value;
            }
            if(regi[rt].value < 0){
                rt_u = regi[rt].value + (int64_t)pow(2, 32);
            }else{
                rt_u = regi[rt].value;
            }
            regi[32].value = rs_u / rt_u;
            regi[33].value = rs_u % rt_u;
            dectobin(regi[32].value, regi[32].binary_code);
            dectobin(regi[33].value, regi[33].binary_code);
        }else if(func == 16){
            // mfhi
            regi[rd].value = regi[33].value;
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 18){
            // mflo
            regi[rd].value = regi[32].value;
            dectobin(regi[rd].value, regi[rd].binary_code);
        }else if(func == 17){
            // mthi
            regi[33].value = regi[rs].value;
            dectobin(regi[33].value, regi[33].binary_code);
        }else if(func == 19){
            // mtlo
            regi[32].value = regi[rs].value;
            dectobin(regi[32].value, regi[32].binary_code);
        }else if(func == 24){
            // mult
            int bin_64[64];
            int64_t mult_result = ((int64_t)regi[rs].value * (int64_t)regi[rt].value);
            dectobin_64(mult_result, bin_64);
            for(int i = 0; i < 32; i++){
                regi[32].binary_code[i] = bin_64[32 + i];
                regi[33].binary_code[i] = bin_64[i];
            }
            regi[32].value = bintodec(regi[32].binary_code);
            regi[33].value = bintodec(regi[33].binary_code);
        }else if(func == 25){
            // multu
            int64_t rs_u;
            int64_t rt_u;
            if(regi[rs].value < 0){
                rs_u = regi[rs].value + (int64_t)pow(2, 32);
            }else{
                rs_u = regi[rs].value;
            }
            if(regi[rt].value < 0){
                rt_u = regi[rt].value + (int64_t)pow(2, 32);
            }else{
                rt_u = regi[rt].value;
            }

            int bin_64[64];
            int64_t mult_result = rs_u * rt_u;
            dectobin_64(mult_result, bin_64);
            for(int i = 0; i < 32; i++){
                regi[32].binary_code[i] = bin_64[32 + i];
                regi[33].binary_code[i] = bin_64[i];
            }
            regi[32].value = bintodec(regi[32].binary_code);
            regi[33].value = bintodec(regi[33].binary_code);

        }else if(func == 9){
            // jalr
            regi[rd].value = *pc + 4;
            *pc = regi[rs].value;
            if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
                err_case = 1;
            }else{
                err_case = 4;
            }
            dectobin(regi[rd].value, regi[rd].binary_code);

        }else if(func == 8){
            // jr
            *pc = regi[rs].value;
            if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
                err_case = 1;
            }else{
                err_case = 4;
            }
        }else if(func == 12){
            // syscall
            int v0 = regi[2].value;
            int a0;
            if(v0 == 1){
               // printf("syscall ");
                printf("%d", regi[4].value);
                //printf("\n");
            }else if(v0 == 4){
                a0 = regi[4].value - 268435456;
                int address = a0 / 4;
                int hex_index = a0 %4;
                int c = 1;

                while(1){
                    c = hextodec(mem[address].hex_code[2*hex_index]) * 16 + hextodec(mem[address].hex_code[2*hex_index + 1]);
                    //printf("hex code %c %c \n", mem[address].hex_code[2*hex_index], mem[address].hex_code[2*hex_index + 1]);
                    if(c == 0){
                        break;
                    }
                    printf("%c", c);
                    //putchar(c);
                    hex_index++;
                    if(hex_index % 4 == 0){
                        hex_index = 0;
                        address += 1;
                    }

                }
            }else if(v0 == 10){
                printf("EXIT syscall\n");
                err_case = 5;
            }else{
                printf("Invalid syscall\n");
                err_case = 5;
            }
        }
    }else if(opcode == 8 || opcode == 9){
        //  addi, addiu
        regi[rt].value = regi[rs].value + imm;
        dectobin(regi[rt].value, regi[rt].binary_code);
        //printf("addi, addiu %08x\n", regi[rt].value);
    }else if(opcode == 12){
        //  andi
        int imm_b[32];
        dectobin(imm, imm_b);
        //  zero expansion
        for(int i = 0; i <16; i++){
            imm_b[i] = 0;
        }
        for(int i = 0; i < 32; i++){
            regi[rt].binary_code[i] = regi[rs].binary_code[i] && imm_b[i];
        }
        regi[rt].value = bintodec(regi[rt].binary_code);
    }else if(opcode == 15){
        //  lui
        int imm_b[32];
        dectobin(imm, imm_b);
        for(int i = 0; i < 16; i++){
            regi[rt].binary_code[16 + i] = 0;
            regi[rt].binary_code[i] = imm_b[16 + i];
        }

        regi[rt].value = bintodec(regi[rt].binary_code);
        //printf("lui %08x\n", regi[rt].value);
    }else if(opcode == 13){
        //  ori
        int imm_b[32];
        dectobin(imm, imm_b);
        //  zero extension
        for(int i = 0; i < 16; i++){
            imm_b[i] = 0;
        }
        for(int i = 0; i < 32; i++){
            regi[rt].binary_code[i] = regi[rs].binary_code[i] | imm_b[i];
        }
        regi[rt].value = bintodec(regi[rt].binary_code);
    }else if(opcode == 10){
        //  slti
        if(regi[rs].value < imm){
            regi[rt].value = 1;
        }else{
            regi[rt].value = 0;
        }
        dectobin(regi[rt].value, regi[rt].binary_code);
    }else if(opcode == 11){
        //  sltiu
        int64_t imm_u = imm;
        if(imm < 0){
            //  signed extension then change to unsigned
            imm_u = imm + (int64_t)pow(2, 32);
        }
        if(regi[rs].value < imm_u){
            regi[rt].value = 1;
        }else{
            regi[rt].value = 0;
        }
        dectobin(regi[rt].value, regi[rt].binary_code);
    }else if(opcode == 14){
        //  xori
        int imm_b[32];
        dectobin(imm, imm_b);
        for(int i = 0; i < 16; i++){
            // zero extension
            imm_b[i] = 0;
        }
        for(int i = 0; i < 32; i++){
            regi[rt].binary_code[i] = regi[rs].binary_code[i] ^ imm_b[i];
        }
        regi[rt].value = bintodec(regi[rt].binary_code);
    }else if(opcode == 35){
        // lw
        int address = regi[rs].value - 268435456 + imm;
        //printf("opcode %d address %d\n", opcode, address);
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
            //printf("lw ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else if(address % 4 != 0){
            err_case = 2;
            //printf("lw ");
            printf("Memory address alignment error: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = address / 4;
            for(int i = 0; i < 32; i++){
                regi[rt].binary_code[i] = mem[address].binary_code[i];
            }
            regi[rt].value = bintodec(regi[rt].binary_code);
        }
    }else if(opcode == 33){
        // lh
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
            //printf("lh ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else if(address % 2 != 0){
            err_case = 2;
            //printf("lh ");
            printf("Memory address alignment error: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = address / 4;
            int sign = mem[address].binary_code[0];
            for(int i = 0; i < 16; i++){
                regi[rt].binary_code[i] = sign;
                regi[rt].binary_code[i + 16] = mem[address].binary_code[i];
            }
            regi[rt].value = bintodec(regi[rt].binary_code);
        }
    }else if(opcode == 37){
        // lhu
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
            //printf("lhu ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else if(address % 2 != 0){
            err_case = 2;
            //printf("lhu ");
            printf("Memory address alignment error: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = address / 4;
            for(int i = 0; i < 16; i++){
                regi[rt].binary_code[i] = 0;
                regi[rt].binary_code[i + 16] = mem[address].binary_code[i];
            }
            regi[rt].value = bintodec(regi[rt].binary_code);
        }
    }else if(opcode == 32){
        // lb
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
           // printf("lb ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else{
            int sign = mem[address].binary_code[0];
            address = regi[rs].value - 268435456 + (imm / 4);
            int bin_index = (imm % 4)* 8;
            for(int i = 0; i < 24; i++){
                regi[rt].binary_code[i] = sign;
            }
            for(int i = 0; i < 8; i++){
                regi[rt].binary_code[24 + i] = mem[address].binary_code[bin_index + 8];
            }
            regi[rt].value = bintodec(regi[rt].binary_code);
        }
    }else if(opcode == 36){
        // lbu
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
           // printf("lbu ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = (regi[rs].value - 268435456 + imm / 4);
            int bin_index = (imm % 4)* 8;
           // printf("lbu address : %d, bin_index %d mem %08x\n", address, bin_index, mem[address].value);
            for(int i = 0; i < 24; i++){
                regi[rt].binary_code[i] = 0;
            }
            for(int i = 0; i < 8; i++){
                regi[rt].binary_code[24 + i] = mem[address].binary_code[bin_index + i];
            }
            regi[rt].value = bintodec(regi[rt].binary_code);
        }
    }else if(opcode == 43){
        // sw
        int address = (regi[rs].value - 268435456 + imm);
        //printf("address %d\n", address);
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
            //printf("sw ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else if(address % 4 != 0){
            err_case = 2;
           // printf("sw ");
            printf("Memory address alignment error: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = address / 4;
            mem[address].value = regi[rt].value;
            dectobin(mem[address].value, mem[address].binary_code);
           // printf("address %d value %08x\n", address, mem[address].value);
        }
    }else if(opcode == 41){
        // sh
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
           // printf("sh ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else if(address % 2 != 0){
            err_case = 2;
           // printf("sh ");
            printf("Memory address alignment error: 0x%08x\n", regi[rs].value + imm);
        }else{
            for(int i = 0; i < 16; i++){
                mem[address].binary_code[i + 16] = regi[rt].binary_code[i + 16];
            }
            mem[address].value = bintodec(mem[address].binary_code);
          //  printf("address %d value %08x\n", address, mem[address].value);
        }
    }else if(opcode == 40){
        // sb
        int address = regi[rs].value - 268435456 + imm;
        if((address/4 < 0) || (address/4 > 16384)){
            err_case = 1;
          //  printf("sb ");
            printf("Memory address out of range: 0x%08x\n", regi[rs].value + imm);
        }else{
            address = regi[rs].value - 268435456 + (imm / 4);
            int bin_index = (imm % 4)* 8;
            for(int i = 0; i < 8; i++){
                mem[address].binary_code[bin_index + i] = regi[rt].binary_code[24 + i];
            }
            mem[address].value = bintodec(mem[address].binary_code);
         //   printf("address %d value %08x\n", address, mem[address].value);
        }
    }else if(opcode == 2){
        // j
        *pc = (imm * 4);
       // printf("j imm %d PC :%d\n", imm, *pc);
        if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
            err_case = 1;
        }else{
            err_case = 4;
        }
    }else if(opcode == 3){
        // jal
        regi[31].value = *pc +4;
        *pc = (imm * 4);
        if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
            err_case = 1;
        }else{
            err_case = 4;
        }
        dectobin(regi[31].value, regi[31].binary_code);
    }else if(opcode == 4){
        // beq
        if(regi[rs].value == regi[rt].value){
            *pc = *pc + (imm * 4);
            if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
                err_case = 1;
            }
        }
    }else if(opcode == 5){
        // bne
        if(regi[rs].value != regi[rt].value){
            *pc = *pc + (imm * 4);
            if(((*pc / 4) < 0) ||((*pc / 4) > 16384)){
                err_case = 1;
            }
        }
    }
    return err_case;
}

int hextodec(char hex){
    int dec;
    if(hex == '0'){
        dec = 0;
    }else if(hex == '0'){
        dec = 0;
    }else if(hex == '1'){
        dec = 1;
    }else if(hex == '2'){
        dec = 2;
    }else if(hex == '3'){
        dec = 3;
    }else if(hex == '4'){
        dec = 4;
    }else if(hex == '5'){
        dec = 5;
    }else if(hex == '6'){
        dec = 6;
    }else if(hex == '7'){
        dec = 7;
    }else if(hex == '8'){
        dec = 8;
    }else if(hex == '9'){
        dec = 9;
    }else if(hex == 'a'){
        dec = 10;
    }else if(hex == 'b'){
        dec = 11;
    }else if(hex == 'c'){
        dec = 12;
    }else if(hex == 'd'){
        dec = 13;
    }else if(hex == 'e'){
        dec = 14;
    }else if(hex == 'f'){
        dec = 15;
    }
    return dec;
}

void dectobin(int decimal, int bin[32]){
    int remain[32];
    //  2's complement
    if(decimal < 0){
        decimal = decimal + (int)pow(2, 32) + 1;
        remain[31] = 1;
    }else{
        remain[31] = 0;
    }
    //  initialize
    for(int i = 0; i < 31; i++){
        bin[i] = 0;
        remain[i] = 0;
    }
    int index = 0;
    while(1){
        remain[index] = decimal % 2;
        decimal = decimal / 2;
        index++;

        if(decimal == 0)
            break;
    }
    for(int i = 0; i < 32; i++){
        bin[31 - i] = remain[i];
    }
}

void dectobin_64(int64_t decimal, int bin[64]){
    int remain[64];

    if(decimal < 0){
        decimal = decimal + (int64_t)pow(2, 64) + 1;
        remain[63] = 1;
    }else{
        remain[63] = 0;
    }

    for(int i = 0; i < 63; i++){
        bin[i] = 0;
        remain[i] = 0;
    }
    int index = 0;
    while(1){
        remain[index] = decimal % 2;
        decimal = decimal / 2;
        index++;

        if(decimal == 0)
            break;
    }
    for(int i = 0; i < 64; i++){
        bin[63 - i] = remain[i];
    }
}

// signed decimal
int bintodec(int bin[32]){
    int decimal = 0;
    for(int i = 0; i < 32; i++){
        decimal += bin[i] * (int)pow(2, 31 - i);
    }
    return decimal;
}

void print_regi(Regi regi[32], int pc){
    for(int i = 0; i < 32; i++){
        printf("$%d: 0x%08x\n", i, regi[i].value);
    }
    printf("LO: 0x%08x\n", regi[32].value);
    printf("HI: 0x%08x\n", regi[33].value);
    printf("PC: 0x%08x\n", pc);
}
