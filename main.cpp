#include <iostream>
#include <fstream> 
#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

struct Team {
    string name;
    int winCount;
    int scoreCount;
};

struct Pattern{
    string format;
    int result;
    bool hasScore;
};

const int NUMBER_OF_TEAMS = 2;
const int ERROR_THREASHOLD = 2;
const string DATA_FILENAME = "data.txt";
const string PATTERN_FILENAME = "patterns.txt";
const string TEAM_NAMES[NUMBER_OF_TEAMS] = {"perspolis","esteghlal"};
const string PATTERN_NAME_PARAMETER = "$name";
const string PATTERN_SCORE_PARAMETER = "$s";

void checkFile(fstream&, string);
int lineCount(fstream&);
void initTeam(Team[]);
int string2Int(string);
string int2String(int);
void readPattern(fstream&, Pattern[], int);
string patternNameMaker(Pattern, Team);
void patternScoreMaker(string&, int[]);
void lowerCase(string&);
string trimWhiteSpace(string);
void removeDot(string&);
void scoreSpaces(string&);
bool extractInteger(string, int[]);
int switchTeam(int);
bool compareString(string, string);
void extractScoredData(string, Team[], int[], int&, int&, int&, Pattern[], int);
void extractData(string, Team[], int&, int&, Pattern[], int);
void analyzeData(string, Team[], int&, int&, int&, Pattern[], int);
void readData(fstream&, Team[], int&, int&, int&, Pattern[], int);
void finalStats(Team[], int, int, int);

int main(){

    fstream dataFile, patternFile;
    checkFile(dataFile, DATA_FILENAME);
    checkFile(patternFile, PATTERN_FILENAME);

    const int PATTERN_COUNT = lineCount(patternFile);
    Pattern patterns[PATTERN_COUNT];
    readPattern(patternFile, patterns, PATTERN_COUNT);
    patternFile.close();

    Team teams[2];
    initTeam(teams);

    int totalVote = 0, drawCount = 0, scorePredictionCount = 0;
    readData(dataFile, teams, totalVote, drawCount, scorePredictionCount, patterns, PATTERN_COUNT);
    dataFile.close();
    
    finalStats(teams, totalVote, drawCount, scorePredictionCount);

    return 0;
}

void checkFile(fstream &file, string filename){
    file.open(filename, ios::in);
    if (! file){
        cout << "could not open " << filename << "!" << endl;
        exit(1);
    }
}

int lineCount(fstream &file){
    int count = 0;
    string line;
    while (getline(file, line)){
        count++;
    }
    file.clear();
    file.seekg(0, ios::beg);
    return count;
}

void initTeam(Team teams[]){
    for (int i = 0; i < NUMBER_OF_TEAMS; i++){
        teams[i].name = TEAM_NAMES[i];
        teams[i].scoreCount = 0;
        teams[i].winCount = 0;
    }
}

int string2Int(string input){
    istringstream toBeConverted(input);
    int result;
    toBeConverted >> result;
    return result;
}

string int2String(int input){
    ostringstream toBeConverted;
    toBeConverted << input;
    return toBeConverted.str();
}

void readPattern(fstream &file, Pattern patterns[], int patternCount){
    string data;
    for (int i = 0; i < patternCount; i++){
        getline(file, data, ';');
        patterns[i].format = data;
        getline(file, data, ';');
        patterns[i].result = string2Int(data);
        getline(file, data, '\n');
        patterns[i].hasScore = string2Int(data);
    }
}

string patternNameMaker(Pattern pattern, Team team){
    string str = pattern.format;
    string substr = team.name;
    int index = 0;
    while (true){
        index = str.find(PATTERN_NAME_PARAMETER, index);
        if (index == string::npos){
            break;
        }
        str.replace(index, PATTERN_NAME_PARAMETER.length(), substr);
        index += PATTERN_NAME_PARAMETER.length();
    }
    return str;
}

void patternScoreMaker(string &str, int scores[]){ 
    int scoreIndex = 0;
    int index = 0;
    while (true){
        index = str.find(PATTERN_SCORE_PARAMETER, index);
        if (index == string::npos){
            break;
        }
        str.replace(index, PATTERN_SCORE_PARAMETER.length(), int2String(scores[scoreIndex]));
        index += PATTERN_SCORE_PARAMETER.length();
        scoreIndex++;
    }
}

void lowerCase(string &str){
    for (int i = 0; i < str.length(); i++){
        str[i] = tolower(str[i]);
    }
}

void scoreSpaces(string &str){
    int index = 0;
    index = str.find("-", index);
    if (index != string::npos){
        if (str[index - 1] != ' ' && str[index + 1] == ' '){
            str.replace(index, 1, " -");
        } else if (str[index - 1] == ' ' && str[index + 1] != ' '){
            str.replace(index, 1, "- ");
        } else if (str[index - 1] != ' ' && str[index + 1] != ' '){
            str.replace(index, 1, " - ");
        }
    }
    return;
}

string trimWhiteSpace(string str){
    istringstream input(str);
    string word, output;
    while (input >> word){
        if (! output.empty()){
            output += ' ';
        }  
        output += word;
    }
    return output;
}

void removeDot(string &str){
    int index = 0;
    while (true){
        index = str.find(".", index);
        if (index == string::npos){
            break;
        }
        str.erase(index, 1);
    }
}

bool extractInteger(string str, int scores[]){
    int index = 0;
    stringstream stream;
    stream << str;
    string temp;
    int number;
    while (stream >> temp) {
        if (stringstream(temp) >> number){
            scores[index] = number;
            index++;
        }
        temp = "";
    }
    if (index == 2){
        return true;
    } else {
        return false;
    }
}

int switchTeam(int i){
    if (i == 0){
        return 1;
    } else if (i == 1){
        return 0;
    }
    return -1;
}

bool compareString(string str1, string str2){
    int faultCount = 0;
    if (str1.length() == str2.length()){
        for (int i = 0; i < str1.length(); i++){
            if (str1[i] != str2[i]){
                faultCount++;
            }
        }
    } else {
        string longStr = (str1.length() > str2.length())? str1: str2;
        string shortStr = (longStr == str1)? str2: str1;
        if (longStr.length() - shortStr.length() <= ERROR_THREASHOLD){
            for (int i = 0, j = 0; i < longStr.length() && j < shortStr.length(); i++, j++){
                char c1 = longStr[i], c2 = shortStr[j];
                if (shortStr[j] != longStr[i]){
                    faultCount++;
                    j--;
                }
            }

        } else {
            return false;
        }
    }
    if (faultCount <= ERROR_THREASHOLD){
        return true;
    }
    return false;
}

void extractScoredData(string data, Team teams[], int scores[], int &totalVote, int &drawCount, int &scorePredictionCount , Pattern patterns[], int patternCount){
    string generatedPattern;
    for (int i = 0; i < NUMBER_OF_TEAMS; i++){
        for (int j = 0; j < patternCount; j++){
            if (patterns[j].hasScore){
                generatedPattern = patternNameMaker(patterns[j], teams[i]);
                patternScoreMaker(generatedPattern, scores);
                if (compareString(generatedPattern, data)){
                    totalVote++;
                    scorePredictionCount++;
                    if (patterns[j].result == 2){
                        teams[i].winCount++;
                        teams[i].scoreCount += scores[0];
                        teams[switchTeam(i)].scoreCount += scores[1];
                    } else if (patterns[j].result == 0){
                        drawCount++;
                        teams[i].scoreCount += scores[0];
                        teams[switchTeam(i)].scoreCount += scores[1];
                    } else if (patterns[j].result == 1){
                        teams[i].scoreCount += min(scores[0], scores[1]);
                        teams[switchTeam(i)].winCount++;
                        teams[switchTeam(i)].scoreCount += max(scores[0], scores[1]);
                    }
                    return;
                }
            }
        }
    }
}

void extractData(string data, Team teams[], int &totalVote, int &drawCount, Pattern patterns[], int patternCount){
    string generatedPattern;
    for (int i = 0; i < NUMBER_OF_TEAMS; i++){
        for (int j = 0; j < patternCount; j++){
            if (! patterns[j].hasScore){
                generatedPattern = patternNameMaker(patterns[j], teams[i]);
                if (compareString(generatedPattern, data)){
                    totalVote++;
                    if (patterns[j].result == 2){
                        teams[i].winCount++;
                    } else if (patterns[j].result == 0){
                        drawCount++;
                    } else if (patterns[j].result == 1){
                        teams[switchTeam(i)].winCount++;
                    }
                    return;
                }
            }
        }
    }
}

void analyzeData(string data, Team teams[], int &totalVote, int &drawCount, int &scorePredictionCount, Pattern patterns[], int patternCount){
    int scores[2];
    if (extractInteger(data, scores)){
        extractScoredData(data, teams, scores, totalVote, drawCount, scorePredictionCount, patterns, patternCount);
    } else {
        extractData(data, teams, totalVote, drawCount, patterns, patternCount);
    }
}

void readData(fstream &file, Team teams[], int &totalVote, int &drawCount, int &scorePredictionCount, Pattern patterns[], int patternCount){
    string data;
    while (getline(file, data)){
        lowerCase(data);
        data = trimWhiteSpace(data);
        removeDot(data);
        scoreSpaces(data);
        analyzeData(data, teams, totalVote, drawCount, scorePredictionCount, patterns, patternCount);
    }
}

void finalStats(Team teams[], int totalVote, int drawCount, int scorePredictionCount){
    float teamOneWinPercent = (teams[0].winCount / float(totalVote)) * 100;
    float teamTwoWinPercent = (teams[1].winCount / float(totalVote)) * 100;
    float drawPercent = (drawCount / float(totalVote)) * 100;
    float teamOneAverageScore = teams[0].scoreCount / float(scorePredictionCount);
    float teamTwoAverageScore = teams[1].scoreCount / float(scorePredictionCount);
    cout << "data analysis finished.\n-------------------------------------------------------\n";
    cout << teams[0].name << ":\n * win : " << teamOneWinPercent << "%\n * average score : " << teamOneAverageScore << endl;
    cout << teams[1].name << ":\n * win : " << teamTwoWinPercent << "%\n * average score : " << teamTwoAverageScore << endl;
    cout << "draw : " << drawPercent << "%" << endl;
}