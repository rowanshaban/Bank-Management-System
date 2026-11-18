#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define MAX_ACCOUNTS 1000
#define MAX_DEPOSIT 10000

#define B_RED     "\033[1;31m"
#define B_GREEN   "\033[1;32m"
#define B_BLUE    "\033[1;34m"
#define B_WHITE   "\033[1;37m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define BLUE    "\033[0;34m"
#define WHITE   "\033[0;37m"
#define RESET   "\033[0m"

typedef struct
{
    int day;
    int month;
    int year;
} date;

typedef struct
{
    char num[20];
    char name[100];
    char mobile[12];
    char email[100];
    double balance;
    date dateopened;
    char status[100];
    double daily_withdrawn;
    int last_withdraw_day;
    int last_withdraw_year;
} acc;


extern acc accounts[MAX_ACCOUNTS];
extern int totalaccounts;
extern char acc_num_in[100];

#endif
