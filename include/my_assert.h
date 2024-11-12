#ifndef MY_ASSERTS_H
#define MY_ASSERTS_H
 
#undef assert
#undef __assert 


#include "Global.h"


#if DEBUGGING_IS_ON
#define ENABLE_ASSERTIONS 1
#else
#define ENABLE_ASSERTIONS 0
#endif



#if 1


 
void assert_init(void (*assert_indicator)(void));
#if ENABLE_ASSERTIONS==1
void assertion_failure(char* expr, char* file, uint16_t linenum);
#define assert(expr) \
    if (expr) ; \
    else assertion_failure(#expr,__FILE__,__LINE__);
#else
#define assert(expr) /*nothing*/
#endif // ENABLE_ASSERTIONS==1
 
 
 
 
#else
 

 
void assert_init(void (*assert_indicator)(void));
#if ENABLE_ASSERTIONS==1
void assertion_failure(char* expr, char* file, int linenum);
#define m_assert(expr) \
    if (expr) ; \
    else assertion_failure(#expr,__FILE__,__LINE__);
#else
#define m_assert(expr) /*nothing*/
#endif // ENABLE_ASSERTIONS==1
 
 
 
#endif 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#endif // MY_ASSERTS_H_


