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
int indiv_it = 0, fam_it = 0, child_it, u_fam=0, u_id=0;
string unique_id[5000];
string unique_fam[1000];

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
		if(data[1]== 'I')
		{
			u_id++;
			unique_id[u_id]=data;
		}
		else if(data[1]=='F')
		{
			u_fam++;
			unique_fam[u_fam]=data;

		}
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
char** splitTheDate(char* theDate) {
	char** d = (char**)malloc(4*sizeof(char*));
	char* splitDate = strtok(theDate, " /\n");
	d[0] = splitDate;
	splitDate = strtok(NULL, " /\n");
	d[1] = splitDate;
	splitDate = strtok(NULL, " /\n");
	d[2] = splitDate;
	return d;
}

/* checks if firstDate is less than secondDate
 */
int compareDates(char** firstDate, char** secondDate) {
	if(atoi(firstDate[2]) < atoi(secondDate[2])) {
		return 1;
	} else if(atoi(firstDate[2]) == atoi(secondDate[2]) && monthToInt(firstDate[1]) < monthToInt(secondDate[1])) {
		return 2;
	} else if(atoi(firstDate[2]) == atoi(secondDate[2]) && monthToInt(firstDate[1]) == monthToInt(secondDate[1]) && atoi(firstDate[0]) < atoi(secondDate[0])) {
		return 3;
	}
	return 0;
}

/* US02: Birth before marriage
 * prints error message if INDI's birth date occurs after marriage
 */
void bornBeforeMarriage() {
	char** marriage = (char**)malloc(4*sizeof(char*));
	char** indiv = (char**)malloc(4*sizeof(char*));
	int id, compareValue;
	for(int j = 1; j <= fam_it; j++) {
		char* marriageDate = (char*)malloc((FAMs[j][2].length()+1)*sizeof(char));
		FAMs[j][2].copy(marriageDate, FAMs[j][2].length(), 0);
		marriage = splitTheDate(marriageDate);

		for(int k = 0; k < 2; k++) {
			id = atoi(FAMs[j][k].c_str());
			char* indivDate = (char*)malloc((INDIs[id][3].length()+1)*sizeof(char));
			INDIs[id][3].copy(indivDate, INDIs[id][3].length(), 0);
			indiv = splitTheDate(indivDate);

			if((compareValue = compareDates(marriage, indiv)) == 1) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
			} else if(compareValue == 2) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
			} else if(compareValue == 3) {
				cout << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Error US02: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs after marriage date in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
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
	char** marriage = (char**)malloc(4*sizeof(char*));
	char** indiv = (char**)malloc(4*sizeof(char*));
	int id, compareValue;
	for(int j = 1; j <= fam_it; j++) {
		char* marriageDate = (char*)malloc((FAMs[j][2].length()+1)*sizeof(char));
		FAMs[j][2].copy(marriageDate, FAMs[j][2].length(), 0);
		marriage = splitTheDate(marriageDate);

		for(int k = 5; k < 20 && FAMs[j][k] != "-1"; k++) {
			id = atoi(FAMs[j][k].c_str());
			char* indivDate = (char*)malloc((INDIs[id][3].length()+1)*sizeof(char));
			INDIs[id][3].copy(indivDate, INDIs[id][3].length(), 0);
			indiv = splitTheDate(indivDate);

			//compare the years, then months, then days
			if((compareValue = compareDates(indiv, marriage)) == 1) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
			} else if(compareValue == 2) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
			} else if(compareValue == 3) {
				cout << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << '\n';
				cout << "  Marriage Date:" << FAMs[j][2] << '\n';
				cout << "  Birth Date:" << INDIs[id][3] << '\n';

				result << "Anomaly US08: Birth Date of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") occurs before parents' marriage in Family @F" << j << "@." << endl;
				result << "  Marriage Date:" << FAMs[j][2] << endl;
				result << "  Birth Date:" << INDIs[id][3] << endl;
			}
		}
	}
	free(marriage);
	free(indiv);
}

/* US09: Child Born after Parents' Marriage
 * parses through all families and checks that all children are born before parent's death
 * prints error message for mother and anomaly message for father if otherwise
 */
void childBornBeforeParentsDeath() {

}

/* US10: person marries at least 14 years of age
 * parses through all families and checks that all married persons were at least 14 at date of marriage
 * prints error message if otherwise
 */
void confirmMarriageAge() {
	char** marriage = (char**)malloc(4*sizeof(char*));
	char** indiv = (char**)malloc(4*sizeof(char*));
	int id;
	for(int j = 1; j <= fam_it; j++) {
		char* marriageDate = (char*)malloc((FAMs[j][2].length()+1)*sizeof(char));
		FAMs[j][2].copy(marriageDate, FAMs[j][2].length(), 0);
		marriage = splitTheDate(marriageDate);

		for(int k = 0; k < 2; k++) {
			id = atoi(FAMs[j][k].c_str());
			char* indivDate = (char*)malloc((INDIs[id][3].length()+1)*sizeof(char));
			INDIs[id][3].copy(indivDate, INDIs[id][3].length(), 0);
			indiv = splitTheDate(indivDate);

			if(compareDates(marriage, indiv) == 0) {
				if((atoi(marriage[2]) - atoi(indiv[2])) < 14) {
					cout << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << '\n';
					cout << "  Marriage Date:" << FAMs[j][2] << '\n';
					cout << "  Birth Date:" << INDIs[id][3] << '\n';

					result << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << endl;
					result << "  Marriage Date:" << FAMs[j][2] << endl;
					result << "  Birth Date:" << INDIs[id][3] << endl;
				} else if((atoi(marriage[2]) - atoi(indiv[2])) == 14 && monthToInt(marriage[1]) < monthToInt(indiv[1])) {
					cout << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << '\n';
					cout << "  Marriage Date:" << FAMs[j][2] << '\n';
					cout << "  Birth Date:" << INDIs[id][3] << '\n';

					result << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << endl;
					result << "  Marriage Date:" << FAMs[j][2] << endl;
					result << "  Birth Date:" << INDIs[id][3] << endl;
				} else if((atoi(marriage[2]) - atoi(indiv[2])) == 14 && monthToInt(marriage[1]) == monthToInt(indiv[1]) && atoi(marriage[0]) < atoi(indiv[0])) {
					cout << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << '\n';
					cout << "  Marriage Date:" << FAMs[j][2] << '\n';
					cout << "  Birth Date:" << INDIs[id][3] << '\n';

					result << "Error US10: Age of " << INDIs[id][0] << " " << INDIs[id][1] << " (" << unique_id[id] << ") in Family @F" << j << "@ at marriage is less than 14 years." << endl;
					result << "  Marriage Date:" << FAMs[j][2] << endl;
					result << "  Birth Date:" << INDIs[id][3] << endl;
				}
			}
		}
	}
	free(marriage);
	free(indiv);
}

void checkGender()
{
	int j;
	for(j = 1; j <= fam_it; j++)
	{
		string temp = FAMs[j][0];
		int temp1 = atoi(temp.c_str());
		string temp_s= INDIs[temp1][2];

		string temp_W12 = FAMs[j][1];
		int temp12 = atoi(temp_W12.c_str());
		string tem12 = INDIs[temp12][2];

		if(temp_s =="F")
		{
			temp_s="Female";
			cout<<"Error US21-Correct gender for "<< INDIs[temp1][0]<<" "<<INDIs[temp1][1]<<" having sex "<<temp_s<<" must be male as he is husband of "
			     <<INDIs[temp12][0]<<" "<< INDIs[temp12][1]<<"\n";
		}

		string temp_W = FAMs[j][1];
		int temp2 = atoi(temp_W.c_str());
		string tem = INDIs[temp2][2];


		if(tem =="M")
		{
			tem= "Male";
			cout<<"Error US21-Correct gender for "<<INDIs[temp2][0]<<" "<< INDIs[temp2][1]<<" having gender "<<tem<<" must be female as she is wife of "
			     << INDIs[temp1][0]<<" "<<INDIs[temp1][1]<<endl;
		}

		// Writing to output.txt
		string temp_H1 = FAMs[j][0];
		int temp3 = atoi(temp_H1.c_str());
		string temprs_h= INDIs[temp3][2];

		string temp_W123 = FAMs[j][1];
		int temp123 = atoi(temp_W123.c_str());
		string tem123 = INDIs[temp123][2];
		if(temprs_h =="F")
		{
			temprs_h= "Female";
			result <<"Error US21-Correct gender for "<<INDIs[temp3][0] << " " << INDIs[temp3][1] <<" having gender "<<temprs_h<<" must be male as he is husband of "
			     <<INDIs[temp123][0]<<" "<< INDIs[temp123][1]<<"\n";
		}

		string temp_W1 = FAMs[j][1];
		int temp4 = atoi(temp_W1.c_str());
		string temprs_w= INDIs[temp4][2];
		if (temprs_w =="M")
		{
			temprs_w= "Male";
			result <<"Error US21-Correct gender for "<<INDIs[temp4][0] << " " << INDIs[temp4][1]<<" having gender " <<temprs_w<<" must be female as she is wife of "
					<< INDIs[temp1][0]<<" "<<INDIs[temp1][1]<<endl;
		}
	}
}

void checkID()
{
	int arr_size = sizeof(unique_id)/sizeof(unique_id[0]);
	int size=0;

	for(int i=0;i< arr_size; i++)
	{
		if(unique_id[i]!="")
			size++;
	}
	for(int i = 0; i < size; i++)
      for(int j = i+1; j < size; j++)
        if(unique_id[i] == unique_id[j])
         {
		   cout<<"Error US22:Individual ID: "<<unique_id[i]<<" of"<<" "<< INDIs[i][0] << " " << INDIs[i][1] << " is conflicting with Individual ID: "
		       <<unique_id[j]<<" of"<<" "<<INDIs[j][0] << " " << INDIs[j][1]<<endl;

       	}
           int arr_sizet = sizeof(unique_id)/sizeof(unique_id[0]);
	int sizet=0;

	// printing to output.txt
	for(int i=0;i< arr_sizet; i++)
	{
		if(unique_id[i]!="")
			sizet++;
	}
	for(int i = 0; i <= sizet; i++)
      for(int j = i+1; j <= sizet; j++)
        if(unique_id[i] == unique_id[j])
              result<<"Error US22: Individual ID: "<<unique_id[i]<<" of"<<" "<< INDIs[i][0] << " " << INDIs[i][1] << " is conflicting with Individual ID: "
		       <<unique_id[j]<<" of"<<" "<<INDIs[j][0] << " " << INDIs[j][1]<<endl;
}

void checkFid()
{
	int arr_sizef = sizeof(unique_fam)/sizeof(unique_fam[0]);
	int size=0;

	for(int i=0;i< arr_sizef; i++)
	{
		if(unique_fam[i]!="")
			size++;
	}
	size++;
	for(int i = 0; i < size; i++)
      for(int j = i+1; j < size; j++)
        if(unique_fam[i] == unique_fam[j])
           cout<<"Error US22: Family ID "<<unique_fam[i] << " is shared by two families";

    int arr_sizef1 = sizeof(unique_fam)/sizeof(unique_fam[0]);
	int size1=0;

    for(int i=0;i< arr_sizef1; i++)
	{
		if(unique_fam[i]!="")
			size1++;
	}
    size++;
	for(int i = 1; i <= size1; i++)
      for(int j = i+1; j <= size1; j++)
        if(unique_fam[i] == unique_fam[j])
			result<<"Error US22: Family Id "<<unique_fam[i] <<" is shared by two families";
}


/* opens GEDCOM file and creates new output.txt
 * reads the level of each line and runs appropriate function
 * if unrecognized, prints error message and terminates program
 */
int main(int argc, char* argv[])
 {
	string num;
	int j;
	const char* def = "GEDCOM_test.ged";
	if(argc > 1)
	{
		def = argv[1];
	}
	test.open(def, ios::in);
	result.open("output.txt", ios::out);
	test >> data;
	while(!test.eof())
	 {
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
	for(j = 1; j <= indiv_it; j++)
	{
		cout << unique_id[j] << ": ";
		cout << INDIs[j][0] << " " << INDIs[j][1] <<"   " <<"Gender:"<< INDIs[j][2] << '\n';

		result << unique_id[j] << ": ";
		result << INDIs[j][0] << " " << INDIs[j][1] <<"   "<<"Gender:"<< INDIs[j][2] <<endl;
	}

	//print the family IDs as well as names and IDs of all husbands and wives
	cout << '\n' << "========= FAMs-IDs - IDs -Spouses' Names- Sex - US21-Correct gender for role ==============" << '\n';
	result << '\n' << "======== FAMs-IDs - IDs - Spouses' Names- Sex- US21- Correct gender for role ==============" << endl;
	for(j = 1; j <= fam_it; j++)
	{
		cout << "Family ID: " << unique_fam[j] << ": " << '\n';
		cout << "Husband ID: " << unique_id[atoi(FAMs[j][0].c_str())] << '\n';
		string temp = FAMs[j][0];
		int temp1 = atoi(temp.c_str());
		cout<< "Husband Name: " << INDIs[temp1][0] <<" "<<INDIs[temp1][1] <<"\n";
		cout<<"Husband Sex:" <<INDIs[temp1][2]<<"\n";

		cout << "Wife ID: " << unique_id[atoi(FAMs[j][1].c_str())] <<""<< '\n';
		string temp_W = FAMs[j][1];
		int temp2 = atoi(temp_W.c_str());
		cout<< "Wife Name: " << INDIs[temp2][0] <<" "<< INDIs[temp2][1] <<"\n";
		cout<< "Wife Sex:" <<INDIs[temp2][2]<<"\n";

		// Writing to output.txt
		result << "Family ID: " << unique_fam[j] << ": " << '\n';
		result << "Husband ID: " << unique_id[atoi(FAMs[j][0].c_str())] << '\n';
		string temp_H1 = FAMs[j][0];
		int temp3 = atoi(temp_H1.c_str());
		result << "Husband Name: " << INDIs[temp3][0] << " " << INDIs[temp3][1] <<"\n";
		result << "Husband Sex: " << INDIs[temp3][2] <<"\n";

		result << "Wife ID: " << unique_id[atoi(FAMs[j][1].c_str())] << '\n';
		string temp_W1 = FAMs[j][1];
		int temp4 = atoi(temp_W1.c_str());
		result << "Wife Name: " << INDIs[temp4][0] << " " << INDIs[temp4][1] << endl;
		result << "Wife Sex: "<< INDIs[temp4][2] << endl;

	}


	//print anyone who was married before their birthdate
	cout << '\n' << "========================== US02 - Birth before Marriage =============================" << '\n';
	result << '\n' << "========================== US02 - Birth before Marriage =============================" << endl;
	bornBeforeMarriage();

	//print anyone who was born before parents' marriage
	cout << '\n' << "========================== US08 - Child born before Parents' Marriage =============================" << '\n';
	result << '\n' << "========================== US08 - Child born before Parents' Marriage =============================" << endl;
	childBornAfterMarriage();

	//print anyone who was born after parent's death
	cout << '\n' << "========================== US09 - Child born after Parent's Death =============================" << '\n';
	result << '\n' << "========================== US09 - Child born after Parent's Death =============================" << endl;
	childBornBeforeParentsDeath();

	//print anyone who was less than 14 at date of marriage
	cout << '\n' << "========================== US10 - At Least 14 at Marriage =============================" << '\n';
	result << '\n' << "========================== US10 - At Least 14 at Marriage =============================" << endl;
	confirmMarriageAge();

	// print husband having gender female and wife having gender male in family
	cout << '\n' << "========================== US21 - Correct gender for role =============================" << '\n';
	result << '\n' << "========================== US21 - Correct gender for role =============================" << endl;
	checkGender();

    cout << '\n' << "========================== US22 - Unique ID for Individual and family =============================" << '\n';
	result << '\n' << "========================== US22 - Unique ID for Individual and family =============================" << endl;
	checkID();
	checkFid();


	test.close();
	result.close();
	return 0;
}
