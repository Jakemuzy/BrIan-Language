#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "DataStructures/Structs.h"
#include "DataStructures/Pair.h"
#include "DataStructures/Queue.h"
#include "FileStream.h"

#include "DataStructures/Map.h"

typedef enum { 
    KEYWORD, IDENTIFIER,
    INTEGRAL, DECIMAL, HEX, BIN, 
    OPERATOR, SEPARATOR, 
    CHAR_LITERAL, STRING_LITERAL,
    COMMENT, ERR
} TokenState;
/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/


/* Helper Functions 
Token KeywordOrIdent(char* word)
{
    for(i = 0; i < KeywordMap.size(); i++)
    {
        Token kw = KeywordMap.At(i);
        if(word == kw)
        {
            return kw;
        }
    }
    return IDENTIFIER;
}
*/

/* Main loop */
int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    Queue Tokens = MakeQueue();
    
    /* Open code file to read */
    FILE* file;
    file = fopen(argv[1], "r");
    if(!file)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }

    Pair p;
    int i = 0;
    char* j = "hey";
    p = MakePair(ANY(i, C_INT), ANY(j, C_STR));

    /* Iterate through each character */
    int charCount = 0;
    char c;

    Token curTok;
    curTok.type = IDENTIFIER;
    curTok.lexeme = "";
    curTok.line = charCount;

    TokenState state = IDENTIFIER;
    while((c = fgetc(file)) != EOF)
    {
         
        /* Finish Token */
        if(isspace(c))
        {

        }

        /* = and == */
        if(c == '=')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = EQ;
            }
            else if(next == '=')
            {
                state = EQQ;
            }
            else 
            {
                ungetc(next, file);
            }
        }

        /* +, -, /, *, **, %, and EQQ versions */
        if(c == '+')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = PLUS;
            }
            else if(next == '=')
            {
                state = PEQ;
            } 
            else if(next == '+')
            {
                state = PP;
            }
            else 
            {
                ungetc(next, file);
            }
        }
        if(c == '-')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = MINUS;
            }
            else if(next == '=')
            {
                state = SEQ;
            }
            else if(next == '-')
            {
                state = SS;
            }
            else 
            {
                ungetc(next, file);
            }
        }
        if(c == '/')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = DIV;
            }
            else if(next == '=')
            {
                state = DEQ;
            }
            else 
            {
                ungetc(next, file);
            }
        }
        if(c == '*')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = MULT;
            }
            else if(next == '=')
            {
                state = MEQ;
            }
            else if(next == '*')
            {
                state = POW;
            }
            else 
            {
                ungetc(next, file);
            }
        }
        if(c == '%')
        {
            char next = fgetc(file);
            if(isspace(next))
            {
                state = MOD;
            }
            else if(next == '=')
            {
                state = MODEQ;
            }
            else 
            {
                ungetc(next, file);
            }
        }

        /* Comments */
        if(c == '/')
        {
            char next = fgetc(file);
            if(next == '*')
            {
                state = COMMENT;
                while((next = fgetc(file)) != '*')
                {
                    if(next == EOF)
                    {
                        state = ERR;
                        break;
                    }
                }

                if((next = fgetc(file)) == '/')
                    break;
                else 
                    ungetc(next, file);
            }
            else if (next == '/')
            {
                /* Add EOF check */
                while((next = fgetc(file)) != '\n')
                    ;
            }
            else 
            {
                ungetc(next, file);
            }
        }

        /* Ident or Keyword */

        /* Numbers */
        if(isdigit(c))
        {
            state = INTEGRAL;
            while(state == INTEGRAL)
            {
                char next = fgetc(file);
                if(isdigit(next))
                    ;
                else if (next == '.')
                    state = DECIMAL;
                else if (isspace(next))
                {
                    ungetc(next, file);
                    break;
                }
                else if (isalpha(next))
                {
                    state = IDENTIFIER;
                    ungetc(next, file);
                    break;
                } 
                else 
                    ;
            }

            while(state == DECIMAL)
            {
                char next = fgetc(file);
                if(isdigit(next))
                    ;
                else if (isspace(next))
                {
                    ungetc(next, file);
                    break;
                }
                else if (isalpha(next))
                {
                    state = ERR;
                    break;
                }
                else
                {
                    ungetc(next, file);
                    break;
                }
            }

        }

        /* Literals */
        if(c == '\'')
        {
            char next = fgetc(file);
            if(next != EOF);
            {
                next = fgetc(file);
                if(next != '\'')
                {
                    state = ERR;
                    break;
                }
            }
        }
        if(c == '\"')
        {
            char next;
            while((next = fgetc(file) != '\"'))
            {
                if(next == EOF)
                {
                    state = ERR;
                    printf("ERROR: EOF reached before string literal ended\n");
                    break;
                }
            }
        }
        
        charCount++;
    }
    
    FreeQueue(&Tokens);
	return 0;
}
