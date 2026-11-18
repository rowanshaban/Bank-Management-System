#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "definitions.h"
#include "helpers.h"

int get_current_day_of_year()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_yday;
}

int get_current_year()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900;
}

void clean(char *str)
{
    char *newline = strchr(str, '\n');
    if (newline != NULL)
    {
        *newline = '\0';
    }
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

int is_float(char *str)
{
    if (!str || str[0] == '\0') return 0;
    int dot = 0, d = 0;
    for (int i = 0; str[i]; i++)
    {
        if (isdigit(str[i])) d = 1;
        else if (str[i] == '.')
        {
            dot++;
            if (dot > 1) return 0;
        }
        else
        {
            d = 0;
            break;
        }
    }
    return d;
}

int is_int(char *str)
{
    if (!str || str[0] == '\0') return 0;
    int i = 0;
    for (; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

int is_valid_username(char *s)
{
    if (strlen(s) < 3) return 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] < 'A' || (s[i] > 'Z' && s[i] < 'a') || s[i] > 'z') return 0;
    }
    return 1;
}

int is_valid_password(char *s)
{
    if (strlen(s) < 4) return 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == ' ') return 0;
    }
    return 1;
}

int is_valid_email(const char *email)
{
    int at_count = 0;
    int dot_after_at = 0;
    const char *at_ptr = NULL;
    if (email == NULL || email[0] == '\0') return 0;

    for (int i = 0; email[i] != '\0'; i++)
    {
        if (email[i] == '@')
        {
            at_count++;
            at_ptr = &email[i];
        }
    }

    if (at_count != 1) return 0;
    for (const char *p = at_ptr; *p != '\0'; p++)
    {
        if (*p == '.')
        {
            dot_after_at = 1;
            break;
        }
    }
    if (!dot_after_at) return 0;
    return 1;
}

int account_active(int n)
{
    return strcasecmp(accounts[n].status, "Active") == 0;
}

int account_number_exists(char *s)
{
    int index = -1;
    for(int i = 0; i < totalaccounts; i++)
    {
        if(strcmp(accounts[i].num, s) == 0)
        {
            index = i;
            break;
        }
    }
    return index;
}

int validate_int(const char *str)
{
    int i = 0;
    if (str[0] == '\0') return -1;
    while (str[i] != '\0')
    {
        if (!isdigit((unsigned char)str[i])) return -1;
        i++;
    }
    return atoi(str);
}

int contains_ignore_case(const char *haystack, const char *needle)
{
    if (!haystack || !needle) return 0;
    int h_len = strlen(haystack);
    int n_len = strlen(needle);
    if (n_len == 0) return 1;
    if (n_len > h_len) return 0;
    char *h_lower = (char*)malloc(h_len + 1);
    char *n_lower = (char*)malloc(n_len + 1);
    if (!h_lower || !n_lower)
    {
        free(h_lower);
        free(n_lower);
        return 0;
    }
    strcpy(h_lower, haystack);
    strcpy(n_lower, needle);
    for(int i = 0; i < h_len; i++) h_lower[i] = tolower(h_lower[i]);
    for(int i = 0; i < n_len; i++) n_lower[i] = tolower(n_lower[i]);
    int result = strstr(h_lower, n_lower) != NULL;
    free(h_lower);
    free(n_lower);
    return result;
}

int get_account_input(char *acc_num_out)
{
    int index;
    while (1)
    {
        printf(B_BLUE "Enter account number (10 digits): " RESET);
        fgets(acc_num_out, 20, stdin);
        clean(acc_num_out);
        index = account_number_exists(acc_num_out);
        if (index != -1) return index;
        printf(B_RED "Error: Invalid Account Number. Try again.\n" RESET);
    }
}

int check_status_valid(int index)
{
    if (!account_active(index))
    {
        printf(B_RED "Error: Account Inactive. Cannot proceed with Transaction.\n" RESET);
        return 0;
    }
    return 1;
}

double prompt_amount(const char *message)
{
    char entryTemp[20];
    double amount;
    while(1)
    {
        printf(B_BLUE "%s" RESET, message);
        fgets(entryTemp, sizeof(entryTemp), stdin);
        clean(entryTemp);
        if(is_float(entryTemp))
        {
            amount = strtod(entryTemp, NULL);
            if (amount <= 0)
            {
                printf(B_RED "Error: Amount must be greater than 0.\n" RESET);
                continue;
            }
            return amount;
        }
        else
        {
            printf(B_RED "Error: Invalid Input. Try again\n" RESET);
        }
    }
}

int check_daily_limit(int index, double amount)
{
    int today = get_current_day_of_year();
    int current_year = get_current_year();
    if (accounts[index].last_withdraw_day != today || accounts[index].last_withdraw_year != current_year)
    {
        accounts[index].daily_withdrawn = 0;
        accounts[index].last_withdraw_day = today;
        accounts[index].last_withdraw_year = current_year;
    }
    if (accounts[index].daily_withdrawn + amount > 50000)
    {
        printf(B_RED "Error: Daily withdrawal limit reached.\n" RESET);
        printf(B_RED "You have withdrawn $%.2lf today. Limit is $50,000.\n" RESET, accounts[index].daily_withdrawn);
        return 0;
    }
    return 1;
}

void update_history(char *acc_num, char *action, double amount, double balance, char *extra_details)
{
    char filename[30];
    snprintf(filename, sizeof(filename), "%s.txt", acc_num);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date_str[50];
    strftime(date_str, sizeof(date_str), "%d/%m/%Y - %I:%M%p", &tm);
    FILE* f = fopen(filename, "a");
    if(f != NULL)
    {
        fprintf(f, "%s $%.2lf%s, BALANCE $%.2lf, on %s\n", action, amount, extra_details, balance, date_str);
        fclose(f);
    }
    else
    {
        printf(B_RED "Warning: Could not save transaction history\n" RESET);
    }
}
