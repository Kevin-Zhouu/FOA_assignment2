/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Shiyuan Zhou
  Dated:     1 Oct 2022

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define GOOD_LUCK "GOOD LUCK CLASS!!!\n" // good luck message

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned int action_t; // an action is identified by an integer

typedef struct event event_t; // an event ...
struct event
{                  // ... is composed of ...
    action_t actn; // ... an action that triggered it and ...
    event_t *next; // ... a pointer to the next event in the trace
};

typedef struct
{                  // a trace is a linked list of events
    event_t *head; // a pointer to the first event in this trace
    event_t *foot; // a pointer to the last event in this trace
    int freq;      // the number of times this trace was observed
} trace_t;

typedef struct
{                  // an event log is an array of distinct traces
                   //     sorted lexicographically
    trace_t *trcs; // an array of traces
    int ndtr;      // the number of distinct traces in this log
    int cpct;      // the capacity of this event log as the number
                   //     of  distinct traces it can hold
} log_t;

typedef action_t **DF_t; // a directly follows relation over actions

typedef struct
{
    int n_dis_events; // the number of distinct events
    int n_dis_traces; // the number of distinct
    int n_tot_events; // the number of
    int n_tot_traces; // the number of

} trace_stats_t;
#define MAX_TRACE_LEN 1000
#define TRACE_END 1
/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
// Function Definitions
log_t *read_all_traces();
int get_trace(trace_t *cur_trace);
int sort_trace_log(log_t *trace_log);
int trace_compare(trace_t A, trace_t B);
int trace_swap(trace_t *A, trace_t *B);
trace_stats_t calc_stats();
void print_stats(trace_stats_t trace_stats);
// Linked list operations
trace_t *make_empty_list(void);
int is_empty_list(trace_t *list);
void free_list(trace_t *list);
trace_t *insert_at_head(trace_t *list, action_t value);
trace_t *insert_at_foot(trace_t *list, action_t value);
action_t get_head(trace_t *list);
trace_t *get_tail(trace_t *list);
void print_trace(trace_t *list);

int main(int argc, char *argv[])
{

    freopen("test0.txt", "r", stdin);
    read_all_traces();
    return EXIT_SUCCESS; // remember, algorithms are fun!!!
}
/****************************************************************/

/* extract a single word out of the standard input, but not
   more than "limit" characters in total. One character of
   sensible trailing punctuation is retained.
   argument array W must be limit+1 characters or bigger
*/
log_t *read_all_traces()
{

    // printf("reading:\n");
    int cur_code;
    trace_t *cur_trace;
    log_t *trace_log;
    while ((cur_code = get_trace(cur_trace)) == TRACE_END)
    {
        // check if it is the end of the trace
        print_trace(cur_trace);
    }
    return trace_log;

    /* ok, first character of next word has been found */
}
int get_trace(trace_t *cur_trace)
{
    trace_t *new_trace = make_empty_list();
    char cur_char;
    while ((cur_char = getchar()) != EOF)
    {
        if (isalnum(cur_char))
        {
            printf("%c", cur_char);
            insert_at_foot(new_trace, (action_t)cur_char);
        }
        if (cur_char == '\n')
            return TRACE_END;
    }
    cur_trace = new_trace;
    return EOF;
}

/* The following codes are derived from the list operations by
 * Alistair Moffat, PPSAA, Chapter 10, December 2012
 * (c) University of Melbourne */
trace_t *
make_empty_list(void)
{
    trace_t *list = (trace_t *)malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->foot = NULL;
    return list;
}

int is_empty_list(trace_t *list)
{
    assert(list != NULL);
    return list->head == NULL;
}

void free_list(trace_t *list)
{
    assert(list != NULL);
    // free the elements of the list
    event_t *curr = list->head, *prev;
    while (curr)
    {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    // free the list itself
    free(list);
}

trace_t *
insert_at_head(trace_t *list, action_t value)
{
    event_t *new = (event_t *)malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    new->actn = value;
    new->next = list->head;
    list->head = new;
    if (list->foot == NULL)
    {
        /* this is the first insertion into the list */
        list->foot = new;
    }
    return list;
}

trace_t *
insert_at_foot(trace_t *list, action_t value)
{
    event_t *new = (event_t *)malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    new->actn = value;
    new->next = NULL;
    if (list->foot == NULL)
    {
        /* this is the first insertion into the list */
        list->head = new;
        list->foot = new;
    }
    else
    {
        list->foot->next = new;
        list->foot = new;
    }
    return list;
}

action_t
get_head(trace_t *list)
{
    assert(list != NULL && list->head != NULL);
    return list->head->actn;
}

trace_t *
get_tail(trace_t *list)
{
    assert(list != NULL && list->head != NULL);
    event_t *oldhead = list->head;
    list->head = list->head->next;
    if (list->head == NULL)
    {
        /* the only list node just got deleted */
        list->foot = NULL;
    }
    free(oldhead);
    return list;
}
void print_trace(trace_t *list)
{
    assert(list != NULL);
    event_t *cur_event;
    cur_event = list->head;
    while (cur_event != NULL)
    {
        printf(", %d", cur_event->actn);
        cur_event = cur_event->next;
    }
}