#ifndef _OPERATORY_H_
#define _OPERATORY_H_

extern in_addr & zero(in_addr &); 
extern bool operator==(const in_addr, const in_addr); 
extern bool operator!=(const in_addr, const in_addr);
extern ssize_t read_while(int, void *, size_t);
extern void debuger(const char * arg1, ...);

#endif
