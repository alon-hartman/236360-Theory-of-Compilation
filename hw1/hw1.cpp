#include <array>
#include <cstring>
#include <iostream>
#include <sstream>

#include "tokens.hpp"

using namespace std;

int main()
{
    int token;
    // clang-format off
    array<const char *, ENUM_SIZE> token_to_string = {"", "VOID", "INT", "BYTE", "B", "BOOL", "AND", "OR", "NOT",
                                                      "TRUE", "FALSE", "RETURN", "IF", "ELSE", "WHILE", "BREAK",
                                                      "CONTINUE", "SC", "COMMA", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
                                                      "ASSIGN", "RELOP", "BINOP", "COMMENT", "ID", "NUM", "STRING",
                                                      "ERROR", "UNRECOGNIZED_CHAR", "UNCLOSED_STRING", "HEX_LENGTH_0",
                                                      "HEX_LENGTH_1", "ESCAPE_ERROR"};
    // clang-format on
    while ((token = yylex()))
    {
        switch (token)
        {
        case COMMENT:
            cout << yylineno << " " << token_to_string[token] << " //" << endl;
            break;
        case STRING: {
            stringstream buffer;
            buffer << yylineno << " " << token_to_string[token] << " ";
            string input_text(yytext);
            for (int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    if (i == input_text.length() - 2)
                    {
                        cout << "Error unclosed string" << endl;
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
                                throw invalid_argument("");
                            }
                        }
                        catch (invalid_argument)
                        {
                            cout << "Error undefined escape sequence " << input_text.substr(i, 3) << endl;
                            exit(0);
                        }
                        buffer << (char)std::stoi(input_text.substr(i + 1, 2), nullptr, 16);
                        i += 2;
                        break;
                    case 't':
                        buffer << '\t';
                        break;
                    case 'n':
                        buffer << endl;
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
                        cout << "Error undefined escape sequence " << input_text[i] << endl;
                        exit(0);
                    }
                }
                else
                {
                    buffer << input_text[i];
                }
            }
            cout << buffer.str() << endl;
            break;
        }
        case UNRECOGNIZED_CHAR:
            cout << "Error " << yytext[strlen(yytext) - 1] << endl;
            exit(0);
        case UNCLOSED_STRING:
            cout << "Error unclosed string" << endl;
            exit(0);
        case ESCAPE_ERROR: {
            string input_text = yytext;
            for (int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    i++;
                    if (input_text[i] != 'n' || input_text[i] != 't' || input_text[i] != '0' || input_text[i] != 'r' ||
                        input_text[i] != '\\' || input_text[i] != '\"')
                    {
                        cout << "Error undefined escape sequence " << input_text[i] << endl;
                        exit(0);
                    }
                }
            }
            exit(0);
        }
        case HEX_LENGTH_0:
            cout << "Error undefined escape sequence x" << endl;
            exit(0);
        case HEX_LENGTH_1:
            cout << "Error undefined escape sequence x" << yytext[strlen(yytext) - 2] << endl;
            exit(0);
        case ERROR:
            cout << "Error " << yytext << endl;
            exit(0);
        default:
            cout << yylineno << " " << token_to_string[token] << " " << yytext << endl;
            break;
        }
    }
    return 0;
}
