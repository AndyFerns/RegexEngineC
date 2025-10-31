#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Helper function to get the precedence of an operator
static int precedence(char op) {
    switch (op) {
        case '|': return 1; // Union
        case '.': return 2; // Concatenation
        case '*': return 3; // Kleene Star
    }
    return 0; // Other characters (operands)
}

void preprocess_regex(const char* regex, char* outputBuffer, int bufferSize) {
    int j = 0;
    for (int i = 0; regex[i] != '\0'; i++) {
        // Ensure we don't overflow the buffer
        if (j >= bufferSize - 2) return;

        // Copy the current character
        outputBuffer[j++] = regex[i];

        // Check the current and next characters to see if a '.' should be inserted
        char current = regex[i];
        char next = regex[i + 1];

        if (next == '\0') continue;

        // Conditions for inserting a '.'
        // 1. Current is an operand and next is an operand: ab -> a.b
        // 2. Current is a ')' and next is an operand: (a)b -> (a).b
        // 3. Current is a '*' and next is an operand: a*b -> a*.b
        // 4. Current is an operand and next is a '(': a(b) -> a.(b)
        // 5. Current is ')' and next is '(': (a)(b) -> (a).(b)
        // 6. Current is '*' and next is '(': a*(b) -> a*.(b)
        if ((isalnum(current) || current == ')' || current == '*') &&
            (isalnum(next) || next == '(')) {
            outputBuffer[j++] = '.';
        }
    }
    outputBuffer[j] = '\0'; // Null-terminate the string
}

int regex_to_postfix(const char* infix, char* postfix, int bufferSize) {
    char operator_stack[1024]; // A simple stack for operators
    int stack_top = -1;
    int postfix_idx = 0;

    for (int i = 0; infix[i] != '\0'; i++) {
        char token = infix[i];

        if (isalnum(token)) {
            // If the token is an operand, add it to the output
            if (postfix_idx >= bufferSize - 1) return -1;
            postfix[postfix_idx++] = token;
        } else if (token == '(') {
            // If it's a '(', push it onto the operator stack
            operator_stack[++stack_top] = token;
        } else if (token == ')') {
            // If it's a ')', pop operators until '(' is found
            while (stack_top > -1 && operator_stack[stack_top] != '(') {
                if (postfix_idx >= bufferSize - 1) return -1;
                postfix[postfix_idx++] = operator_stack[stack_top--];
            }
            stack_top--; // Pop the '('
        } else {
            // It's an operator
            while (stack_top > -1 && precedence(operator_stack[stack_top]) >= precedence(token)) {
                if (postfix_idx >= bufferSize - 1) return -1;
                postfix[postfix_idx++] = operator_stack[stack_top--];
            }
            operator_stack[++stack_top] = token;
        }
    }

    // Pop any remaining operators from the stack to the output
    while (stack_top > -1) {
        if (postfix_idx >= bufferSize - 1) return -1;
        postfix[postfix_idx++] = operator_stack[stack_top--];
    }

    postfix[postfix_idx] = '\0'; // Null-terminate the postfix string
    return 0;
}