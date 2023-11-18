#ifndef UTILS
#define UTILS

// Structs to send data via RPL ports
typedef struct Data
{
    char nodeID[10];
    char temp[10];
    char hum[10];
    char photo[10];
    char solar[10];
} Data;

#endif