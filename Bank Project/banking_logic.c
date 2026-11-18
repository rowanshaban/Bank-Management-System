#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "definitions.h"
#include "helpers.h"

acc accounts[MAX_ACCOUNTS];
int totalaccounts = 0;
char acc_num_in[100];

void load()
{
    FILE *f = fopen("accounts.txt", "r");
    if (!f) return;
    totalaccounts = 0;
    char line[500];
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) < 5) continue;
        char *token = strtok(line, ",");
        if (!token) continue;
        strcpy(accounts[totalaccounts].num, token);
        token = strtok(NULL, ",");
        if (!token) break;
        strcpy(accounts[totalaccounts].name, token);
        token = strtok(NULL, ",");
        if (!token) break;
        strcpy(accounts[totalaccounts].email, token);
        token = strtok(NULL, ",");
        if (!token) break;
        accounts[totalaccounts].balance = atof(token);
        token = strtok(NULL, ",");
        if (!token) break;
        strcpy(accounts[totalaccounts].mobile, token);
        token = strtok(NULL, ",");
        if (!token) break;
        int m, y;
        if (sscanf(token, "%d-%d", &m, &y) == 2)
        {
            accounts[totalaccounts].dateopened.month = m;
            accounts[totalaccounts].dateopened.year = y;
            accounts[totalaccounts].dateopened.day = 1;
        }
        token = strtok(NULL, ",");
        if (!token) break;
        while(*token == ' ') token++;
        strcpy(accounts[totalaccounts].status, token);
        accounts[totalaccounts].status[strcspn(accounts[totalaccounts].status, "\n")] = 0;
        accounts[totalaccounts].daily_withdrawn = 0;
        accounts[totalaccounts].last_withdraw_day = 0;
        accounts[totalaccounts].last_withdraw_year = 0;
        totalaccounts++;
    }
    fclose(f);
}

void login(char *users)
{
    FILE *f;
    char username[100], password[100];
    char fileusername[100], filepassword[100];
    while (1)
    {
        f = fopen(users, "r");
        if (f == NULL)
        {
            printf(B_RED "Error: users.txt not found. Exiting.\n" RESET);
            exit(1);
        }
        while (1)
        {
            printf("Username: ");
            fgets(username,sizeof(username), stdin);
            clean(username);
            if (is_valid_username(username)) break;
            printf(B_RED "Invalid username format.\n" RESET);
        }
        while (1)
        {
            printf("Password: ");
            fgets(password,sizeof(password), stdin);
            clean(password);
            if (is_valid_password(password)) break;
            printf(B_RED "Invalid password format.\n" RESET);
        }
        int found = 0;
        while (fscanf(f, "%99s %99s", fileusername, filepassword) == 2)
        {
            if (strcmp(username, fileusername) == 0 && strcmp(password, filepassword) == 0)
            {
                found = 1;
                break;
            }
        }
        fclose(f);
        if (found)
        {
            printf(B_GREEN "Login successful!\n" RESET);
            return;
        }
        else
        {
            printf(B_RED "Invalid username or password. Try again.\n" RESET);
        }
    }
}

void save()
{
    FILE *f = fopen("accounts.txt", "w");
    if (!f)
    {
        printf(B_RED "Error saving file\n" RESET);
        return;
    }
    for (int i = 0; i < totalaccounts; i++)
    {
        fprintf(f, "%s,%s,%s,%.2lf,%s,%d-%d,%s\n",
                accounts[i].num, accounts[i].name, accounts[i].email,
                accounts[i].balance, accounts[i].mobile,
                accounts[i].dateopened.month, accounts[i].dateopened.year,
                accounts[i].status);
    }
    fclose(f);
}

void ask_to_save()
{
    char choice[20];
    printf("Do you want to save changes to file? (yes/no): ");
    fgets(choice, sizeof(choice), stdin);
    clean(choice);
    if (strcasecmp(choice, "yes") == 0)
    {
        save();
        printf(B_GREEN "Success: Data saved to accounts.txt\n" RESET);
    }
    else
    {
        printf("Changes not saved.\n");
    }
}

void merge_name(int left, int mid, int right)
{
    int nl = mid - left + 1;
    int nr = right - mid;
    if (nl <= 0 || nr <= 0) return;
    acc l[nl], r[nr];
    for (int i = 0; i < nl; i++) l[i] = accounts[left + i];
    for (int j = 0; j < nr; j++) r[j] = accounts[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < nl && j < nr)
    {
        if (strcasecmp(l[i].name, r[j].name) <= 0)
        {
            accounts[k] = l[i];
            i++;
        }
        else
        {
            accounts[k] = r[j];
            j++;
        }
        k++;
    }
    while (i < nl)
    {
        accounts[k] = l[i];
        i++;
        k++;
    }
    while (j < nr)
    {
        accounts[k] = r[j];
        j++;
        k++;
    }
}

void merge_sort_by_name(int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        merge_sort_by_name(left, mid);
        merge_sort_by_name(mid + 1, right);
        merge_name(left, mid, right);
    }
}

void sort_by_name()
{
    merge_sort_by_name(0, totalaccounts - 1);
}

void merge_balance(int left, int mid, int right)
{
    int nl = mid - left + 1;
    int nr = right - mid;
    if (nl <= 0 || nr <= 0) return;
    acc l[nl], r[nr];
    for (int i = 0; i < nl; i++) l[i] = accounts[left + i];
    for (int j = 0; j < nr; j++) r[j] = accounts[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < nl && j < nr)
    {
        if (l[i].balance > r[j].balance)
        {
            accounts[k] = l[i];
            i++;
        }
        else
        {
            accounts[k] = r[j];
            j++;
        }
        k++;
    }
    while (i < nl)
    {
        accounts[k] = l[i];
        i++;
        k++;
    }
    while (j < nr)
    {
        accounts[k] = r[j];
        j++;
        k++;
    }
}

void merge_sort_by_balance(int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        merge_sort_by_balance(left, mid);
        merge_sort_by_balance(mid + 1, right);
        merge_balance(left, mid, right);
    }
}
void sort_by_balance()
{
    merge_sort_by_balance(0, totalaccounts - 1);
}

void merge_date(int left, int mid, int right)
{
    int nl = mid - left + 1;
    int nr = right - mid;
    if (nl <= 0 || nr <= 0) return;
    acc l[nl], r[nr];
    for (int i = 0; i < nl; i++) l[i] = accounts[left + i];
    for (int j = 0; j < nr; j++) r[j] = accounts[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < nl && j < nr)
    {
        if (l[i].dateopened.year < r[j].dateopened.year || (l[i].dateopened.year == r[j].dateopened.year && l[i].dateopened.month <= r[j].dateopened.month))
        {
            accounts[k] = l[i];
            i++;
        }
        else
        {
            accounts[k] = r[j];
            j++;
        }
        k++;
    }
    while (i < nl)
    {
        accounts[k] = l[i];
        i++;
        k++;
    }
    while (j < nr)
    {
        accounts[k] = r[j];
        j++;
        k++;
    }
}

void merge_sort_by_date(int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        merge_sort_by_date(left, mid);
        merge_sort_by_date(mid + 1, right);
        merge_date(left, mid, right);
    }
}
void sort_by_date()
{
    merge_sort_by_date(0, totalaccounts - 1);
}

void sort_by_status()
{
    acc temp[MAX_ACCOUNTS];
    int idx = 0;
    for (int i = 0; i < totalaccounts; i++)
        if (strcasecmp(accounts[i].status, "Active") == 0) temp[idx++] = accounts[i];
    for (int i = 0; i < totalaccounts; i++)
        if (strcasecmp(accounts[i].status, "Inactive") == 0) temp[idx++] = accounts[i];
    for (int i = 0; i < totalaccounts; i++) accounts[i] = temp[i];
}

void print_accounts_sorted()
{
    int choice;
    printf("Print accounts sorted by:\n");
    printf(B_BLUE "1. Name\n");
    printf("2. Balance\n");
    printf("3. Date Opened\n" RESET);
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    switch(choice)
    {
    case 1:
        sort_by_name();
        break;
    case 2:
        sort_by_balance();
        break;
    case 3:
        sort_by_date();
        break;
    default:
        printf(B_RED "Invalid choice!\n" RESET);
        return;
    }
    sort_by_status();
    printf("\n--- Accounts ---\n");
    for (int i = 0; i < totalaccounts; i++)
    {
        printf("Name: %s, Balance: %.2f, Date: %02d/%04d, Status: %s\n",
               accounts[i].name, accounts[i].balance,
               accounts[i].dateopened.month, accounts[i].dateopened.year,
               accounts[i].status);
    }
}


void add()
{
    char newnum[11], newname[100], newemail[100], newmobile[12];
    int valid;
    while (1)
    {
        printf("Enter new account number (10 digits): ");
        if (!fgets(newnum, sizeof(newnum), stdin)) continue;
        clean(newnum);
        if (strlen(newnum)!= 10)
        {
            printf(B_RED "Account number must be 10 digits\n" RESET);
            continue;
        }
        if (!is_int(newnum)) printf(B_RED "Invalid input. Digits only\n" RESET);
        else if (account_number_exists(newnum) != -1) printf(B_RED "Error. There is another account with the same number.\n" RESET);
        else break;
    }
    do
    {
        valid = 1;
        printf("Enter new name: ");
        if (!fgets(newname, sizeof(newname), stdin)) continue;
        clean(newname);
        if (strlen(newname) == 0) valid = 0;
        for (int i = 0; newname[i] && valid; i++)
            if (!isalpha((unsigned char)newname[i]) && !isspace((unsigned char)newname[i])) valid = 0;
        if (!valid) printf(B_RED "Invalid name. Try again.\n" RESET);
    }
    while (!valid);
    for (int i = 0; newname[i]; i++)
    {
        if (i == 0 || newname[i-1] == ' ') newname[i] = toupper(newname[i]);
        else newname[i] = tolower(newname[i]);
    }
    while (1)
    {
        printf("Enter new email: ");
        if (!fgets(newemail,sizeof(newemail),stdin)) continue;
        clean(newemail);
        if (!is_valid_email(newemail))
        {
            printf(B_RED "Invalid email format! Please try again.\n" RESET);
            continue;
        }
        break;
    }
    do
    {
        valid = 1;
        printf("Enter new mobile number (11 digits): ");
        if (!fgets(newmobile,sizeof(newmobile),stdin)) continue;
        clean(newmobile);
        if (strlen(newmobile) != 11) valid = 0;
        for (int i = 0; newmobile[i] && valid; i++)
            if (!isdigit((unsigned char)newmobile[i])) valid = 0;
        if (!valid) printf(B_RED "Invalid mobile number. Must be 11 digits\n" RESET);
    }
    while (!valid);
    strcpy(accounts[totalaccounts].num, newnum);
    strcpy(accounts[totalaccounts].name,newname);
    strcpy(accounts[totalaccounts].email,newemail);
    strcpy(accounts[totalaccounts].mobile,newmobile);
    accounts[totalaccounts].balance = 0.0;
    strcpy(accounts[totalaccounts].status, "active");
    ask_to_save();
}

void modify()
{
    char option[4];
    do
    {
        if(totalaccounts==0)
        {
            printf(B_RED "No accounts available.\n" RESET);
            return;
        }
        char accnumber[100];
        printf("Enter account number to modify: ");
        fgets(accnumber,sizeof(accnumber),stdin);
        accnumber[strcspn(accnumber,"\n")]=0;
        int found=0, i;
        for(i=0; i<totalaccounts; i++)
        {
            if(strcmp(accounts[i].num,accnumber)==0)
            {
                printf("Account found\n");
                char newname[100],newmobile[100],newemail[100];
                printf("Current name: %s\n",accounts[i].name);
                printf("Enter new name (Enter to keep): ");
                fgets(newname,sizeof(newname),stdin);
                newname[strcspn(newname,"\n")]=0;
                printf("Current Mobile: %s\n",accounts[i].mobile);
                printf("Enter new mobile (Enter to keep): ");
                fgets(newmobile,sizeof(newmobile),stdin);
                newmobile[strcspn(newmobile,"\n")]=0;
                printf("Current Address: %s\n",accounts[i].email);
                printf("Enter new address (Enter to keep): ");
                fgets(newemail,sizeof(newemail),stdin);
                newemail[strcspn(newemail,"\n")]=0;
                char confirm[10];
                printf("Do you want to save changes? (yes/no): ");
                fgets(confirm, sizeof(confirm), stdin);
                confirm[strcspn(confirm, "\n")] = 0;
                if (strcasecmp(confirm, "yes") == 0)
                {
                    if(strlen(newname) > 0) strcpy(accounts[i].name,newname);
                    if(strlen(newmobile) > 0) strcpy(accounts[i].mobile,newmobile);
                    if(strlen(newemail) > 0) strcpy(accounts[i].email,newemail);
                    save();
                    printf(B_GREEN "Account updated successfully\n" RESET);
                }
                else
                {
                    printf("Changes discarded.\n");
                }
                found=1;
                break;
            }
        }
        if(!found) printf(B_RED "Account isn't found\n" RESET);
        printf("Modify another? (yes/no): ");
        fgets(option,sizeof(option),stdin);
        option[strcspn(option,"\n")]=0;
    }
    while(strcmp(option,"yes")==0);
}

void search()
{
    char acc_num[20];
    printf("Enter account number to search: ");
    fgets(acc_num, sizeof(acc_num), stdin);
    acc_num[strcspn(acc_num, "\n")] = 0;
    int index = account_number_exists(acc_num);
    if (index != -1)
    {
        printf("\n--- Account Details ---\n");
        printf("Account Number: %s\nName: %s\nEmail: %s\nBalance: %.2lf\nMobile: %s\nDate Opened: %d-%02d-%02d\nStatus: %s\n\n",
               accounts[index].num, accounts[index].name, accounts[index].email,
               accounts[index].balance, accounts[index].mobile,
               accounts[index].dateopened.year, accounts[index].dateopened.month,
               accounts[index].dateopened.day, accounts[index].status);
    }
    else printf(B_RED "Account not found.\n" RESET);
}

void advanced_search()
{
    char key[100];
    printf("Enter keyword to search: ");
    scanf(" %[^\n]", key);
    int found = 0;
    for (int i = 0; i < totalaccounts; i++)
    {
        if (contains_ignore_case(accounts[i].name, key))
        {
            printf("\nAccount Number: %s\nName: %s\nEmail: %s\nBalance: %.2lf\nMobile: %s\nDate Opened: %d-%02d-%02d\nStatus: %s\n",
                   accounts[i].num, accounts[i].name, accounts[i].email,
                   accounts[i].balance, accounts[i].mobile,
                   accounts[i].dateopened.year, accounts[i].dateopened.month,
                   accounts[i].dateopened.day, accounts[i].status);
            found = 1;
        }
    }
    if (!found) printf(B_RED "No matching accounts.\n" RESET);
}

void delete_account()
{
    char target_num[20];
    printf("Enter account number to delete (10 digits): ");
    fgets(target_num, sizeof(target_num), stdin);
    clean(target_num);
    int found = 0;
    for (int i = 0; i < totalaccounts; i++)
    {
        if (strcmp(accounts[i].num, target_num) == 0)
        {
            found = 1;
            if (accounts[i].balance > 0)
            {
                printf(B_RED "Cannot delete: account balance is %.2f (must be zero).\n" RESET, accounts[i].balance);
            }
            else
            {
                for (int j = i; j < totalaccounts - 1; j++)
                {
                    accounts[j] = accounts[j + 1];
                }
                totalaccounts--;
                printf(B_GREEN "Account %s deleted successfully.\n" RESET, target_num);
            }
            return;
        }
    }
    if (!found) printf(B_RED "Account %s not found.\n" RESET, target_num);
}

void delete_multiple()
{
    char choice_str[3];
    int choice;
    printf("\n--- Delete Multiple Accounts ---\n");
    printf(B_BLUE "1. By Date\n");
    printf("2. Inactive Accounts\n" RESET);
    printf("Choice: ");
    fgets(choice_str, sizeof(choice_str), stdin);
    clean(choice_str);
    choice = validate_int(choice_str);
    switch (choice)
    {
    case 1:
    {
        int m, y, deleted = 0;
        char date_input[64];
        printf("Enter date (MM-YYYY): ");
        if (fgets(date_input, sizeof(date_input), stdin) == NULL) break;
        for (int i = 0; date_input[i] != '\n' && date_input[i] != '\0'; i++)
        {
            if (!isdigit(date_input[i]) && date_input[i] != '-')
            {
                printf(B_RED "Malicious input detected: Only digits and '-' allowed.\n" RESET);
                goto end_delete_date;
            }
        }
        if (sscanf(date_input, "%d-%d", &m, &y) != 2)
        {
            printf(B_RED "Invalid format. Use MM-YYYY (e.g., 05-2023).\n" RESET);
            break;
        }
        if (m < 1 || m > 12 || y < 1900 || y > 2100)
        {
            printf(B_RED "Invalid date range. Month must be 1-12 and Year 1900-2100.\n" RESET);
            break;
        }
        for (int i = 0; i < totalaccounts; i++)
        {
            if (accounts[i].dateopened.month == m && accounts[i].dateopened.year == y)
            {
                for (int j = i; j < totalaccounts - 1; j++) accounts[j] = accounts[j+1];
                totalaccounts--;
                i--;
                deleted++;
            }
        }
        printf(B_GREEN "%d accounts deleted.\n" RESET, deleted);
        printf("Operation complete.\n");
end_delete_date:
        break;
    }
    case 2:
    {
        int deleted = 0;
        time_t t = time(NULL);
        struct tm today = *localtime(&t);
        int curr_m = today.tm_mon + 1;
        int curr_y = today.tm_year + 1900;
        for (int i = 0; i < totalaccounts; i++)
        {
            int months = (curr_y - accounts[i].dateopened.year) * 12 + (curr_m - accounts[i].dateopened.month);
            if (strcmp(accounts[i].status, "inactive") == 0 && accounts[i].balance == 0 && months >= 3)
            {
                for (int j = i; j < totalaccounts - 1; j++) accounts[j] = accounts[j+1];
                totalaccounts--;
                i--;
                deleted++;
            }
        }
        printf(B_GREEN "%d inactive accounts deleted.\n" RESET, deleted);
        break;
    }
    default:
        printf(B_RED "Invalid choice.\n" RESET);
        return;
    }
}

void change_status()
{
    char target_num[20];
    printf("Enter account number to change status: ");
    fgets(target_num, sizeof(target_num), stdin);
    clean(target_num);
    for (int i = 0; i < totalaccounts; i++)
    {
        if (strcmp(accounts[i].num, target_num) == 0)
        {
            char new_status[20];
            printf("Current status: %s\n", accounts[i].status);
            printf("Enter new status (Active/Inactive): ");
            fgets(new_status, sizeof(new_status), stdin);
            clean(new_status);
            if (strcmp(new_status, "Active") == 0 || strcmp(new_status, "Inactive") == 0 ||
                    strcmp(new_status, "active") == 0 || strcmp(new_status, "inactive") == 0)
            {
                strcpy(accounts[i].status, new_status);
                printf(B_GREEN "Status updated successfully.\n" RESET);
            }
            else
            {
                printf(B_RED "Invalid status entered. No changes made.\n" RESET);
            }
            return;
        }
    }
    printf(B_RED "Account %s not found.\n" RESET, target_num);
}

void withdraw()
{
    char acc_num[20];
    int index;
    index = get_account_input(acc_num);
    if (!check_status_valid(index)) return;
    double amount = prompt_amount("Enter withdrawal amount: ");
    if(amount > 10000)
    {
        printf(B_RED "Error: Maximum withdrawal per transaction is $10,000\n" RESET);
        return;
    }
    if(accounts[index].balance < amount)
    {
        printf(B_RED "Error: Insufficient funds. Current Balance %.2lf\n" RESET, accounts[index].balance);
        return;
    }
    if (!check_daily_limit(index, amount)) return;
    accounts[index].balance = accounts[index].balance - amount;
    accounts[index].daily_withdrawn = accounts[index].daily_withdrawn + amount;
    printf(B_GREEN "Transaction Successful.\n" RESET "New Balance: $%.2lf\n", accounts[index].balance);
    update_history(acc_num, "WITHDREW", amount, accounts[index].balance, "");
    ask_to_save();
}

void deposit()
{
    char acc_num[20];
    int index;
    index = get_account_input(acc_num);
    if (!check_status_valid(index)) return;
    double amount = prompt_amount("Enter deposit amount: ");
    if (amount > 10000)
    {
        printf(B_RED "Transaction UNsuccessful \nExceeds limit of $10,000 per transaction.\n" RESET);
        return;
    }
    printf("Processing deposit ...\n");
    accounts[index].balance += amount;
    printf(B_GREEN "Transaction SUCCESSFUL." RESET " New Balance: $%.2lf\n", accounts[index].balance);
    update_history(acc_num, "DEPOSIT", amount, accounts[index].balance, "");
    ask_to_save();
}

void transfer()
{
    char s_acc_num[20], r_acc_num[20];
    int s_index, r_index;
    printf("\n--- SENDER INFO ---\n");
    s_index = get_account_input(s_acc_num);
    if (!check_status_valid(s_index)) return;
    printf("\n--- RECEIVER INFO ---\n");
    r_index = get_account_input(r_acc_num);
    if (strcmp(s_acc_num, r_acc_num) == 0)
    {
        printf(B_RED "Error: Cannot transfer to self.\n" RESET);
        return;
    }
    if (!check_status_valid(r_index)) return;
    double amount = prompt_amount("Enter transfer amount: ");
    if (accounts[s_index].balance < amount)
    {
        printf(B_RED "Error: Insufficient funds.\n" RESET);
        return;
    }
    accounts[s_index].balance = accounts[s_index].balance - amount;
    accounts[r_index].balance = accounts[r_index].balance + amount;
    printf(B_GREEN "Transaction Successful.\n" RESET "Sender New Balance: $%.2lf\nReceiver New Balance: $%.2lf\n", accounts[s_index].balance, accounts[r_index].balance);
    char to_msg[30], from_msg[30];
    snprintf(to_msg, sizeof(to_msg), " TO %s", r_acc_num);
    snprintf(from_msg, sizeof(from_msg), " FROM %s", s_acc_num);
    update_history(s_acc_num, "TRANSFERRED", amount, accounts[s_index].balance, to_msg);
    update_history(r_acc_num, "RECEIVED", amount, accounts[r_index].balance, from_msg);
    ask_to_save();
}

void report()
{
    char acc_num[20];
    int index;
    while (1)
    {
        printf("Enter account number to view report for (10 digits): ");
        fgets(acc_num, sizeof(acc_num), stdin);
        clean(acc_num);
        index = account_number_exists(acc_num);
        if ( index != -1) break;
        printf(B_RED "Error: Invalid Account Number. Try again.\n" RESET);
    }
    char filename[30];
    snprintf(filename, sizeof(filename), "%s.txt", acc_num);
    FILE* f = fopen(filename,"r");
    if(f == NULL)
    {
        printf(B_RED "No transaction history found.\n" RESET);
        return;
    }
    int lines = 0;
    char buffer[2048];
    while(fgets(buffer, 2048, f) != NULL) lines++;
    if (lines == 0)
    {
        printf("No transaction history yet.\n");
        fclose(f);
        return;
    }
    rewind(f);
    int current_line = 1;
    if(lines >= 5) printf("--- Last 5 Transactions ---\n");
    else printf("--- Last %d Transactions ---\n", lines);
    while(fgets(buffer, 2048, f) != NULL)
    {
        if(current_line > lines - 5) printf("%s", buffer);
        current_line++;
    }
    printf("---------------------------\n");
    fclose(f);
}

void quit()
{
    printf(B_GREEN "Exiting...\n" RESET);
    exit(0);
}

void menu()
{
    char option[10];
    int logged = 0;
    while (!logged)
    {
        printf(B_BLUE "\n--- WELCOME ---\n");
        printf("1. LOGIN\n");
        printf("2. QUIT\n" RESET);
        printf("---------------\n");
        printf("Choose an option: ");
        fgets(option, sizeof(option), stdin);
        clean(option);
        if (strcmp(option, "1") == 0)
        {
            login("users.txt");
            logged = 1;
        }
        else if (strcmp(option, "2") == 0)
        {
            quit();
        }
        else
        {
            printf(B_RED "Invalid option. Please try again.\n" RESET);
        }
    }
    printf("\nLoading data...\n");
    load();
    int running = 1;
    while (running)
    {
        printf(B_BLUE "\n--- MAIN MENU ---\n");
        printf("1.  ADD\n");
        printf("2.  DELETE (Single)\n");
        printf("3.  DELETE MULTIPLE\n");
        printf("4.  MODIFY\n");
        printf("5.  SEARCH\n");
        printf("6.  ADVANCED SEARCH\n");
        printf("7.  CHANGE STATUS\n");
        printf("8.  WITHDRAW\n");
        printf("9.  DEPOSIT\n");
        printf("10. TRANSFER\n");
        printf("11. REPORT\n");
        printf("12. PRINT\n");
        printf("13. QUIT\n" RESET);
        printf("-----------------\n");
        printf("Choose an option: ");
        fgets(option, sizeof(option), stdin);
        clean(option);
        int valid_op = 1;
        if (strcmp(option, "1") == 0) add();
        else if (strcmp(option, "2") == 0) delete_account();
        else if (strcmp(option, "3") == 0) delete_multiple();
        else if (strcmp(option, "4") == 0) modify();
        else if (strcmp(option, "5") == 0) search();
        else if (strcmp(option, "6") == 0) advanced_search();
        else if (strcmp(option, "7") == 0) change_status();
        else if (strcmp(option, "8") == 0) withdraw();
        else if (strcmp(option, "9") == 0) deposit();
        else if (strcmp(option, "10") == 0) transfer();
        else if (strcmp(option, "11") == 0) report();
        else if (strcmp(option, "12") == 0) print_accounts_sorted();
        else if (strcmp(option, "13") == 0) quit();
        else
        {
            printf(B_RED "Invalid option.\n" RESET);
            valid_op = 0;
        }
        if (running && valid_op)
        {
            char sub_opt[10];
            printf("\nOperation Complete.\n");
            printf(B_BLUE "1. Return to Menu\n");
            printf("0. Exit System\n" RESET);
            printf("Select: ");
            fgets(sub_opt, sizeof(sub_opt), stdin);
            clean(sub_opt);
            if (strcmp(sub_opt, "0") == 0) quit();
        }
    }
}
