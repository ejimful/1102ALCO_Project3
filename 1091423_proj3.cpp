#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include <cmath>
#include<algorithm>
#include <cstdlib>
#include <iomanip>
using namespace std;

struct contentofRS//�C��RS�������e
{
	bool used;
	string operand;
	string rs1;
	string rs2;
};
struct inst//�C��instuction�����e
{
	string type;
	string rd;
	string rs1;
	string rs2;
	int imm;
};
struct buffer//buffer���e
{
	int cycle;
	int RS;
	bool empty; // 0 �N��Ū��A1 �N���F��
};
vector<inst> instruction; //�j�p��input.txt�`���
int RF[6] = { 0,0,2,4,6,8 }; //��l��RF�ARF[0]���|�Ψ�
string RAT[6];//RAT[0]���|�Ψ�
contentofRS RS[6];//RS[0]���|�Ψ�
int cycle = 1; //�ثe�i��쪺Cycle�s��
bool changedCycle; //�O�_���ܤ�
int C_ADDandSUB, C_MUL, C_DIV; //�|��type��ALU Cycle time
buffer bufferADD;
buffer bufferMUL;

void input()
{
	ifstream inFile("input.txt", ios::in);
	if (!inFile) {
		cout << "File could not be opened!\n";
		system("pause");
		exit(1);
	}
	stringstream ss;
	string buffer;
	while (getline(inFile, buffer)) {
		ss.str("");
		ss.clear();
		ss << buffer;

		inst insttemp;
		string temp;
		//�Ntype�g�J
		ss >> temp;
		insttemp.type = temp;
		//�Nrd�g�J
		ss >> temp;
		temp.pop_back();
		insttemp.rd = temp;
		//�Nrs1�g�J
		ss >> temp;
		temp.pop_back();
		insttemp.rs1 = temp;
		//�P�_type����ءA�Y��ADDI�N�Ntemp�୼int���A�A�g�Jimm�A��L�h�O�g�Jrs2
		ss >> temp;
		if (insttemp.type == "ADDI")
			insttemp.imm = stoi(temp);
		else
			insttemp.rs2 = temp;
		instruction.push_back(insttemp);//�N�������n��instuction push�i�h
	}
}
void Issue()
{
	if (!instruction.empty())//�P�_�٦��S��instruction����issue
	{
		if (instruction[0].type == "ADD" || instruction[0].type == "SUB" || instruction[0].type == "ADDI")
		{
			for (int i = 1; i <= 3; i++)//ADD��ALU
			{
				if (!RS[i].used)//�P�_RS[i]���S���Ŷ�
				{
					if (instruction[0].type == "SUB")
						RS[i].operand = "-";
					else
						RS[i].operand = "+";

					int rs1 = instruction[0].rs1[1] - '0'; //��X��instruction��rs1 register�s��
					if (RAT[rs1] == "")//RAT[rs1]����
						RS[i].rs1 = to_string(RF[rs1]);
					else//RAT[rs1]���F��
						RS[i].rs1 = RAT[rs1];

					if (instruction[0].type == "ADDI")//�Y�O"ADDI"�Aimm�������rs2
						RS[i].rs2 = to_string(instruction[0].imm);
					else
					{
						int rs2 = instruction[0].rs2[1] - '0';//��X��instruction��rs2 register�s��
						if (RAT[rs2] == "")//RAT[rs2]����
							RS[i].rs2 = to_string(RF[rs2]);
						else//RAT[rs2]���F��
							RS[i].rs2 = RAT[rs2];
					}
					int rd = instruction[0].rd[1] - '0';//��X��instruction��rd register�s��
					RAT[rd] = "RS" + to_string(i);// ��s RAT ����
					RS[i].used = 1;//�NRS[i]�]��"���b�ϥΤ�"
					changedCycle = 1;//��Cycle���ܤ�
					instruction.erase(instruction.begin()); //�Nissue�i�Ӫ�instruction�R��
					break;
				}
			}
		}
		else if (instruction[0].type == "DIV" || instruction[0].type == "MUL")
		{
			for (int i = 4; i <= 5; i++)//MUL��ALU
			{
				if (!RS[i].used)//�P�_RS[i]���S���Ŷ�
				{
					RS[i].used = 1;//�NRS[i]�]��"���b�ϥΤ�"
					if (instruction[0].type == "MUL")
						RS[i].operand = "*";
					else
						RS[i].operand = "/";

					int rs1 = instruction[0].rs1[1] - '0'; //��X��instruction��rs1 register�s��
					if (RAT[rs1] == "")//RAT[i]����
						RS[i].rs1 = to_string(RF[rs1]);
					else//RAT���F��
						RS[i].rs1 = RAT[rs1];

					int rs2 = instruction[0].rs2[1] - '0';//��X��instruction��rs2 register�s��
					if (RAT[rs2] == "")//RAT[i]����
						RS[i].rs2 = to_string(RF[rs2]);
					else//RAT���F��
						RS[i].rs2 = RAT[rs2];

					int rd = instruction[0].rd[1] - '0';//��X��instruction��rd register�s��
					RAT[rd] = "RS" + to_string(i);//��sRAT����
					changedCycle = 1;//��Cycle���ܤ�
					instruction.erase(instruction.begin()); //�Nissue�i�Ӫ�instruction�R��
					break;
				}
			}
		}
	}
}
void Dispatch()
{
	if (!bufferADD.empty)//�P�_bufferADD�O�_���F��
	{
		for (int i = 1; i <= 3; i++)//�qRS��X�ƭȳ��ǳƦn��
		{
			if (RS[i].used)
			{
				if (RS[i].rs1[0] != 'R' && RS[i].rs2[0] != 'R')
				{
					bufferADD.RS = i;
					bufferADD.empty = 1;
					bufferADD.cycle = cycle + C_ADDandSUB;
					changedCycle = 1;
				}
			}
		}
	}
	if (!bufferMUL.empty)//�P�_bufferMUL�O�_���F��
	{
		for (int i = 4; i <= 5; i++)//�qRS��X�ƭȳ��ǳƦn��
		{
			if (RS[i].used)//�n�������k�M���k�ⶵ�A�]��ALUcycle�Ƥ��P
			{
				if (RS[i].rs1[0] != 'R' && RS[i].rs2[0] != 'R')
				{
					bufferMUL.RS = i;
					bufferMUL.empty = 1;
					if (RS[i].operand == "*")
						bufferMUL.cycle = cycle + C_MUL;
					else
						bufferMUL.cycle = cycle + C_DIV;
					changedCycle = 1;
				}
			}
		}
	}
}

void WriteResult(buffer& thisbuffer)
{
	int result;
	if (cycle == thisbuffer.cycle)
	{
		//�p��buffer����
		string operand = RS[thisbuffer.RS].operand;
		int rs1 = stoi(RS[thisbuffer.RS].rs1);
		int rs2 = stoi(RS[thisbuffer.RS].rs2);
		if (operand == "+")
			result = rs1 + rs2;
		else if (operand == "-")
			result = rs1 - rs2;
		else if (operand == "*")
			result = rs1 * rs2;
		else
			result = rs1 / rs2;

		//��sRAT�MRF����
		for (int i = 1; i <= 5; i++)
			if (RAT[i] == "RS" + to_string(thisbuffer.RS))
			{
				RAT[i] = "";
				RF[i] = result;
			}

		//�Nresult�g�iRS
		for (int i = 1; i <= 5; i++)
		{
			if (RS[i].rs1 == "RS" + to_string(thisbuffer.RS))
				RS[i].rs1 = to_string(result);
			if (RS[i].rs2 == "RS" + to_string(thisbuffer.RS))
				RS[i].rs2 = to_string(result);
		}

		//freeRS
		RS[thisbuffer.RS].operand = "";
		RS[thisbuffer.RS].rs1 = "";
		RS[thisbuffer.RS].rs2 = "";
		RS[thisbuffer.RS].used = 0;
		thisbuffer.empty = 0;
		changedCycle = 1;
	}
}
void printCycleStatus()
{
	cout << "Cycle : " << cycle << endl << endl;

	cout << "     --RF---" << endl;
	for (int i = 1; i <= 5; ++i)
		cout << "  F" << i << " | " << setw(3) << RF[i] << " |" << endl;
	cout << "     -------" << endl << endl;

	cout << "     --RAT--" << endl;
	for (int i = 1; i <= 5; ++i)
		cout << "  F" << i << " | " << setw(3) << RAT[i] << " |" << endl;
	cout << "     -------" << endl << endl;

	cout << "     -RS----------------" << endl;
	for (int i = 1; i <= 3; i++)
	{
		cout << " RS" << i << " | " << setw(3) << RS[i].operand << " | " << setw(3) << RS[i].rs1 << " | " << setw(3) << RS[i].rs2 << " | " << endl;

		if (i == 3)
		{
			cout << "     -------------------" << endl;
			cout << " BUFFER: ";
			if (!bufferADD.empty)
				cout << "empty" << endl << endl;
			else
				cout << "(RS" << bufferADD.RS << ") " << RS[bufferADD.RS].rs1 << " " << RS[bufferADD.RS].operand << " " << RS[bufferADD.RS].rs2 << endl << endl;
		}
	}

	cout << "     -RS----------------" << endl;
	for (int i = 4; i <= 5; i++)
	{
		cout << " RS" << i << " | " << setw(3) << RS[i].operand << " | " << setw(3) << RS[i].rs1 << " | " << setw(3) << RS[i].rs2 << " | " << endl;

		if (i == 5)
		{
			cout << "     -------------------" << endl;
			cout << " BUFFER: ";
			if (!bufferMUL.empty)
				cout << "empty" << endl << endl;
			else
				cout << "(RS" << bufferMUL.RS << ") " << RS[bufferMUL.RS].rs1 << " " << RS[bufferMUL.RS].operand << " " << RS[bufferMUL.RS].rs2 << endl << endl;
		}
	}
}
int main()
{
	input();//Ū��
	//��JALU��Cycle Time
	cout << "Please input cycle time of ADD/SUB, MUL, DIV." << endl;
	cin >> C_ADDandSUB >> C_MUL >> C_DIV;

	while (true)
	{
		if (instruction.empty()) //�P�_instruction��RS�٦��S���F��
		{
			int i;
			for (i = 0; i < 5; i++)
				if (RS[i].used != 0)
					break;
			if (i == 5)
				break;
		}
		changedCycle = 0;
		WriteResult(bufferADD);//�ݦ��S��ADD ALU����
		WriteResult(bufferMUL);//�ݦ��S��MUL ALU����
		Dispatch();
		Issue();
		if (changedCycle)
			printCycleStatus();
		cycle++;
	}
}