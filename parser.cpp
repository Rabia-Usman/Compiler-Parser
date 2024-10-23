#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <map>

using namespace std;

// Define token types
enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR,
    T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, T_FOR, T_WHILE,
    T_BREAK, T_CONTINUE, T_SWITCH, T_CASE, T_DEFAULT,
    T_CIN, T_COUT,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF, 
    T_COLON // Added for case statements
};

// Token structure
struct Token {
    TokenType type;
    string value;
    int line; // Line number of the token
};

// Lexer class for tokenization
class Lexer {
private:
    string src;
    size_t pos;
    int line; // Line number tracker 

public:
    Lexer(const string &src) : src(src), pos(0), line(1) {} // Initialize members in constructor

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];
            
            if (isspace(current)) {
                if (current == '\n') line++; // Increment line on new line
                pos++;
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }
            if (current == '"') {
                tokens.push_back(Token{T_STRING, consumeString(), line});
                continue;
            }
            if (current == '\'') {
                tokens.push_back(Token{T_CHAR, consumeChar(), line});
                continue;
                }
            if (isalpha(current) || current == '_') {
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word, line});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                else if (word == "if") tokens.push_back(Token{T_IF, word, line});
                else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                else if (word == "for") tokens.push_back(Token{T_FOR, word, line});
                else if (word == "while") tokens.push_back(Token{T_WHILE, word, line});
                else if (word == "break") tokens.push_back(Token{T_BREAK, word, line});
                else if (word == "continue") tokens.push_back(Token{T_CONTINUE, word, line});
                else if (word == "switch") tokens.push_back(Token{T_SWITCH, word, line});
                else if (word == "case") tokens.push_back(Token{T_CASE, word, line});
                else if (word == "default") tokens.push_back(Token{T_DEFAULT, word, line});
                else if (word == "cin") tokens.push_back(Token{T_CIN, word, line});
                else if (word == "cout") tokens.push_back(Token{T_COUT, word, line});
                else tokens.push_back(Token{T_ID, word, line});
                continue;
            }
            
            switch (current) {
                case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
                case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;  
                case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;  
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                case '>': tokens.push_back(Token{T_GT, ">", line}); break;
                case ':': tokens.push_back(Token{T_COLON, ":", line}); break; // Added for case statements
                default: cout << "Unexpected character at line " << line << ": " << current << endl; exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    }

    string consumeNumber() {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos])) pos++;
        if (src[pos] == '.') { // Handle float/double
            pos++;
            while (pos < src.size() && isdigit(src[pos])) pos++;
        }
        return src.substr(start, pos - start);
    }

    string consumeString() {
        size_t start = pos + 1; // Skip the opening quote
        pos++;
        while (pos < src.size() && src[pos] != '"') {
            pos++;
        }
        if (src[pos] != '"') {
            cout << "Error: Unclosed string literal at line " << line << endl;
            exit(1);
        }
        pos++; // Skip the closing quote
        return src.substr(start, pos - start - 1);
    }

    // Add this new method to the Lexer class
    string consumeChar() {
        size_t start = pos + 1; // Skip the opening quote
        pos++;
        if (src[pos] == '\\') { // Handle escape sequences
            pos++; // Move past the backslash
            if (src[pos] == 'n' || src[pos] == 't' || src[pos] == 'r' || src[pos] == '\'' || src[pos] == '\\') {
                pos++;
            } else {
                cout << "Error: Invalid escape sequence at line " << line << endl;
                exit(1);
            }
        } else {
            pos++; // Move past the character
        }

        if (src[pos] != '\'') { // Expecting closing single quote
            cout << "Error: Unclosed character literal at line " << line << endl;
            exit(1);
        }
        pos++; // Skip the closing quote
        return src.substr(start, pos - start - 1); // Return the character literal
    }

    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && (isalnum(src[pos]) || src[pos] == '_')) pos++;
        return src.substr(start, pos - start);
    }
};

// Parser class for parsing tokens
class Parser {
public:
    Parser(const vector<Token> &tokens) : tokens(tokens), pos(0) {} // Initialize members in constructor

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || 
            tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING || 
            tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            if (tokens[pos + 1].type == T_ASSIGN) {
                parseAssignment();
            } else if (tokens[pos].value == "cin") {
                parseCin();
            } else if (tokens[pos].value == "cout") {
                parseCout();
            } else {
                cout << "Syntax error at line " << tokens[pos].line << ": unexpected token " << tokens[pos].value << endl;
                exit(1);
            }
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_FOR) {
            parseForStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_BREAK) {
            expect(T_BREAK);
            expect(T_SEMICOLON);
        } else if (tokens[pos].type == T_CONTINUE) {
            expect(T_CONTINUE);
            expect(T_SEMICOLON);
        } else if (tokens[pos].type == T_SWITCH) {
            parseSwitchStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
            parseBlock();
        } else {
            cout << "Syntax error at line " << tokens[pos].line << ": unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);  
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);  
    }

    void parseCin() {
        expect(T_CIN);
        expect(T_ID); // Assuming we're only inputting variable names for simplicity
        expect(T_SEMICOLON);
    }

    void parseCout() {
        expect(T_COUT);
        expect(T_ID); // Assuming we're only outputting variable names for simplicity
        expect(T_SEMICOLON);
    }
    
    void parseForStatement() {
        expect(T_FOR);
        expect(T_LPAREN);
        parseStatement(); // Initialization
        parseExpression(); // Condition
        parseStatement(); // Increment
        expect(T_RPAREN);
        parseStatement(); // Loop body
    }

    void parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression(); // Condition
        expect(T_RPAREN);
        parseStatement(); // Loop body
    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression(); // Condition
        expect(T_RPAREN);
        parseStatement(); // Body
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement(); // Else body
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression(); // Value to return
        expect(T_SEMICOLON);
    }

    void parseSwitchStatement() {
        expect(T_SWITCH);
        expect(T_LPAREN);
        parseExpression(); // Value to switch on
        expect(T_RPAREN);
        expect(T_LBRACE);
        while (tokens[pos].type == T_CASE || tokens[pos].type == T_DEFAULT) {
            if (tokens[pos].type == T_CASE) {
                expect(T_CASE);
                parseExpression(); // Value of case
                expect(T_COLON); // Expect colon
                parseStatement(); // Body of case
            } else if (tokens[pos].type == T_DEFAULT) {
                expect(T_DEFAULT);
                expect(T_COLON); // Expect colon
                parseStatement(); // Body of default
            }
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        TokenType type = tokens[pos].type;
        expect(type); // Expect data type
        expect(T_ID); // Variable name
        expect(T_SEMICOLON); // End of declaration
    }

    void parseAssignment() {
        expect(T_ID); // Variable name
        expect(T_ASSIGN); // Assignment operator
        parseExpression(); // Value to assign
        expect(T_SEMICOLON); // End of assignment
    }

    void parseExpression() {
        // A simplified expression parser
        if (tokens[pos].type == T_ID || tokens[pos].type == T_NUM) {
            expect(tokens[pos].type); // Variable or number
        } else {
            cout << "Syntax error at line " << tokens[pos].line << ": unexpected token in expression " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void expect(TokenType expected) {
        if (tokens[pos].type != expected) {
            cout << "Syntax error at line " << tokens[pos].line << ": expected token of type " << expected << ", but got " << tokens[pos].type << endl;
            exit(1);
        }
        pos++;
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source-file>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    string src((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Lexer lexer(src);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}

