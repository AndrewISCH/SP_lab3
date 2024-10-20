#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <regex>
#include <algorithm>
#include <iomanip>
#include <cmath>

#define NUMBER           0b100000000
#define STRING_CONST     0b010000000
#define CHAR_CONST       0b001000000
#define PREPROCESSOR     0b000100000
#define COMMENT          0b000010000
#define RESERVED_WORD    0b000001000 
#define OPERATOR         0b000000100
#define PUNCTUATION      0b000000010
#define IDENTIFIER       0b000000001
#define ERROR            0b000000000

using namespace std;

extern const vector<string>  reservedWords;

extern const vector<string>  punctuation;

extern const vector<string>  operators;

extern const vector<string>  preprocessorDirectives;


regex punctuationPattern(R"([;,}{><])");

regex singleLineCommentPattern(R"(^//[^\n]*$)");

regex blockCommentPattern(R"(^/\*.*?\*/)");

regex symbolPattern(R"(^'[\s\S]'$)");

regex idPattern(R"(^[a-zA-Z_][\w]*$)");
regex oneSymbolOperatorPattern(R"(^[+-=<>|&/?:*%\[\]\(\)!\^~\.]$)");

regex startsWithZeroNumberPattern(R"(^(0[0-7]*|0[Xx][0-9a-zA-Z]+|0[Bb][01]+|0?\.[0-9]*([eE][+-]?[\d]+)?)$)");

regex startsWithNotZeroNumberPattern(R"(^([1-9][0-9]*([uU]?[Ll]{0,2}|[lL]{0,2}[uU]?)?|[1-9][0-9]*\.[0-9]*([eE][+-]?[0-9]+)?)$)");

//regex breakNumberSequencePattern(R"(^[+-=<>|&/?:*%\[\]\(\)!\^~;,}{><\s])$)");
regex breakNumberSequencePattern(R"(^[+\-=<>|&/?:*%\[\]\(\)!\^~;,}{><\s]$)");


bool contains(const vector<string>  arr,int size, string element) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == element) {
            return true;
        }
    }
    return false;  
}


vector <pair<string, int>> analizeRegExp(const string& code){
    vector<pair<string,int>> lexemes;
    string token;
    int lineNumber = 1;
    bool isMultilineComment = false;

    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];

        // spaces
        if (isspace(c)) continue;

        // numbers
        if (isdigit(c) || c == '.') {
            if (c == '.' && (i + 1 >= code.length() || !isdigit(code[i + 1]))) {
                lexemes.push_back(make_pair(string(1, c), OPERATOR));
                continue;
            }
            bool dotted = (c == '.');
            bool zeroFirst = (c == '0' || dotted);
            token = c;
            i++;
            while (i < code.length() && !regex_match(string(1,code[i]), breakNumberSequencePattern)) {
                if (code[i] == '.') { 
                    if (dotted) {
                      break;
                    }
                    dotted = true;
                }
                if ((code[i] == 'E' || code[i] == 'e') && i+1 < code.length()) {
                    if (code[i + 1] == '+' || code[i + 1] == '-') {
                        token += code[i++];
                        token += code[i++];
                        continue;
                    }
                }
                token += code[i++];
            }
            regex pattern = zeroFirst ? startsWithZeroNumberPattern : startsWithNotZeroNumberPattern;
            if (regex_match(token, pattern)) lexemes.push_back(make_pair(token, NUMBER));
            else lexemes.push_back(make_pair(token, ERROR));
            if (i < code.length()) i--;
            continue;
        }
        // punctuation
        if (contains(punctuation, punctuation.size(), string(1, c))) {
            token = c;
            if (c == '<' || c == '>') {
                if (c == '<' && i+1 < code.length() && code[i + 1] != '>') {
                    if (code[i + 1] == '=' || code[i + 1] == '<') {
                        lexemes.push_back(make_pair(token + code[++i], OPERATOR));
                        
                    }
                    else {
                        lexemes.push_back(make_pair(token, OPERATOR | PUNCTUATION));
                    }
                    continue;
                }
                else if (c == '>' && i < code.length() && code[i + 1] != '<') {
                    if (code[i + 1] == '=' || code[i + 1] == '>') {
                        lexemes.push_back(make_pair(token + code[++i], OPERATOR));
                    }
                    else {
                        lexemes.push_back(make_pair(token, OPERATOR | PUNCTUATION));
                    }
                    continue;

                }
                else lexemes.push_back(make_pair(token, OPERATOR | PUNCTUATION));
            }
            else lexemes.push_back(make_pair(token, PUNCTUATION));
            continue;
        };
       
        
        // str const
        if (c == '\"') {
            token = "\"";
            ++i;
            while (i < code.length() && code[i] != '\"') {
                if (code[i] == '\n') ++lineNumber;
                token += code[i++];
            }
            if (i < code.length()) token += "\"";
            lexemes.push_back(make_pair(token,STRING_CONST));
            continue;
        }
        // char const
        if (c == '\'') {
            token = '\'';
            i++;
            while (code[i] != '\'') {
                token += code[i];
                i++;
            }
            if(regex_match(token, symbolPattern)) lexemes.push_back(make_pair(token, CHAR_CONST));
            else lexemes.push_back(make_pair(token, ERROR));
        }
        
        // Comments
        if (c == '/') {
            token = "/";
            if (i+1 < code.length() && code[i+1] == '/') {
                i+=2;
                token += '/';
                while (i < code.length() && code[i] != '\n') {
                    token += code[i++];
                }
                i++;
                lexemes.push_back(make_pair(token, COMMENT));
            }
            else if (i+1 < code.length() && code[1+i] == '*') {
                i+=2;
                token += '*';
                while (true) {
                    if (regex_match(token, blockCommentPattern)) {
                        lexemes.push_back(make_pair(token, COMMENT));
                        break;
                    }
                    if (i >= code.length()) {
                        lexemes.push_back(make_pair(token, ERROR));
                        break;
                    }
                    token += code[i++];
                }                 

            }else lexemes.push_back(make_pair(token, OPERATOR));

            continue;
        }
        // Preprocessors
        if (c == '#') {
            token = '#';
            i++;
            while (i < code.length() && isalpha(code[i])) {
                token += code[i];
                i++;
            }
            if (contains(preprocessorDirectives, preprocessorDirectives.size(), token)) lexemes.push_back(make_pair(token, PREPROCESSOR));
            else lexemes.push_back(make_pair(token, ERROR));

            continue;
        }
       
        
        
        // IDs, starting with '_'
        if (c == '_') {
            token = '_';
            i++;
            while (i < code.length() && regex_match(token+code[i], idPattern)) {
                token += code[i];
                i++;
            }
            if (i < code.length()) i--;
            lexemes.push_back(make_pair(token, IDENTIFIER));
            continue;

        }
            
        // reserved | id 
        if (isalpha(c)) {
            token.clear();
            token = c;
            i++;
            while (i < code.length() &&(isalnum(code[i]) || code[i] == '_')) {
                token += code[i];
                i++;
            }
            --i;
          
            if (contains(reservedWords, reservedWords.size(), token)) {
                lexemes.push_back(make_pair(token, RESERVED_WORD));
            }
            else lexemes.push_back(make_pair(token, IDENTIFIER));
                
            continue;
        }

        //operators
        if (regex_match(string(1,c), oneSymbolOperatorPattern)) {
           
            if (i+1 < code.length() &&
                contains(operators, operators.size(), string(1,c) + code[i+1])) {
                lexemes.push_back(make_pair(string(1, c) + code[i + 1], OPERATOR));
                ++i;
                continue;
            }
            lexemes.push_back(make_pair(string(1, c), OPERATOR));
            continue;
        }

        lexemes.push_back(make_pair(string(1,c), ERROR));
    }

    return lexemes;
}


int main()
{
   
    const char* InputFilename = "code.txt";
    FILE* fileInput;
    fopen_s(&fileInput, InputFilename, "r");
    std::string content; 
    
    char ch; 

    while ((ch = fgetc(fileInput)) != EOF) { 
        content += ch; 
    }
    vector <pair<string, int>> lexemsRegex = analizeRegExp(content);

    cout << content << '\n';
    size_t max_length = 0;
    for (const auto& token : lexemsRegex) {
        max_length = max(max_length, token.first.length());
    }
    const int width = static_cast<int>(max_length + 2);
    cout << "Analis result:\n";
    for (pair<string, int> a : lexemsRegex) {
        string type;
        while (a.second) {
            if (a.second >= NUMBER) {
                a.second ^= NUMBER;
                if (type.size()) type += " | NUMBER";
                else type = "NUMBER";
            } else if (a.second >= STRING_CONST) {
                a.second ^= STRING_CONST;
                if (type.size()) type += " | STRING_CONST";
                else type = "STRING_CONST";
            }
            else if (a.second >= CHAR_CONST) {
                a.second ^= CHAR_CONST;
                if (type.size()) type += " | CHAR_CONST";
                else type = "CHAR_CONST";
            }
            else if (a.second >= PREPROCESSOR) {
                a.second ^= PREPROCESSOR;
                if (type.size()) type += " | PREPROCESSOR";
                else type = "PREPROCESSOR";
            }
            else if (a.second >= COMMENT) {
                a.second ^= COMMENT;
                if (type.size()) type += " | COMMENT";
                else type = "COMMENT";
            }
            else if (a.second >= RESERVED_WORD) {
                a.second ^= RESERVED_WORD;
                if (type.size()) type += " | RESERVED_WORD";
                else type = "RESERVED_WORD";
            }
            else if (a.second >= OPERATOR) {
                a.second ^= OPERATOR;
                if (type.size()) type += " | OPERATOR";
                else type = "OPERATOR";
            }
            else if (a.second >= PUNCTUATION) {
                a.second ^= PUNCTUATION;
                if (type.size()) type += " | PUNCTUATION";
                else type = "PUNCTUATION";
            }
            else if (a.second >= IDENTIFIER) {
                a.second ^= IDENTIFIER;
                if (type.size()) type += " | IDENTIFIER";
                else type = "IDENTIFIER";
            }
        }
        if (!type.length()) type = "ERROR";
        cout << left << setw(width) << a.first << "   ||\t " << type << '\n';
    }
    
    fclose(fileInput);
   
}

