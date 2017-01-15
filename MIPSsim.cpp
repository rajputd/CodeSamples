#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>

using namespace std;

enum category {
    one,
    two,
    three,
    invalid,
};

enum instruction {
    //category 1 instructions
    NOP,
    J,
    BEQ,
    BNE,
    BGTZ,
    SW,
    LW,
    BREAK,

    //category 2 instructions
    XOR,
    MUL,
    ADD,
    SUB,
    AND,
    OR,
    ADDU,
    SUBU,

    //category 3 instructions
    ORI,
    XORI,
    ADDI,
    SUBI,
    ANDI,
    SRL,
    SRA,
    SLL,

    //invalid instruction
    NOTFOUND,
};

string decode(string instruction);
instruction findInstruction(string identifier);
string shiftleftbytwo(string number);
string twoctob(string number);
void disassemble(char* filename);
void simulate(char* filename);

int main(int argc, char* argv[]) {

    if(argc == 0){
        cout << "No input arguement\n";
        return 1;
    }

    disassemble(argv[1]);
    simulate(argv[1]);

    return 0;
}


//determines category of instruction
string decode(string input) {

    //determine instruction
    instruction command = findInstruction(input.substr(0,6));

    //get function arguments
    string arguments = input.substr(6,26);

    //output string
    string i;

    switch(command) {
        case NOP:
            {
                //get value of remaining bits
                string value = arguments.substr(0,26);
                long left =  strtol(value.c_str(), NULL, 2);

                //verify that value is correct
                if(left == 0) {
                    i = "NOP";
                } else {
                    i = "invalid instruction";
                }

                break;
            }

        case J:
            {

                //shift argument left 2 bits
                arguments = shiftleftbytwo(arguments);

                //get instr_index
                string value = arguments.substr(0,26);
                long instr_index =  strtol(value.c_str(), NULL, 2);

                //construct command
                char buffer[11];
                sprintf(buffer, "J #%li", instr_index);
                i = buffer;

                break;
            }

        case BEQ:
        case BNE:
            {
                //get arguments
                string rs = arguments.substr(0,5);
                string rt = arguments.substr(5,5);
                string offset = arguments.substr(10,16);

                //convert offset from two's compliment to normal binary if negative
                if(offset[0] == '1') {
                    offset = twoctob(offset);
                }

                //convert arguments to numbers
                long source =  strtol(rs.c_str(), NULL, 2);
                long compare = strtol(rt.c_str(), NULL, 2);
                long increment = strtol(offset.c_str(), NULL, 2);

                //construct command
                char buffer[20];
                if(command == BEQ){
                    sprintf(buffer, "BEQ R%li, R%li, #%li", source, compare, increment);
                } else {
                    sprintf(buffer, "BNE R%li, R%li, #%li", source, compare, increment);
                }

                i = buffer;

                break;
            }

        case BGTZ:
            {
                //get arguments
                string rs = arguments.substr(0,5);
                string offset = arguments.substr(10,16);

                //convert offset from two's compliment to normal binary if negative
                if(offset[0] == '1') {
                    offset = twoctob(offset);
                }

                //convert arguments to numbers
                long source =  strtol(rs.c_str(), NULL, 2);
                long increment = strtol(offset.c_str(), NULL, 2);

                //construct command
                char buffer[16];
                sprintf(buffer, "BGTZ R%li, #%li", source, increment);
                i = buffer;

                break;
            }

        case SW:
        case LW:
            {

                //get arguments
                string base = arguments.substr(0,5);
                string rt = arguments.substr(5,5);
                string offset = arguments.substr(10,16);

                //convert offset from two's compliment to normal binary if negative
                if(offset[0] == '1') {
                    offset = twoctob(offset);
                }

                //convert arguments to numbers
                long start =  strtol(base.c_str(), NULL, 2);
                long reg = strtol(rt.c_str(), NULL, 2);
                long increment = strtol(offset.c_str(), NULL, 2);

                //construct command
                char buffer[18];
                if(command == SW) {
                    sprintf(buffer, "SW R%li, %li(R%li)", reg, increment, start);
                } else {
                    sprintf(buffer, "LW R%li, %li(R%li)", reg, increment, start);
                }

                i = buffer;

                break;
            }

        case BREAK: i = "BREAK"; break;

        case XOR:
        case MUL:
        case ADD:
        case SUB:
        case AND:
        case OR:
        case ADDU:
        case SUBU:
            {
                //get arguments
                string dest = arguments.substr(0,5);
                string src1 = arguments.substr(5,5);
                string src2 = arguments.substr(10,5);

                //convert arguments into numbers
                long destination =  strtol(dest.c_str(), NULL, 2);
                long source1 = strtol(src1.c_str(), NULL, 2);
                long source2 = strtol(src2.c_str(), NULL, 2);

                //format appropriate output string
                char buffer[18];
                switch(command) {
                    case XOR: sprintf(buffer, "XOR R%li, R%li, R%li", destination, source1, source2); break;
                    case MUL: sprintf(buffer, "MUL R%li, R%li, R%li", destination, source1, source2); break;
                    case ADD: sprintf(buffer, "ADD R%li, R%li, R%li", destination, source1, source2); break;
                    case SUB: sprintf(buffer, "SUB R%li, R%li, R%li", destination, source1, source2); break;
                    case AND: sprintf(buffer, "AND R%li, R%li, R%li", destination, source1, source2); break;
                    case OR:  sprintf(buffer, "OR R%li, R%li, R%li", destination, source1, source2); break;
                    case ADDU: sprintf(buffer, "ADDU R%li, R%li, R%li", destination, source1, source2); break;
                    case SUBU: sprintf(buffer, "SUBU R%li, R%li, R%li", destination, source1, source2); break;
                }

                i = buffer;

                break;
            };


        case ORI:
        case XORI:
        case ADDI:
        case SUBI:
        case ANDI:
            {
                //get arguments
                string dest = arguments.substr(0,5);
                string src1 = arguments.substr(5,5);
                string immediate_value = arguments.substr(10,16);

                //convert offset from two's compliment to normal binary if negative
                if(command == ADDI || command == SUBI) {
                    if(immediate_value[0] == '1') {
                    immediate_value = twoctob(immediate_value);
                    }
                }

                //convert arguments into numbers
                long destination =  strtol(dest.c_str(), NULL, 2);
                long source1 = strtol(src1.c_str(), NULL, 2);
                long immediate = strtol(immediate_value.c_str(), NULL, 2);

                //assemble instruction
                char buffer[21];
                switch(command) {
                    case ORI: sprintf(buffer, "ORI R%li, R%li, #%li", destination, source1, immediate); break;
                    case XORI: sprintf(buffer, "XORI R%li, R%li, #%li", destination, source1, immediate); break;
                    case ADDI: sprintf(buffer, "ADDI R%li, R%li, #%li", destination, source1, immediate); break;
                    case SUBI: sprintf(buffer, "SUBI R%li, R%li, #%li", destination, source1, immediate); break;
                    case ANDI: sprintf(buffer, "ANDI R%li, R%li, #%li", destination, source1, immediate); break;
                }

                i = buffer; break;
            }

        case SRL:
        case SRA:
        case SLL:
             {
                //get arguments
                string dest = arguments.substr(0,5);
                string src1 = arguments.substr(5,5);
                string sa = arguments.substr(21,5);

                //convert arguments into numbers
                long destination =  strtol(dest.c_str(), NULL, 2);
                long source1 = strtol(src1.c_str(), NULL, 2);
                long shift_amount = strtol(sa.c_str(), NULL, 2);

                //assemble instruction
                char buffer[17];
                switch(command) {
                    case SRL: sprintf(buffer, "SRL R%li, R%li, #%li", destination, source1, shift_amount); break;
                    case SRA: sprintf(buffer, "SRA R%li, R%li, #%li", destination, source1, shift_amount); break;
                    case SLL: sprintf(buffer, "SLL R%li, R%li, #%li", destination, source1, shift_amount); break;
                }

                i = buffer;

                break;

            }

        case NOTFOUND: i = "NOTFOUND"; break;
    }

    return i;
}

instruction findInstruction(string identifier) {

    //get category code number
    const char* catCode = identifier.substr(0,3).c_str();
    long catNum = strtol(catCode, NULL, 2);
    category c;

    //determine category
    switch(catNum) {
        case 1: c = one; break;
        case 2: c = two; break;
        case 4: c = three; break;
        default: c = invalid; break;
    }

    //determine opcode value
    const char* opcode = identifier.substr(3,3).c_str();
    long opvalue = strtol(opcode, NULL, 2);

    switch(c){
        case one:
            switch(opvalue) {
                case 0: return NOP;
                case 1: return J;
                case 2: return BEQ;
                case 3: return BNE;
                case 4: return BGTZ;
                case 5: return SW;
                case 6: return LW;
                case 7: return BREAK;
                default: return NOTFOUND;
            }
            break;
        case two:
            switch(opvalue) {
                case 0: return XOR;
                case 1: return MUL;
                case 2: return ADD;
                case 3: return SUB;
                case 4: return AND;
                case 5: return OR;
                case 6: return ADDU;
                case 7: return SUBU;
                default: return NOTFOUND;
            }
            break;
        case three:
            switch(opvalue) {
                case 0: return ORI;
                case 1: return XORI;
                case 2: return ADDI;
                case 3: return SUBI;
                case 4: return ANDI;
                case 5: return SRL;
                case 6: return SRA;
                case 7: return SLL;
                default: return NOTFOUND;
            }
            break;

        case invalid: return NOTFOUND;
    }

}

string shiftleftbytwo(string number) {
    int size = number.size();
    string output = number;

    for(int j = 0; j < size; j++) {
        if(j + 2 < size) {
            output[j] = output[j + 2];
        } else {
            output[j] = '0';
        }
    }

    return output;
}

string twoctob(string number) {
    string output = number;
    int size = output.size();

    bool negative;

    //check if negative
    if(output[0] == '1') {
        negative = true;
    } else {
        negative = false;
    }

    //invert all bits
    for(int i = 0; i < size; i++) {
        if(output[i] == '0') {
            output[i] = '1';
        } else  {
            output[i] = '0';
        }
    }

    //add one
    for(int i = size - 1; i >= 0; i--) {
        if(output[i] == '1') {
            output[i] = '0';
        } else {
            output[i] = '1';
            break;
        }
    }

    //add negative sign if negative
    if(negative) {
        return "-" + output;
    } else {
        return output;
    }
}

void disassemble(char* filename) {

    //open files
    FILE* input = fopen(filename, "r");
    FILE* output = fopen("disassembly.txt", "w");

    if(input == NULL){
        cout << "could not open file\n";
        return;
    }

    if(output == NULL){
        cout << "could not create file\n";
        return;
    }

    int start = 64;
    char address[16];
    sprintf(address, "%i", start);
    bool breakset = false;
    char buffer[32];

    while(fscanf(input, "%s", buffer) != EOF) {

        //write binary and address to file
        fputs(buffer, output);
        fputs("\t", output);
        fputs(address, output);
        fputs("\t", output);

        //check if we are done reading instructions
        if(breakset) {
            //start interpreting binaries as data, not commands
            string number = buffer;

            //convert negative number to normal binary
            if(buffer[0] == '1') {
                number = twoctob(buffer);
            }

            //convert number to decimal
            long value = strtol(number.c_str(), NULL, 2);
            char num[11];
            sprintf(num, "%li", value);
            fputs(num, output);

        } else {
            //decode and write command
            string inst = decode(buffer);

            if(inst.compare("BREAK") == 0) {
                breakset = true;
            }

            fputs(inst.c_str(), output);
        }

        fputs("\n", output);

        //increment address
        start = start + 4;
        sprintf(address, "%i", start);
    }

    //close files
    fclose(input);
    fclose(output);
}

void simulate(char* filename) {

    //open files
    FILE* input = fopen(filename, "r");
    FILE* output = fopen("simulation.txt", "w");

    if(input == NULL){
        cout << "could not open file\n";
        return;
    }

    if(output == NULL){
        cout << "could not create file\n";
        return;
    }

    //create memory for program
    vector<string> prog_mem;
    vector<long> data;
    long registers[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    bool breakset = false;
    int pc = 64;
    int dataStart;

    //read and load data into appropriate memory location
    char readbuffer[32];
    while(fscanf(input, "%s", readbuffer) != EOF) {
        //if breakset == true, load binaries into data, not prog_mem
        if(breakset) {
            string number = readbuffer;

            //convert negative number to normal binary
            if(readbuffer[0] == '1') {
                number = twoctob(readbuffer);
            }

            //convert number to decimal
            long value = strtol(number.c_str(), NULL, 2);
            data.push_back(value);
        } else {

            //push instruction to program memory
            prog_mem.push_back(readbuffer);

            //if BREAK instruction is read
            if(findInstruction(readbuffer) == BREAK) {
                breakset = true;
                dataStart = pc + 4;
            }
        }

        pc += 4;

    }


    //reset pc
    pc = 64;

    //prepare to write output
    int cycle = 1;
    string writebuffer;
    bool branch = false;
    bool jump = false;
    int jump_address;

    do {
        //fetch instruction
        writebuffer = prog_mem[(pc / 4) - 16];
        instruction i = findInstruction(writebuffer.substr(0,6));

        //decode instruction
        string instr = decode(writebuffer);
        stringstream ss;
        ss.str(instr);

        //execute instruction
        switch(i) {
        case NOP: break;
        case J:
            {
                jump = true;
                ss.ignore(3,'#') >> jump_address;

                break;
            }
        case BEQ:
        case BNE:
            {
                int rs, rt;

                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), '#') >> jump_address;

                jump_address *= 4; //shift address left by 2

                if(i == BEQ) {
                    if(registers[rs] == registers[rt]) {
                            branch = true;
                        } else {
                            branch = false;
                        }
                } else if (i == BNE){
                    if(registers[rs] != registers[rt]) {
                             branch = true;
                        } else {
                            branch = false;
                        }
                }
                break;
            }
        case BGTZ:
            {
                int rs;

                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), '#') >> jump_address;

                jump_address *= 4;

                if(registers[rs] > 0) {
                    branch = true;
                } else {
                    branch = false;
                }

                break;
            }

        case SW:
        case LW:
            {
                int rt, base, offset;

                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), ' ') >> offset;
                ss.ignore(ss.str().size(), 'R') >> base;

                if(i == SW) {
                    data[((registers[base] + offset) - dataStart) / 4] = registers[rt];
                } else if (i == LW) {
                    registers[rt] = data[((registers[base] + offset) - dataStart) / 4];
                }

                break;
            }

        case BREAK:  break;

        case XOR:
        case OR:
            {
                int rd, rs, rt;

                ss.ignore(3);
                ss.ignore(ss.str().size(), 'R') >> rd;
                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), 'R') >> rt;

                if(i == XOR) {
                    registers[rd] = registers[rs] ^ registers[rt];
                } else if (i == OR) {
                    registers[rd] = registers[rs] | registers[rt];
                }

                break;
            }
        case MUL:
        case ADD:
        case SUB:
        case AND:
        case ADDU:
        case SUBU:
            {

                int rd, rs, rt;

                ss.ignore(ss.str().size(), 'R') >> rd;
                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), 'R') >> rt;

                //may need to potentially change ADDU and SUBU
                switch(i) {
                    case MUL: registers[rd] = registers[rs] * registers[rt]; break;
                    case ADD: registers[rd] = registers[rs] + registers[rt]; break;
                    case SUB: registers[rd] = registers[rs] - registers[rt]; break;
                    case AND: registers[rd] = registers[rs] & registers[rt]; break;
                    case ADDU: registers[rd] = registers[rs] + registers[rt]; break;
                    case SUBU: registers[rd] = registers[rs] - registers[rt]; break;
                }
                break;
            }
        case ORI:
        case XORI:
            {
                int rs, rt, immediate;
                char dummy;

                ss.ignore(ss.str().size(), 'R') >> dummy;
                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), '#') >> immediate;

                if( i == ORI) {
                    registers[rt] = registers[rs] | immediate;
                } else if (i == XORI) {
                    registers[rt] = registers[rs] ^ immediate;
                }

                break;
            }
        case ADDI:
        case SUBI:
        case ANDI:
            {
                int rs, rt, immediate;

                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), 'R') >> rs;
                ss.ignore(ss.str().size(), '#') >> immediate;

                switch(i) {
                    case ADDI: registers[rt] = registers[rs] + immediate; break;
                    case SUBI: registers[rt] = registers[rs] - immediate; break;
                    case ANDI: registers[rt] = registers[rs] & immediate; break;
                }

                break;
            }
        case SRL:
            {
                int rd, rt, sa;
                char dummy;

                ss.ignore(ss.str().size(), 'R') >> dummy;
                ss.ignore(ss.str().size(), 'R') >> rd;
                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), '#') >> sa;

                //need to used an unsigned into to make shift operator logical
                unsigned int dum = registers[rt];
                registers[rd] = dum >> sa;

                break;
            }
        case SLL:
            {

                int rd, rt, sa;

                ss.ignore(ss.str().size(), 'R') >> rd;
                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), '#') >> sa;

                registers[rd] = registers[rt] << sa;

                break;
            }
        case SRA:
            {
                int rd, rt, sa;
                char dummy;

                ss.ignore(ss.str().size(), 'R') >> dummy;
                ss.ignore(ss.str().size(), 'R') >> rd;
                ss.ignore(ss.str().size(), 'R') >> rt;
                ss.ignore(ss.str().size(), '#') >> sa;

                registers[rd] = registers[rt] >> sa;
                break;
            }
        case NOTFOUND: cout << "NOTFOUND"; break;
    }

        //create string for cycle and pc numbers
        char iteration[25];
        sprintf(iteration, "Cycle %i:\t%i\t", cycle, pc);

        //write cycles, address, and instruction
        fputs("--------------------\n", output);
        fputs(iteration, output);
        fputs(instr.c_str(), output);


        fputs("\n\nRegisters\n",output);

        //write all register values
        char reg[90];
        for(int i = 0; i < 32; i += 8) {
            sprintf(reg, "R%02i:\t%li\t%li\t%li\t%li\t%li\t%li\t%li\t%li\n", i, registers[i],
             registers[i + 1], registers[i + 2], registers[i + 3], registers[i + 4],
             registers[i + 5], registers[i + 6], registers[i + 7]);
             fputs(reg, output);
        }

        fputs("\nData", output);

        //write all data values
        for(int i = 0; i < data.size(); i++) {
            //if 32 words have passed
            if(i % 8 == 0) {
                sprintf(iteration, "\n%i:\t", dataStart + (i * 4));
                fputs(iteration, output);
            }

            if(i % 8 == 7) {
                sprintf(iteration, "%li", data[i]);
            } else {
                sprintf(iteration, "%li\t", data[i]);
            }

            fputs(iteration,output);

        }

        fputs("\n\n", output);

        //adjust pc to appropriate value
        if(jump) {
            pc = jump_address;
            jump = false;
        } else {

            if(branch) {
                pc += jump_address;
                branch = false;
            }

            pc += 4;
        }

        cycle++;


    } while (findInstruction(writebuffer.substr(0,6)) != BREAK);

    //close files
    fclose(input);
    fclose(output);


}
