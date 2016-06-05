/* CS 555 WS
 * Project 03 - GEDCOM parser : Save INDIs and FAMs
 * Clara Ramos, Larisa Machado, Varun Kumar
 * We pledge our honor that we have abided by the Stevens Honor System.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
using namespace std;

/* Global variables
 * data = current string pulled from GEDCOM file
 * tag = line's tag; if N/A, "Invalid tag"
 * test = file that code is parsing through
 * result = file where output is printed
 * INDIs = array that holds the first name of each individual
 * FAMs = array that holds the families
 * indiv = flag that dictates if the info being read is from an INDI
 * it = iterator for INDIs
 */
string data, tag = "Invalid tag";
fstream test, result;
string INDIs[5000][2];
string FAMs[1000][2];
bool indiv = false;
int indiv_it = 0;
bool fam = false;
int fam_it = 0;

/* Takes in the line's current level
 * prints the level to output and output.txt
 */
void printLevel(string s) {
	cout << s << '\n';
	cout << tag << '\n';
	result << s << endl;
	result << tag << endl;
}

//prints what's currently in data and the rest of the line
void restOfLine() {
	cout << data;
	result << data;
	getline(test, data);
	cout << data << '\n';
	result << data << endl;
}

/* Current line's level is 0
 * parses through the line looking for applicable tag
 * prints the line, level, and tag to output and to output.txt
 */
int foundAZero() {
	test >> data;
	if(data == "HEAD" || data == "TRLR" || data == "NOTE") {
		indiv = false;
		tag = data;
		cout << data << '\n';
		result << data << endl;
	} else if(data[0] == '@') {
		cout << data << " ";
		result << data << " ";
		test >> data;

		//flags when line describes a FAM or INDI
		if(data == "INDI" || data == "FAM") {
			if(data == "INDI") {
				indiv = true;
				indiv_it++;
			} else if (data == "FAM") {
				fam = true;
				fam_it++;
			}
			tag = data;
			cout << data << '\n';
			result << data << endl;
		} else {
			indiv = false;
			restOfLine();
		}
	} else {
		indiv = false;
		restOfLine();
	}
	return 0;
}

/* Current line's level is 1
 * parses through the line looking for tag
 * prints line, level, and tag to output and output.txt
 */
int foundAOne() {
	test >> data;
	if(data == "HUSB" || data == "WIFE" || data == "CHIL") {
		if (data == "HUSB" && fam == true ) {
		test >> data;
		string temp = data;
		temp.erase (temp.begin()-1);
		temp.erase(temp.begin()-1);
		temp.erase (temp.end()-1);
		cout<< temp << " " << fam_it << endl;
		FAMs[fam_it][0] = temp;	
		}
		 else if (data == "WIFE" && fam == true) {
		test >> data;
		string temp = data;
		temp.erase (temp.begin()-1);
		temp.erase(temp.begin()-1);
		temp.erase (temp.end()-1);
		cout<< temp << " " << fam_it << endl;
		FAMs[fam_it][1] = temp;
		}
		tag = data;
		restOfLine();
	} else if(data == "NAME" && indiv == true) {
		tag = data;
		restOfLine();

		/* divides the INDIs name to first and last
		 * removes the '/' surrounding the last name
		 */
		char* fullName = &data[0];
		char* splitName = strtok(fullName, " /");
		INDIs[indiv_it][0] = splitName;
		splitName = strtok(NULL, " /");
		INDIs[indiv_it][1] = splitName;
	} else if(data == "NAME" || data == "SEX" || data == "FAMC" || data == "FAMS" || data == "DEAT") {
		tag = data;
		restOfLine();
	} else if(data == "BIRT" || data == "MARR" || data == "DIV") {
		tag = data;
		cout << data << '\n';
		result << data << endl;
	} else {
		restOfLine();
	}
	return 0;
}

/* Current line's level is 2
 * parses through the line looking for tag
 * prints line, level, and tag to output and output.txt
 */
int foundATwo() {
	test >> data;
	if(data == "DATE") {
		tag = data;
		restOfLine();
	} else {
		restOfLine();
	}
	return 0;
}

/* opens GEDCOM file and creates new output.txt
 * reads the level of each line and runs appropriate function
 * if unrecognized, prints error message and terminates program
 */
int main() {
	string num;
	int j;
	test.open("GEDCOM_test.ged", ios::in);
	result.open("output.txt", ios::out);
	test >> data;
	while(!test.eof()) {
		tag = "Invalid tag";
		cout << data << " ";
		result << data << " ";
		if(data[0] == '0') {
			foundAZero();
			num = "0";
		} else if(data[0] == '1') {
			foundAOne();
			num = "1";
		} else if(data[0] == '2') {
			foundATwo();
			num = "2";
		} else {
			cerr << "Error: unrecognizable level. Program Terminated" << '\n';
			result << "Error: unrecognizable level. Program Terminated" << '\n';
			return -1;
		}
		printLevel(num);
		test >> data;
	}

	cout << '\n' << "========================== INDIs ============================" << '\n';
	result << '\n' << "========================== INDIs ============================" << endl;
	for(j = 1; j <= indiv_it; j++) {
		cout << "@I" << j << "@: ";
		cout << INDIs[j][0] << " " << INDIs[j][1] << '\n'; 

		result << "@I" << j << "@: ";
		result << INDIs[j][0] << " " << INDIs[j][1] << endl;
	}

	cout << '\n' << "========================== FAMs =============================" << '\n';
	result << '\n' << "========================== FAMs =============================" << endl;
	for(j = 1; j <= fam_it; j++) {
		cout << "Family ID:" << "@F" << j << "@: " << '\n';
		cout << "Husband ID:" << "@I" << FAMs[j][0] << "@" << '\n';
		string temp = FAMs[j][0];
		int temp1 = atoi(temp.c_str());
		cout<< "Husband Name:" << INDIs[temp1][0] <<" "<<INDIs[temp1][1] <<"\n";
		
		cout << "Wife ID:" <<"@I"<< FAMs[j][1] <<"@"<< '\n';
		string temp_W = FAMs[j][1];
		int temp2 = atoi(temp_W.c_str());
		cout<< "Wife Name:" << INDIs[temp2][0] <<" "<< INDIs[temp2][1] <<"\n";
		
		// Writing to output.txt
		result << "Family ID:" << "@F" << j << "@: " << '\n';
		result << "Husband ID:" << "@I" << FAMs[j][0] << "@" << '\n';
		string temp_H1 = FAMs[j][0];
		int temp3 = atoi(temp_H1.c_str());
		result << "Husband Name:" << INDIs[temp3][0] << " " << INDIs[temp3][1] <<"\n";
		
		result << "Wife ID:" <<"@I"<< FAMs[j][1] << "@" << '\n';
		string temp_W1 = FAMs[j][1];
		int temp4 = atoi(temp_W1.c_str());
		result << "Wife Name:" << INDIs[temp4][0] << " " << INDIs[temp4][1] <<"\n";
		
	}
    
	test.close();
	result.close();
	return 0;
}
