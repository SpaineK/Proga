#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>

using namespace std;

enum TokenType {
    TYPE,       
    IDENTIFIER, 
    LPAREN,     
    RPAREN,     
    COMMA,      
    SEMICOLON,  
    VOID,       
    END         
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
private:
    string input;
    size_t position;
    
public:
    Lexer(const string& str) : input(str), position(0) {}
    
    void skipWhitespace() {
        while (position < input.length() && isspace(input[position])) {
            position++;
        }
    }
    
    Token getNextToken() {
        skipWhitespace();
        
        if (position >= input.length()) {
            return {END, ""};
        }
        
        char current = input[position];
        
        if (current == '(') {
            position++;
            return {LPAREN, "("};
        }
        if (current == ')') {
            position++;
            return {RPAREN, ")"};
        }
        if (current == ',') {
            position++;
            return {COMMA, ","};
        }
        if (current == ';') {
            position++;
            return {SEMICOLON, ";"};
        }
        
        if (isalpha(current) || current == '_') {
            return readIdentifier();
        }
        
        return {END, ""};
    }
    
private:
    Token readIdentifier() {
        string result;
        while (position < input.length() && 
               (isalnum(input[position]) || input[position] == '_')) {
            result += input[position];
            position++;
        }
        
        if (result == "void") {
            return {VOID, result};
        }
        
        if (result == "int" || result == "double" || result == "float" || 
            result == "char" || result == "bool" || result == "string" ||
            result == "long" || result == "short" || result == "unsigned") {
            return {TYPE, result};
        }
        
        return {IDENTIFIER, result};
    }
};

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    
public:
    Parser(const string& input) : lexer(input) {
        currentToken = lexer.getNextToken();
    }
    
    bool parseFunctionHeader() {
        try {
            if (!parseType()) {
                return false;
            }
            
            if (currentToken.type != IDENTIFIER) {
                return false;
            }
            eat(IDENTIFIER);
            
            if (currentToken.type != LPAREN) {
                return false;
            }
            eat(LPAREN);
            
            parseParameterList();
            
            if (currentToken.type != RPAREN) {
                return false;
            }
            eat(RPAREN);
            
            if (currentToken.type == SEMICOLON) {
                eat(SEMICOLON);
            }
            
            return currentToken.type == END;
        }
        catch (const exception&) {
            return false;
        }
    }
    
private:
    void eat(TokenType expectedType) {
        if (currentToken.type == expectedType) {
            currentToken = lexer.getNextToken();
        } else {
            throw runtime_error("Unexpected token");
        }
    }
    
    bool parseType() {
        if (currentToken.type == TYPE || currentToken.type == VOID) {
            eat(currentToken.type);
            
            while (currentToken.type == TYPE) {
                eat(TYPE);
            }
            
            while (currentToken.type == IDENTIFIER && currentToken.value == "*") {
                eat(IDENTIFIER);
            }
            
            return true;
        }
        return false;
    }
    
    void parseParameterList() {
        if (currentToken.type == VOID && currentToken.value == "void") {
            eat(VOID);
            return;
        }
        
        if (parseParameter()) {
            while (currentToken.type == COMMA) {
                eat(COMMA);
                if (!parseParameter()) {
                    throw runtime_error("Expected parameter after comma");
                }
            }
        }
    }
    
    bool parseParameter() {
        while (currentToken.type == IDENTIFIER && 
               (currentToken.value == "const" || currentToken.value == "static" ||
                currentToken.value == "volatile")) {
            eat(IDENTIFIER);
        }
        
        if (!parseType()) {
            return false;
        }
        
        if (currentToken.type == IDENTIFIER) {
            eat(IDENTIFIER);
            
            if (currentToken.type == LPAREN) {
                eat(LPAREN);
                if (currentToken.type == RPAREN) {
                    eat(RPAREN);
                } else {
                    return false;
                }
            }
        }
        
        return true;
    }
};

bool isValidFunctionHeader(const string& header) {
    Parser parser(header);
    return parser.parseFunctionHeader();
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Введите заголовок функции (или 'exit' для выхода):" << endl;
    
    string input;
    
    while (true) {
        cout << "> ";
        getline(cin, input);
        
        if (input == "exit" || input == "quit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        bool isValid = isValidFunctionHeader(input);
        
        cout << "Заголовок: \"" << input << "\"" << endl;
        if (isValid) {
            cout << "Корректный заголовок функции" << endl;
        } else {
            cout << "Некорректный заголовок функции" << endl;
        }
        cout << endl;
    }
    
    cout << "Программа завершена" << endl;
    return 0;
}