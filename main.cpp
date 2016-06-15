/* CS 555 WS
 * Project 04 - GEDCOM parser : Sprint 1
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
 * date = states which tag the date belongs to
 * test = file that code is parsing through
 * result = file where output is printed
 * indiv = flag that dictates if the info being read is from an INDI
 * indiv_it = iterator for INDIs
 * fam = flag that dictates if new FAM is read
 * fam_it = iterator for FAMs
 */
string data, tag = "Invalid tag", date = "N/A";
fstream test, result;
bool indiv = false, fam = false;
int indiv_it = 0, fam_it = 0, child_it;

/* INDIs = STRING array that holds people's information
 * Based on indices in the following format:
 * INDIs[x][0] = first name
 * INDIs[x][1] = last name
 * INDIs[x][2] = sex
 * INDIs[x][3] = birth date
 * INDIs[x][4] = death date
 * INDIs[x][5] = FAMC pointer
 * INDIs[x][6] = FAMS pointer
 * If the data is not applicable, leave the index empty (e.g. if alive [x][4] is empty; if unmarried, [x][6] is empty)
 */
string INDIs[5000][7];

/* FAMs = STRING array that holds the families's information
 * Based on the following format:
 * FAMs[x][0] = Husband's ID
 * FAMs[x][1] = Wife's ID
 * FAMs[x][2] = marriage date
 * FAMs[x][3] = divorce date
 * FAMs[x][4] = index of latest recorded child; instantiated to 3
 * FAMs[x][y] = Child's ID (where y is from 5-19 since allowed up to 15 children)
 */
string FAMs[1000][20];


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
				child_it = 5;
				for(int k = 5; k < 20; k++) {
					FAMs[fam_it][k] = "-1";
				}
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
		FAMs[fam_it][0] = temp;
		}
		 else if (data == "WIFE" && fam == true) {
		test >> data;
		string temp = data;
		temp.erase (temp.begin()-1);
		temp.erase(temp.begin()-1);
		temp.erase (temp.end()-1);
		FAMs[fam_it][1] = temp;
		} else if(data == "CHIL" && fam == true) {
			test >> data;
			char* childID = &data[0];
			char* ID = strtok(childID, " /@I");
			FAMs[fam_it][child_it] = ID;
			child_it++;
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
	} else if(data == "SEX" && indiv == true) {
		tag = data;
		restOfLine();
		INDIs[indiv_it][2] = data[1];
	} else if(data == "NAME" || data == "FAMC" || data == "FAMS") {
		tag = data;
		restOfLine();
	} else if(data == "BIRT" || data == "MARR" || data == "DIV" || data == "DEAT") {
		date = data;
		tag = data;
		restOfLine();
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
		data.erase (data.end()-1);
		if(date == "BIRT") {
			INDIs[indiv_it][3] = data;
		} else if(date == "DEAT") {
			INDIs[indiv_it][4] = data;
		} else if(date == "MARR") {
			FAMs[fam_it][2] = data;
		} else if(date == "DIV") {
			FAMs[fam_it][3] = data;
		}
	} else {
		restOfLine();
	}
	return 0;
}

/* Converts months to the appropriate number representation
 * returns -1 if the string is unrecognized
 */
int monthToInt(string m) {
	if(m == "JAN") {
		return 1;
	} else if(m == "FEB") {
		return 2;
	} else if(m == "MAR") {
		return 3;
	} else if (m == "APR") {
		return 4;
	} else if(m == "MAY") {
		return 5;
	} else if(m == "JUN") {
		return 6;
	} else if(m == "JUL") {
		return 7;
	} else if(m == "AUG") {
		return 8;
	} else if(m == "SEP") {
		return 9;
	} else if(m == "OCT") {
		return 10;
	} else if(m == "NOV") {
		return 11;
	} else if(m == "DEC") {
		return 12;
	} else {
		return -1;
	}
}

/* reads in a string in form of DAY MONTH YEAR
 * parses through the string and places the day, month, year into a char**
 */
char** splitTheDate(string s) {
	char** d = (char**)malloc(4*sizeof(char*));
	char* theDate = &s[0];
	char* splitDate = strtok(theDate, " /");
	d[0] = splitDate;
	splitDate = strtok(NULL, " /");
	d[1] = splitDate;
	splitDate = strtok(NULL, " /");
	d[2] = splitDate;
	return d;
}

/* US02: Birth before marriage
 * prints error message if INDI's birth date occurs after marriage
 */
void bornBeforeMarriage() {
	int j, k, day;
	string current;
	char** marriage = (char**)malloc(4*sizeof(char*));
	char** indiv;
	int id;
	for(j = 1; j <= fam_it; j++) {
		//split the marriage date to day, month, year
		marriage = splitTheDate(FAMs[j][2]);

		for(k = 0; k < 2; k++) {
			//split the INDI's birth date to day, month, year
			id = atoi(FAMs[j][k].c_str());
			indiv = splitTheDate(INDIs[id][3]);

			//keeps track of day, because monthToInt overrides indiv[0]
			day = atoi(indiv[0]);

			//compare the years, then months, then days
			if(atoi(marriage[2]) < atoi(indiv[2])) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << indiv[0] << " " << indiv[1] << " " << indiv[2] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << indiv[0] << " " << indiv[1] << " " << indiv[2] << endl;
			} else if(atoi(marriage[2]) == atoi(indiv[2]) && monthToInt(marriage[1]) < monthToInt(indiv[1])) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << endl;
			} else if(atoi(marriage[2]) == atoi(indiv[2]) && monthToInt(marriage[1]) == monthToInt(indiv[1]) && atoi(marriage[0]) <= day) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << endl;
			}
		}
	}
	free(marriage);
	free(indiv);
}

/* US08: Child Born after Parents' Marriage
 * parses through all families and checks that all children are born after marriage
 * prints anomaly message if otherwise
 */
void childBornAfterMarriage() {
	int j, k, day;
	string current, month;
	char** marriage = (char**)malloc(4*sizeof(char*));
	char** indiv;
	int id;
	for(j = 1; j <= fam_it; j++) {
		//split the marriage date to day, month, year
		marriage = splitTheDate(FAMs[j][2]);

		for(k = 5; k < 20 && FAMs[j][k] != "-1"; k++) {
			//split the INDI's birth date to day, month, year
			id = atoi(FAMs[j][k].c_str());
			indiv = splitTheDate(INDIs[id][3]);
			//keeps track of day, because monthToInt overrides indiv[0]
			day = atoi(indiv[0]);
			month = indiv[1];

			//compare the years, then months, then days
			if(atoi(marriage[2]) > atoi(indiv[2])) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs before parents' marriage in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << indiv[0] << " " << indiv[1] << " " << indiv[2] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << indiv[0] << " " << indiv[1] << " " << indiv[2] << endl;
			} else if(atoi(marriage[2]) == atoi(indiv[2]) && monthToInt(marriage[1]) > monthToInt(indiv[1])) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << day << " " << month << " " << indiv[2] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << day << " " << month << " " << indiv[2] << endl;
			} else if(atoi(marriage[2]) == atoi(indiv[2]) && monthToInt(marriage[1]) == monthToInt(indiv[1]) && atoi(marriage[0]) >= day) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << '\n';
				cout << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (@I" << id << "@) occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date: " << marriage[0] << " " << marriage[1] << " " << marriage[2] << endl;
				result << "  Birth Date: " << day << " " << indiv[1] << " " << indiv[2] << endl;
			}
		}
	}
	free(marriage);
	free(indiv);
}

/* opens GEDCOM file and creates new output.txt
 * reads the level of each line and runs appropriate function
 * if unrecognized, prints error message and terminates program
 */
int main(int argc, char* argv[]) {
	string num;
	int j;
	const char* def = "GEDCOM_test.ged";
	if(argc > 1) {
		def = argv[1];
	}
	test.open(def, ios::in);
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

	//print the IDs and names of all individuals
		cout << '\n' << "========================== INDIs - Names -IDs - Gender ============================" << '\n';
	result << '\n' << "========================== INDIs - Names -IDs- Gender============================" << endl;
	for(j = 1; j <= indiv_it; j++) {
		cout << "@I" << j << "@: ";
		cout << INDIs[j][0] << " " << INDIs[j][1] <<"   " <<"Gender:"<< INDIs[j][2] << '\n';

		result << "@I" << j << "@: ";
		result << INDIs[j][0] << " " << INDIs[j][1] <<"   "<<"Gender:"<< INDIs[j][2] <<endl;
	}

	//print the family IDs as well as names and IDs of all husbands and wives
	cout << '\n' << "========= FAMs-IDs - IDs -Spouses' Names- Sex - US21-Correct gender for role ==============" << '\n';
	result << '\n' << "======== FAMs-IDs - IDs - Spouses' Names- Sex- US21- Correct gender for role ==============" << endl;
	for(j = 1; j <= fam_it; j++) {
		cout << "Family ID: " << "@F" << j << "@: " << '\n';
		cout << "Husband ID: " << "@I" << FAMs[j][0] << "@" << '\n';
		string temp = FAMs[j][0];
		int temp1 = atoi(temp.c_str());
		cout<< "Husband Name: " << INDIs[temp1][0] <<" "<<INDIs[temp1][1] <<"\n";
		cout<<"Husband Sex:" <<INDIs[temp1][2]<<"\n";
		// Check valid Sex tag for Husband
		string temp_s= INDIs[temp1][2];
		if(temp_s =="F")
		{
			cout<<"Husband should have Sex tag M"<<"\n";
		}

		cout << "Wife ID: " <<"@I"<< FAMs[j][1] <<"@"<< '\n';
		string temp_W = FAMs[j][1];
		int temp2 = atoi(temp_W.c_str());
		cout<< "Wife Name: " << INDIs[temp2][0] <<" "<< INDIs[temp2][1] <<"\n";
		cout<< "Wife Sex:" <<INDIs[temp2][2]<<"\n";
		//check valid sex tag for wife
		string tem = INDIs[temp2][2];
		if(tem =="M")
		{
			cout<<"Wife should have Sex tag F"<< "\n";
		}

		// Writing to output.txt
		result << "Family ID: " << "@F" << j << "@: " << '\n';
		result << "Husband ID: " << "@I" << FAMs[j][0] << "@" << '\n';
		string temp_H1 = FAMs[j][0];
		int temp3 = atoi(temp_H1.c_str());
		result << "Husband Name: " << INDIs[temp3][0] << " " << INDIs[temp3][1] <<"\n";
		result << "Husband Sex: " << INDIs[temp3][2] <<"\n";
		string temprs_h= INDIs[temp3][2];
		if(temprs_h =="F")
		{
			result << "Husband should have Sex tag M"<<"\n";
		}

		result << "Wife ID: " <<"@I"<< FAMs[j][1] << "@" << '\n';
		string temp_W1 = FAMs[j][1];
		int temp4 = atoi(temp_W1.c_str());
		result << "Wife Name: " << INDIs[temp4][0] << " " << INDIs[temp4][1] <<"\n";
		result << "Wife Sex: "<< INDIs[temp4][2];
		string temprs_w= INDIs[temp4][2];
		if (temprs_w =="M")
		{
			result <<"Wife should have Sex tag F"<<"\n";
		}
	}


	//print anyone who was married before their birthdate
	cout << '\n' << "========================== US02 - Birth before Marriage =============================" << '\n';
	result << '\n' << "========================== US02 - Birth before Marriage =============================" << endl;
	bornBeforeMarriage();

	//print anyone who was born before parents' marriage or after parents' divorce
	cout << '\n' << "========================== US08 - Child born after Parents' Marriage =============================" << '\n';
	result << '\n' << "========================== US08 - Child born after Parents' Marriage =============================" << endl;
	childBornAfterMarriage();

	test.close();
	result.close();
	return 0;
}
