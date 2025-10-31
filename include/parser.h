#ifndef PARSER_H
#define PARSER_H

/**
 * @brief Inserts explicit concatenation characters '.' into a regex string.
 * This makes parsing unambiguous. e.g., "ab" -> "a.b", "(a|b)c" -> "(a|b).c"
 * @param regex The input regular expression string.
 * @param outputBuffer The buffer to store the pre-processed string.
 * @param bufferSize The size of the output buffer.
 */
void preprocess_regex(const char* regex, char* outputBuffer, int bufferSize);

/**
 * @brief Converts an infix regular expression to postfix (Reverse Polish Notation).
 * This uses the Shunting-Yard algorithm.
 * @param infix The pre-processed infix regex string.
 * @param postfix The buffer to store the resulting postfix string.
 * @param bufferSize The size of the postfix buffer.
 * @return 0 on success, -1 on failure (e.g., buffer too small).
 */
int regex_to_postfix(const char* infix, char* postfix, int bufferSize);

#endif // PARSER_H