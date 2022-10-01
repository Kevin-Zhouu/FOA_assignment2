
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *string_dupe(char *s);
char **string_set_dupe(char **s);
int main(int argc, char *argv[])
{
    char *str_set[] = {"Hello", "World", "Gays"};
    char **new_str = string_set_dupe(str_set);
    printf("first st:new_str:%s", *new_str);

    return 0;
}
char *string_dupe(char *s)
{
    char *new_str = (char *)malloc(strlen(s) * sizeof(char));
    printf("strlen(s)*sizeof(char):%d", strlen(s) * sizeof(char));
    printf("sizeof(s):%d", sizeof(s));
    strcpy(new_str, s);
    return new_str;
}
char **string_set_dupe(char **s)
{
    char **new_str_set = (char **)malloc(sizeof(*s));
    int i = 0;
    while (*s != NULL)
    {
        *(new_str_set + i) = string_dupe(*s);
        s++;
    }
    return new_str_set;
}