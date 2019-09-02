#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define DEBUG //uncomment for debug messages

#define INITIAL_CAPACITY 100//initial capacity for code characters
#define INITIAL_STACK_SIZE 9//initial size for token stacks

#define ERR(str) fprintf(stderr, "err: %s\n", str)

char *code_array; int c_cap = INITIAL_CAPACITY; int c_size = 0;
int x_counter = 0;
//position in code pointer and a stack for holding the code

enum {err = 0, ok = 1, eof, number, opr};//flag
enum {add = 1, subtract = 2, multiply = 3, divide = 4, power = 5, pright, pleft};//value
//tokenization enums

const char precedence[] = {0,1,1,2,2,3};//precedence look up table (based on the enum)

typedef struct
{
    int value;
    char flag;
} token;

char peek(int offset)
{
    return code_array[x_counter + offset];
}
//peek ahead of x_counter

int b_isnumber(char input)
{
    if(input >= '0' && input <= '9') {return 1;}
    return 0;
}
//check if input is a digit

int b_isoperator(char input)
{
    if(input == '+' || input == '-' || input == '*' || input == '/' || input == '^') {return 1;}
    return 0;
}

token parse_int()
{
    token temp = {0,0};
    int digit_amount = 0;
    int num = 0;
    char ngt = 0;//is negative

    if(peek(0) == '-'){x_counter++; ngt = 1;}
    if(!b_isnumber(peek(0))){return temp;}

    for (int i = 0; b_isnumber(peek(i)); i++)
    {
        digit_amount++;
    }
    for (int i = digit_amount-1; i >= 0; i--)
    {
        num += (code_array[x_counter] - '0') * pow(10, i);
        x_counter++;
    }
    if(ngt){num *= -1;}
    
    temp.flag = number;
    temp.value = num;
    return temp;
}
//parse a textual number into an int  (supports the '-'s sign)

token parse_op()
{
    token temp = {0,0};

    if(b_isoperator(peek(0)) || peek(0 == '(' || peek(0) == ')'))
    {
        int val;
        
        switch (peek(0))
        {
        case '+':
            val = add;
            break;
        case '-':
            val = subtract;
            break;
        case '*':
            val = multiply;
            break;
        case '/':
            val = divide;
            break;
        case '^':
            val = power;
            break;
        case '(':
            val = pleft;
            break;
        case ')':
            val = pright;
        default:
            break;
        }
        x_counter++;
        temp.flag = opr;
        temp.value = val;
        return temp;
    }
    return temp;
}

token next_token()
{
    token temp = {0,0};
    enum {base_s, pleft_s, num_s, op_s, pright_s, eof_s};//state enum
    
    static char state = base_s;

    switch (state)
    {
    case base_s:
        if(peek(0) == '(')
        {
            temp = parse_op();
            state = pleft_s;
        }
        else if(b_isnumber(peek(0)) || (b_isnumber(peek(1)) && peek(0) == '-'))
        {
            temp = parse_int();
            state = num_s;
        }
        else if(peek(0) == 'q' || peek(0) == '\33')
        {
            puts("Quitting...");
            exit(0);
        }
        break;
    case pleft_s:
        if(peek(0) == '(')
        {
            temp = parse_op();
            state = pleft_s;
        }
        else if (b_isnumber(peek(0)) || (b_isnumber(peek(1)) && peek(0) == '-'))
        {
            temp = parse_int();
            state = num_s;
        }
        break;
    case num_s:
        if(b_isoperator(peek(0)))
        {
            temp = parse_op();
            state = op_s;
        }
        else if(peek(0) == ')')
        {
            temp = parse_op();
            state = pright_s;
        }
        else if(peek(0) == '\n')
        {
            temp.flag = eof;
        }
        break;
    case op_s:
        if(peek(0) == '(')
        {
            temp = parse_op();
            state = pleft_s;
        }
        else if(b_isnumber(peek(0)) || (b_isnumber(peek(1)) && peek(0) == '-'))
        {
            temp = parse_int();
            state = num_s;
        }
        break;
    case pright_s:
        if(peek(0) == ')')
        {
            temp = parse_op();
            state = pright_s;
        }
        else if(b_isoperator(peek(0)))
        {
            temp = parse_op();
            state = op_s;
        }
        else if(peek(0) == '\n')
        {
            temp.flag = eof;
        }
        break;
    default:
        break;
    }
    //lexical analysis FSM

    if(temp.flag == err || temp.flag == eof)
    {
        free(code_array);
        c_size = 0;
        c_cap = INITIAL_CAPACITY;
        //free code arr
        state = base_s;
    }

    
    return temp;
}
//return next token

#define CHECK_STACK_CAP \
do \
{   \
    if(out_size >= stack_capacity || op_size >= stack_capacity) \
    {   \
        stack_capacity *=2; \
        out_stack = realloc(out_stack, sizeof(token) * stack_capacity);\
        op_stack = realloc(op_stack, sizeof(token) * stack_capacity);\
    }\
} while (0);

int evaluate()
{
    token temp = {0,0};
    int result = 0;
    char exit = ok;

    token *op_stack; int op_size = 0;
    token *out_stack;int out_size = 0;
    int stack_capacity = INITIAL_STACK_SIZE;
    op_stack = malloc(sizeof(token) * INITIAL_STACK_SIZE);
    out_stack = malloc(sizeof(token) * INITIAL_STACK_SIZE);
    //alloc stacks for shunting yard algorithm

    x_counter = 0;

    while(exit == ok)
    {
        temp = next_token();
        switch (temp.flag)
        {
        case err:
            ERR("Syntax error");
            exit = err;
            break;
        case eof:
            exit = eof;
            break;
        case number:
            out_size++;
            CHECK_STACK_CAP;
            out_stack[out_size -1] = temp;
            //push to out_stack
            break;
        case opr:
            if (temp.value == pleft)
            {
                op_size++;
                CHECK_STACK_CAP;
                op_stack[op_size -1] = temp;
                //push to op_stack
            }
            else if (temp.value == pright)
            {
                while(op_stack[op_size -1].value != pleft)
                {
                    out_size++;
                    CHECK_STACK_CAP;
                    out_stack[out_size -1] = op_stack[op_size -1];
                    op_size--;
                    //pop op to out stack
                    if(op_size < 1)
                    {
                        exit = err;
                        ERR("parenthesis mismatch");
                        break;
                    }
                }
                op_size--;//get rid of the pleft
            }
            else
            {
                while(((precedence[op_stack[op_size -1].value] > precedence[temp.value]) || (precedence[op_stack[op_size -1].value] == precedence[temp.value] && temp.value == power))
                 && (op_stack[op_size -1].value != pleft) && op_size > 0)
                {
                    out_size++;
                    CHECK_STACK_CAP;
                    out_stack[out_size -1] = op_stack[op_size -1];
                    op_size--;
                    //pop op stack to out
                }
                op_size++;
                CHECK_STACK_CAP;
                op_stack[op_size -1] = temp;
                //push the token on op stack
            }
            
            break;
        default:
            ERR("invalid token flag");
            break;
        }

    }

    if(exit == eof)//should not be anything else
    {
        while(op_size > 0)
        {
            out_size++;
            CHECK_STACK_CAP
            out_stack[out_size -1] = op_stack[op_size -1];
            op_size--;
        }
        //empty op_stack

        #ifdef DEBUG
        for (int i = 0; i < out_size; i++)
        {
            printf("token# flag: %d value: %d\n", out_stack[i].flag, out_stack[i].value);
        }//to list sorted tokens
        #endif

        #define SHIFT_STACK\
        for (int j = i; j < out_size; j++)\
        {\
            out_stack[j-2] = out_stack[j];\
        }\
        out_size -= 2;\
        i -= 2;

        for (int i = 0; i < out_size; i++)
        {
            if(out_stack[i].flag == opr)
            {
                switch (out_stack[i].value)
                {
                case add:
                    out_stack[i].value = out_stack[i-2].value + out_stack[i-1].value;
                    SHIFT_STACK;
                    break;
                case subtract:
                    out_stack[i].value = out_stack[i-2].value - out_stack[i-1].value;
                    SHIFT_STACK;
                    break;
                case multiply:
                    out_stack[i].value = out_stack[i-2].value * out_stack[i-1].value;
                    SHIFT_STACK;
                    break;
                case divide:
                    if(out_stack[i-1].value == 0)
                    {
                        ERR("zero division error -> result undefined");
                        break;
                    }
                    out_stack[i].value = out_stack[i-2].value / out_stack[i-1].value;
                    SHIFT_STACK;
                    break;
                case power:
                    out_stack[i].value = pow(out_stack[i-2].value, out_stack[i-1].value);
                    SHIFT_STACK;
                    break;
                default:
                    break;
                }
            }
        }
        result = out_stack[out_size -1].value;
    }
    else {putchar('?');}

    free(op_stack);
    free(out_stack);
    op_size = 0;
    out_size = 0;
    //free the stacks

    return result;
}
//return expression result

void get_line()
{
    putchar('>');
    for (int i = 0; 1 ; i++)
    {
        char c = getchar();
        if (c_size >= c_cap)
        {
            c_cap*= 2;
            code_array = realloc(code_array, sizeof(char) * c_cap);
            //expand code stack if over capacity
        }
        if(c == '\40')
        {
            i--;
        }
        else if (c != '\n')
        {
            code_array[i] = c;
            c_size++;
        }
        else//is \n
        {
            code_array[i] = '\n';
            c_size++;
            return;
        } 
    }
}

void mainloop()
{
    while(1)
    {
        code_array = malloc(sizeof(char) * INITIAL_CAPACITY);
        get_line();
        printf("result: %d\n", evaluate());
        puts("------------------------------");
    }
}

int main(int argc, char const *argv[])
{
    
    puts("\nSimple Expr. calc v1.0");
    puts("made by Jarek");
    puts("type a mathematical expression");
    puts("or \"quit\"");
    puts("------------------------------");
    mainloop();    
    
    return 0;
}
