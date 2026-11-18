#ifndef HELPERS_H
#define HELPERS_H


int get_current_day_of_year();
int get_current_year();


void clean(char *str);
int is_float(char *str);
int is_int(char *str);
int validate_int(const char *str);
int is_valid_username(char *s);
int is_valid_password(char *s);
int is_valid_email(const char *email);
int contains_ignore_case(const char *haystack, const char *needle);


int account_active(int n);
int account_number_exists(char *s);
int get_account_input(char *acc_num_out);
int check_status_valid(int index);
double prompt_amount(const char *message);
int check_daily_limit(int index, double amount);
void update_history(char *acc_num, char *action, double amount, double balance, char *extra_details);

#endif
