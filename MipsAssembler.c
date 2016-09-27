#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int getImmediateBNE(int labelAddress, int pc){
	int immediate = ((labelAddress - pc)/4);
};


//converts register into a decimal number
int registerToDecimal( const char * mipsRegister){
	char registerType = mipsRegister[1];
	char registerNum = mipsRegister[2];
	char* t = "t";
	char* s = "s";
	char* zero = "0";
	int decimal;
	int charNum;
	switch(registerNum){
		case '0':
			charNum = 0;
			break;
		case '1':
			charNum =1;
			break;
		case '2':
			charNum =2;
			break;
		case '3':
			charNum =3;
			break;
		case '4':
			charNum =4;
			break;
		case '5':
			charNum = 5;
			break;
		case '6':
			charNum =6;
			break;
		case '7':
			charNum =7;
			break;
		case '8':
			charNum =8;
			break;
		case '9':
			charNum = 9;
			break;
	}
	if(strstr(mipsRegister, "t") != NULL){
		decimal = charNum + 8;
		return decimal;
	}
	if(strstr(mipsRegister, "s") != NULL){
	        decimal = charNum + 16;
		return decimal;
	}

	if((strstr(mipsRegister, "0") != NULL) || strstr(mipsRegister, "zero") != NULL ){
        	return 0;
	}


}

int main() {


	char line[50];
	char lines[100][50];
	int instructionCount=0;

	//gets lines from stdin and store them in lines[]
	while(fgets(line, 50, stdin) != NULL){
		strcpy(lines[instructionCount], line);
		instructionCount++;
	}

	//struct to hold info about a line
	struct inSTRUCTion{
		char instruction[20];
		char operands[20];
		int address;
	};
	struct inSTRUCTion structArray[100];

	//holds info about a label
	struct label{
		char theLabel[20];
		int address;
	};
	struct label labelArray[100];

	//stores labels their addresses in an array of structs
	int t=0;
	int u=0;
	int labelCount=0;
	char thisLabel[20];
	while(t<instructionCount){
		if(strstr(lines[t], "la")!=NULL){
			u++;
		}
		if(strstr(lines[t], ":") != NULL){ 
			if(sscanf(lines[t], "%[^:]", thisLabel) == 1){
				strcpy(labelArray[labelCount].theLabel, thisLabel);
				labelArray[labelCount].address = (u*4);
				labelCount++;
			}
		}
		
		if(strstr(lines[t], ".word")!=NULL){
			u++;
			t++;
			continue;

		}
		if(strstr(lines[t], ".space")!=NULL){
			char temp[20];
			char temper[20];
			int space;
			if(strstr(lines[t], ":")!=NULL){
                        	sscanf(lines[t], "%[^:]: %s %d", temp, temper, &space);
				u = u + (space/4);
				t++; 
				continue;
			}else{
				sscanf(lines[t], "%s %d", temp, &space);
				u = u + (space/4);
				t++;
				continue;
                        
			}

		}
		
		char directive[50];
		sscanf(lines[t], "%s", directive);
		if(directive[0]!='.'){
			u++;
		}
		t++;
	}


	//this removes the .text and .data directives,
	//as well as the labels in front of lines
	t=0;
	int newInstructionCount=0;
	char newLines[100][50];
	while(t< instructionCount){
		sscanf(lines[t], "%[^\n]", line);
		//gets rid of directives
		if((strstr(lines[t], ".text") == NULL)&&(strstr(lines[t], ".data") == NULL)){
			//gets rid of labels
			char thisLine[20];
			if(strstr(lines[t], ":") != NULL){
				sscanf(line, "%*s %[^\n]",  thisLine);
				strcpy(newLines[newInstructionCount], thisLine);
				newInstructionCount++;
			}else{
				sscanf(line, " %[^\n]",  thisLine);
				strcpy(newLines[newInstructionCount], thisLine);
				newInstructionCount++;
			}
		}
		t++;
	}

	t=0;
	int addressCounter=0;
	int address;
	char instruction[10];
	char operands[20];

	//loads instructions, operands, and addresses into the array of structs
	while(t<newInstructionCount){
	
		sscanf(newLines[t], "%s %s", instruction, operands);
		strcpy(structArray[t].instruction, instruction);
		strcpy(structArray[t].operands, operands);
		structArray[t].address = (addressCounter*4);
		if(strcmp(instruction, ".word")==0){
			int space = 1;
			addressCounter = addressCounter+ space;
			t++;
			continue;
		}else if(strcmp(instruction, ".space")==0){
			int space = atoi(operands);
			addressCounter = addressCounter + (space/4);
			t++;
			continue;
		}
		
		if(strstr(structArray[t].instruction, "la") != NULL){
			addressCounter++;
		}
	
		
		addressCounter++;
		t++;
	}

	t=0;
	while(t<newInstructionCount){
		//isolates all instructionss and operands, performs necessary
		//operations on them, and prints the address and full instruction in hex
		if(strcmp(structArray[t].instruction, "addi") == 0){
			
			char rs[10];
			int  rsInt;
			char rt[10];
			int rtInt;
			int immediate;
			sscanf(structArray[t].operands, "%[^,],%[^,],%d", rt, rs, &immediate);
			rtInt = registerToDecimal(rt);
			rsInt = registerToDecimal(rs);
			printf("0x%08X: ", structArray[t].address);	
			int hexStuff = (8<<26) + (rsInt<<21)+(rtInt<<16) + immediate;
			printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "la")==0){
						
			char rx[10];
			int rxInt;
			char label[20];
			sscanf(structArray[t].operands, "%[^,],%s", rx, label);
			rxInt = registerToDecimal(rx);
			int labelAddress;
			int x=0;
			while(x<labelCount){
				if(strcmp(labelArray[x].theLabel, label)==0){
					labelAddress = labelArray[x].address;
					break;
				}
				x++;
			}
			unsigned int upper = labelAddress >> 16;
			unsigned int lower = labelAddress << 16;
			lower = lower >> 16;
			printf("0x%08X: ", structArray[t].address);
			int hexStuff = (15 << 26) + (rxInt << 16) + upper;
			printf("0x%08X\n", hexStuff);
			printf("0x%08X: ", structArray[t].address+4);
			hexStuff = (13 << 26) + (rxInt << 21) + (rxInt << 16) + lower;
			printf("0x%08X\n", hexStuff);
		        	
		}else if(strcmp(structArray[t].instruction, "add")==0){
			
			char rs[10];
			int rsInt;
			char rt[10];
			int rtInt;
			char rd[10];
			int rdInt;
			sscanf(structArray[t].operands, "%[^,],%[^,],%s", rd, rs, rt);
			rsInt = registerToDecimal(rs);
			rtInt = registerToDecimal(rt);
			rdInt = registerToDecimal(rd);
			printf("0x%08X: ", structArray[t].address);
			int hexStuff = (0 << 26 ) + (rsInt << 21 ) + (rtInt << 16) + (rdInt << 11) + (0 << 6) + 32;
			printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "lw")==0){
			
			char rt[10];
			int rtInt;
			char rs[10];
			int rsInt;
			int immediate;
			sscanf(structArray[t].operands, "%[^,],%d(%[^)]", rt, &immediate, rs);
			rtInt = registerToDecimal(rt);
			rsInt = registerToDecimal(rs);
			printf("0x%08X: ", structArray[t].address);
			immediate = immediate & 0x0000FFFF;
			int hexStuff = (35 << 26) + (rsInt << 21) + (rtInt << 16) + immediate;
			printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "nor")==0){
				
			char rs[10];
                        int rsInt;
                        char rt[10];
                        int rtInt;
                        char rd[10];
                        int rdInt;
                        sscanf(structArray[t].operands, "%[^,],%[^,],%s", rd, rs, rt);
                        rsInt = registerToDecimal(rs);
                        rtInt = registerToDecimal(rt);
                        rdInt = registerToDecimal(rd);
                        printf("0x%08X: ", structArray[t].address);
                        int hexStuff = (0 << 26 ) + (rsInt << 21 ) + (rtInt << 16) + (rdInt << 11) + (0 << 6) + 39;
                        printf("0x%08X\n", hexStuff);
				
		}else if(strcmp(structArray[t].instruction, "ori")==0){
					
 			char rt[10];
                        int rtInt;
                        char rs[10];
                        int rsInt;
                        int immediate;
                        sscanf(structArray[t].operands, "%[^,],%[^,],%d", rt, rs, &immediate);
                        rtInt = registerToDecimal(rt);
                        rsInt = registerToDecimal(rs);
                        printf("0x%08X: ", structArray[t].address);
                        immediate = immediate & 0x0000FFFF;
                        int hexStuff = (13 << 26) + (rsInt << 21) + (rtInt << 16) + immediate;
                        printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "sll")==0){
				
			char rd[10];
			int rdInt;
			char rt[10];
			int rtInt;
			int shamt;
			sscanf(structArray[t].operands, "%[^,],%[^,],%d", rd, rt, &shamt);
			rdInt = registerToDecimal(rd);
			rtInt = registerToDecimal(rt);
			printf("0x%08X: ", structArray[t].address);
			int hexStuff = (0 << 26) + (0 << 21 ) + (rtInt << 16) + (rdInt<< 11) + (shamt << 6) + 0;
			printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "lui")==0){
						
			char rt[10];
                        int rtInt;
                        int immediate;
                        sscanf(structArray[t].operands, "%[^,],%d", rt, &immediate);
                        rtInt = registerToDecimal(rt);
                        printf("0x%08X: ", structArray[t].address);
                        immediate = immediate & 0x0000FFFF;
                        int hexStuff = (15 << 26) + (0 << 21) + (rtInt << 16) + immediate;
                        printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "sw")==0){
			
			char rt[10];
                        int rtInt;
                        char rs[10];
                        int rsInt;
                        int immediate;
                        sscanf(structArray[t].operands, "%[^,],%d(%[^)]", rt, &immediate, rs);
                        rtInt = registerToDecimal(rt);
                        rsInt = registerToDecimal(rs);
                        printf("0x%08X: ", structArray[t].address);
                        immediate = immediate & 0x0000FFFF;
                        int hexStuff = (43 << 26) + (rsInt << 21) + (rtInt << 16) + immediate;
                        printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "bne")==0){
			
			char rs[10];
			int rsInt;
			char rt[10];
			int rtInt;
			char label[20];
			int immediate;
			int labelAddress;
			sscanf(structArray[t].operands, "%[^,],%[^,],%s", rs, rt, label);
			
			rtInt = registerToDecimal(rt);
                        rsInt = registerToDecimal(rs);

			int x=0;
                        while(x<labelCount){
                                if(strcmp(labelArray[x].theLabel, label)==0){
                                        labelAddress = labelArray[x].address;
                                        break;
                                }
                                x++;
                        }
			labelAddress = labelAddress;
			immediate = getImmediateBNE(labelAddress, structArray[t].address);
			printf("0x%08X: ", structArray[t].address);
			immediate = immediate & 0x0000FFFF;
			int hexStuff = (5 << 26) + (rsInt << 21) + ( rtInt << 16) + immediate;
			printf("0x%08X\n", hexStuff);
			
		}else if(strcmp(structArray[t].instruction, "j")==0){
			char label[20];
			int labelAddress;
			sscanf(structArray[t].operands, "%s", label);
			int x=0;
			while(x<labelCount){
                                if(strcmp(labelArray[x].theLabel, label)==0){
                                        labelAddress = labelArray[x].address;
                                        break;
                                }
                                x++;
                        }
			labelAddress = labelAddress >> 2;
			printf("0x%08X: ", structArray[t].address);
			int hexStuff = (2 << 26) + labelAddress;
			printf("0x%08X\n", hexStuff);
			

		}
		t++;
	}
	return(0);

}
