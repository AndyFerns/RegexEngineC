#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "nfa.h"

/**
 * @brief Simulates an NFA against a given string
 * 
 * @param nfa The NFA (Start State) to emulate
 * 
 * @returns 1 (true) if the string is accepted
 * 
 * @returns 0 (false) if the string is rejected
 */
int simulate_nfa(Nfa* nfa, const char* str);

#endif