#include "tokens.hpp"
#include <array>
#include <cstring>
#include <iostream>
#include <sstream>

int main()
{
    int token;
    std::array<const char *, ENUM_SIZE> token_to_string = {
        "",         "VOID",  "INT",     "BYTE",   "B",      "BOOL",   "AND",    "OR",
        "NOT",      "TRUE",  "FALSE",   "RETURN", "IF",     "ELSE",   "WHILE",  "BREAK",
        "CONTINUE", "SC",    "COMMA",   "LPAREN", "RPAREN", "LBRACE", "RBRACE", "ASSIGN",
        "RELOP",    "BINOP", "COMMENT", "ID",     "NUM",    "STRING", "ERROR",  "UNRECOGNIZED_CHAR"};
    while ((token = yylex()))
    {
        switch (token)
        {
        case COMMENT:
            std::cout << yylineno << " " << token_to_string[token] << " //" << std::endl;
            break;
        case STRING: {
            std::stringstream buffer;
            buffer << yylineno << " " << token_to_string[token] << " ";
            std::string input_text(yytext);
            for (int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    if (i == input_text.length() - 2)
                    {
                        std::cout << "Error unclosed string" << std::endl;
                        exit(0);
                    }
                    i++;
                    switch (input_text[i])
                    {
                    case '\\':
                        buffer << '\\';
                        break;
                    case 'x':
                        try
                        {
                            int c1 = (input_text.substr(i + 1, 1)).c_str()[0];
                            int c2 = (input_text.substr(i + 2, 1)).c_str()[0];
                            if (!((('0' <= c1 && c1 <= '9') || ('A' <= c1 && c1 <= 'F')) &&
                                  (('0' <= c2 && c2 <= '9') || ('A' <= c2 && c2 <= 'F'))))
                            {
                                throw std::invalid_argument("");
                            }
                        }
                        catch (std::invalid_argument)
                        {
                            std::cout << "Error undefined escape sequence " << input_text.substr(i, 3) << std::endl;
                            exit(0);
                        }
                        buffer << (char)std::stoi(input_text.substr(i + 1, 2), nullptr, 16);
                        i += 2;
                        break;
                    case 't':
                        buffer << '\t';
                        break;
                    case 'n':
                        buffer << std::endl;
                        break;
                    case '0':
                        buffer << '\0';
                        break;
                    case 'r':
                        buffer << '\r';
                        break;
                    case '"':
                        buffer << '"';
                        break;
                    default:
                        std::cout << "Error undefined escape sequence " << input_text[i] << std::endl;
                        exit(0);
                    }
                }
                else
                {
                    buffer << input_text[i];
                }
            }
            std::cout << buffer.str() << std::endl;
            break;
        }
        case UNRECOGNIZED_CHAR:
            std::cout << "Error " << yytext[std::strlen(yytext) - 1] << std::endl;
            exit(0);
        case UNCLOSED_STRING:
            std::cout << "Error unclosed string" << std::endl;
            exit(0);
        case ESCAPE_ERROR: {
            std::string input_text = yytext;
            for (int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    i++;
                    if (input_text[i] != 'n' || input_text[i] != 't' || input_text[i] != '0' || input_text[i] != 'r' ||
                        input_text[i] != '\\' || input_text[i] != '\"')
                    {
                        std::cout << "Error undefined escape sequence " << input_text[i] << std::endl;
                        exit(0);
                    }
                }
            }
            exit(0);
        }
        case HEX_ERROR: {
            std::string input_text = yytext;
            for (int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    i++;
                    if (input_text[i] == 'x')
                    {
                        try
                        {
                            std::stoi(input_text.substr(i + 1, 1), nullptr, 16);
                            std::stoi(input_text.substr(i + 2, 1), nullptr, 16);
                        }
                        catch (std::invalid_argument &)
                        {
                            std::cout << "Error undefined escape sequence " << input_text.substr(i, 3) << std::endl;
                            exit(0);
                        }
                    }
                }
            }
            exit(0);
        }
        case HEX_LENGTH_0:
            std::cout << "Error undefined escape sequence x" << std::endl;
            exit(0);
        case HEX_LENGTH_1:
            std::cout << "Error undefined escape sequence x" << yytext[strlen(yytext) - 2] << std::endl;
            exit(0);
        case ERROR:
            std::cout << "Error " << yytext << std::endl;
            exit(0);
        default:
            std::cout << yylineno << " " << token_to_string[token] << " " << yytext << std::endl;
            break;
        }
    }
    return 0;
}
