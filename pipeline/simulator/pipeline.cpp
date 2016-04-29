/***************************************************

	File: Pipeline.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include "GlobalVar.h"

using namespace std;

string Error_Message[4] = {": Write $0 Error", ": Address Overflow", ": Misalignment Error", ": Number Overflow"};

void InitialReg(){
	for(int i=0; i<32; i++)
		reg[i] = 0;
}

void cyclePrint(ofstream &fout, int &Cycle){
	fout << "cycle " << dec << Cycle++ << endl;
	for(int i=0; i<32; i++){
		fout << "$" << setw(2) << setfill('0') << dec << i;
		fout << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg[i] << endl;
	}
	fout << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << PC << endl << endl << endl;
}

int main(){
	char ch;
	int Word = 0, bytes = 4, Cycle = 0, idx = -2;
	int Address[1024];
	// Initialize register;
	InitialReg();

	ofstream fout("snapshot.rpt", ios::out);
	ofstream Errorout("error_dump.rpt", ios::out);

	// Read iimage.bin
	ifstream fin("iimage.bin", ios::in | ios::binary);
	if(!fin){
		cout << "Error to load 'iimage.bin'!\n";
		return 0;
	}
	while(!fin.eof()){
		fin.get(ch);
		Word = (Word << 8) | (unsigned char)ch;
		bytes--;
		if(bytes==0){
			bytes = 4;
			if(idx==-2){
				PC = Word;
				idx++;
				continue;
			}
			else if(idx==-1){
				idx++;
				continue;
			}
			Address[PC+idx*4] = Word;
			idx++;
		}
	}
	fin.close();

	// Read dimage.bin
	fin.open("dimage.bin", ios::in | ios::binary);
	// Read $sp
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	reg[29] = Word;
	// Numbers of words
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	int NumbersOfWords = Word;
	for(int i=0; i<NumbersOfWords*4; i++){
		fin.get(ch);
		Memory[i] = ch;
	}
	cyclePrint(fout, Cycle);
	Halt = false;

	//Start Instructions
	while(!Halt){
		for(int i=0; i<4; i++) error_type[i] = false;
		
		for(int i=0; i<4; i++){
			if(error_type[i]==true)
				Errorout << "In cycle " << Cycle << Error_Message[i] << endl;
		}
		if(Halt==true)
			break;
		cyclePrint(fout, Cycle);
	}
	fout.close();
	Errorout.close();
	return 0;
}