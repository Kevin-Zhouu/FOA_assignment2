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
#define MAX_TRACE_NUM 1000
#define MAX_EVENT_NUM 1000
#define TRACE_END 1
#define CMP_LARGER 1
#define CMP_SMALLER -1
#define CMP_EQUAL 0
#define TRUE 1
#define FALSE 0
/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned char action_t; // an action is identified by an integer

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
    int len;
} trace_t;

typedef struct
{                   // an event log is an array of distinct traces
                    //     sorted lexicographically
    trace_t **trcs; // an array of traces
    int ndtr;       // the number of distinct traces in this log
    int cpct;       // the capacity of this event log as the number
                    //     of  distinct traces it can hold
} log_t;
typedef struct
{
    trace_t *traces[MAX_TRACE_NUM]; // an array of traces
    int num_traces;                 // the number of traces in this log
} trace_list_t;
typedef struct
{
    action_t action;
    int freq;
} event_freq_t;
typedef action_t **DF_t; // a directly follows relation over actions
typedef struct
{
    int n_dis_events; // the number of distinct events
    int n_dis_traces; // the number of distinct
    int n_events;     // the number of
    int n_traces;     // the number of
    trace_t **most_freq_trc;
    int max_freq;
    int num_max_freq_trcs;
    log_t *trace_log;
} trace_stats_t;
/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
// Function Definitions
trace_list_t *read_all_traces();
int get_trace(trace_t *cur_trace);
void add_new_trace(trace_list_t *trace_list, trace_t *cur_trace, int index);
void sort_traces(trace_list_t *trace_list_t);
int trace_cmp(trace_t *A, trace_t *B);
void trace_swap(trace_list_t *trace_list, int index_A, int index_B);
trace_stats_t calc_stats(trace_list_t *trace_list);
int is_event_exist(event_freq_t *event_freq_list, int n_events,
                   action_t action);
int add_event_freq(event_freq_t *event_freq_list, int tot_events,
                   action_t action, int num_actn);
int event_cmp(const void *A, const void *B);
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
void print_all_trace(trace_list_t *trace_list);
int main(int argc, char *argv[])
{

    // freopen("test0.txt", "r", stdin);

    trace_list_t *trace_list = read_all_traces();
    sort_traces(trace_list);

    calc_stats(trace_list);
    // print_all_trace(trace_list);
    for (int i = 0; i < trace_list->num_traces; i++)
    {
        free_list(trace_list->traces[i]);
    }

    free(trace_list);
    return EXIT_SUCCESS; // remember, algorithms are fun!!!
}
/****************************************************************/

trace_list_t *read_all_traces()
{

    // printf("reading:\n");
    int cur_code;
    trace_t *cur_trace = make_empty_list();
    trace_list_t *trace_list = (trace_list_t *)malloc(sizeof(*trace_list));
    int cur_trace_index = 0;
    while ((cur_code = get_trace(cur_trace)) && cur_trace_index <
                                                    MAX_TRACE_NUM)
    {
        // check if it is the end of the trace
        add_new_trace(trace_list, cur_trace, cur_trace_index);
        cur_trace_index++;
        cur_trace = make_empty_list();
        if (cur_code == EOF)
            break;
    }
    trace_list->num_traces = cur_trace_index;
    return trace_list;

    /* ok, first character of next word has been found */
}
int get_trace(trace_t *cur_trace)
{
    char cur_char;
    while ((cur_char = getchar()) != EOF)
    {
        if (isalnum(cur_char))
        {
            insert_at_foot(cur_trace, (action_t)cur_char);
        }
        if (cur_char == '\n')
            return TRACE_END;
    }
    return EOF;
}

void add_new_trace(trace_list_t *trace_list, trace_t *cur_trace, int index)
{
    assert(cur_trace != NULL);
    assert(trace_list != NULL);
    trace_list->traces[index] = cur_trace;
}
void sort_traces(trace_list_t *trace_list)
{
    assert(trace_list != NULL);
    for (int i = 1; i < trace_list->num_traces; i++)
    {
        int j = i;
        while (j > 0 &&
               (trace_cmp(trace_list->traces[j - 1],
                          trace_list->traces[j]) == CMP_LARGER))

        {
            trace_swap(trace_list, j - 1, j);
            j--;
        }
    }
}

int trace_cmp(trace_t *trc_A, trace_t *trc_B)
{
    assert(trc_A != NULL && trc_B != NULL);
    event_t *cur_event_A;
    event_t *cur_event_B;
    cur_event_A = trc_A->head;
    cur_event_B = trc_B->head;
    while (cur_event_A->next != NULL && cur_event_B->next != NULL)
    {
        if (cur_event_A->actn == cur_event_B->actn)
        {
            cur_event_A = cur_event_A->next;
            cur_event_B = cur_event_B->next;
        }
        else
        {
            if (cur_event_A->actn < cur_event_B->actn)
                return -1;
            if (cur_event_A->actn > cur_event_B->actn)
                return 1;
        }
    }
    if (cur_event_A->next == NULL && cur_event_B->next != NULL)
        return -1;
    else if (cur_event_A->next != NULL && cur_event_B->next == NULL)
        return 1;
    return 0;
}
int event_cmp(const void *A, const void *B)
{
    assert(A != NULL && B != NULL);
    action_t letter_A = ((event_freq_t *)A)->action;
    action_t letter_B = ((event_freq_t *)B)->action;
    int result = (int)letter_A - (int)letter_B;
    return result;
};
void trace_swap(trace_list_t *trace_list, int index_A, int index_B)
{
    assert(trace_list != NULL);
    trace_t *trc_tmp = trace_list->traces[index_A];
    trace_list->traces[index_A] = trace_list->traces[index_B];
    trace_list->traces[index_B] = trc_tmp;
}
trace_stats_t calc_stats(trace_list_t *trace_list)
{
    trace_stats_t stats = {0, 0, 0, 0, NULL, 0, 0, NULL};
    stats.n_traces = trace_list->num_traces;
    stats.most_freq_trc = (trace_t **)malloc(sizeof(trace_t *) *
                                             stats.n_traces);
    log_t log = {};
    log.trcs = (trace_t **)malloc(sizeof(trace_t *) * MAX_TRACE_NUM);
    log.ndtr = 0;
    log.cpct = MAX_TRACE_NUM;
    int log_index = 0;
    int most_freq_trc_index = 0;
    trace_t *prev_trace = trace_list->traces[0];
    trace_t *cur_log_trace;
    // calculate distinct traces
    for (int i = 0; i < trace_list->num_traces; i++)
    {
        trace_t *cur_trace = trace_list->traces[i];
        if (i == 0)
        {
            stats.n_dis_traces = 1;
            log.trcs[0] = cur_trace;
            cur_log_trace = log.trcs[0];
        }
        // printf("comparing:");
        // print_trace(prev_trace);
        // print_trace(cur_trace);
        // printf("\n");
        int is_same = (trace_cmp(cur_trace, prev_trace) == 0);
        if (is_same != TRUE)
        {
            log_index++;
            log.trcs[log_index] = cur_trace;
            cur_log_trace = log.trcs[log_index];
            // printf("Not same!\n");
            // print_trace(prev_trace);
            // print_trace(cur_trace);
            stats.n_dis_traces += 1;

            // printf("cur_freq: %d\n", cur_log_trace->freq);
        }
        cur_log_trace->freq++;
        stats.n_events += cur_trace->len;
        if (cur_log_trace->freq > stats.max_freq)
        {
            stats.max_freq = cur_log_trace->freq;
            most_freq_trc_index = 0;
            stats.most_freq_trc[most_freq_trc_index] = cur_trace;
        }
        if (cur_log_trace->freq == stats.max_freq)
        {
            if (stats.most_freq_trc[most_freq_trc_index] != cur_trace)
            {
                most_freq_trc_index++;
                stats.most_freq_trc[most_freq_trc_index] = cur_trace;
            }
        }
        log.ndtr = stats.n_dis_traces;
        prev_trace = cur_trace;
    }
    stats.num_max_freq_trcs = most_freq_trc_index + 1;
    // Calculate distinct events
    event_freq_t event_freq[MAX_EVENT_NUM];
    int event_freq_index = 0;
    for (int i = 0; i < stats.n_dis_traces; i++)
    {
        trace_t *cur_trace = log.trcs[i];
        event_t *cur_event = cur_trace->head;
        // printf("looping:");
        // print_trace(cur_trace);
        while (cur_event != NULL)
        {
            action_t cur_action = cur_event->actn;
            if (is_event_exist(event_freq, event_freq_index + 1, cur_action) == FALSE)
            {
                event_freq[event_freq_index].action = cur_action;
                event_freq[event_freq_index].freq = cur_trace->freq;
                event_freq_index++;
            }
            else
            {
                add_event_freq(event_freq, event_freq_index + 1, cur_action, cur_trace->freq);
            }

            cur_event = cur_event->next;
        }
    }
    event_cmp(event_freq, event_freq + 1);

    stats.n_dis_events = event_freq_index;
    qsort(event_freq, stats.n_dis_events, sizeof(event_freq_t), event_cmp);
    // printf("size:%d", sizeof(event_freq_t));
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", stats.n_dis_events);
    printf("Number of distinct traces: %d\n", stats.n_dis_traces);
    printf("Total number of events: %d\n", stats.n_events);
    printf("Total number of traces: %d\n", stats.n_traces);
    printf("Most frequent trace frequency: %d\n", stats.max_freq);
    for (int i = 0; i < stats.num_max_freq_trcs; i++)
    {
        // print_trace(stats.most_freq_trc[i]);
    }
    for (int i = 0; i < event_freq_index; i++)
    {
        printf("%c = %d\n", event_freq[i].action, event_freq[i].freq);
    }

    return stats;
}
int is_event_exist(event_freq_t *event_freq_list, int tot_events,
                   action_t action)
{
    for (int i = 0; i < tot_events; i++)
    {
        if (event_freq_list[i].action == action)
            return TRUE;
    }
    return FALSE;
}
int add_event_freq(event_freq_t *event_freq_list, int tot_events,
                   action_t action, int num_actn)
{
    assert(event_freq_list != NULL);
    for (int i = 0; i < tot_events; i++)
    {
        if (event_freq_list[i].action == action)
        {
            event_freq_list[i].freq += num_actn;
            return TRUE;
        }
    }
    // not in the array, add to the last element
    event_freq_list[tot_events].action = action;
    event_freq_list[tot_events].freq = num_actn;
    return FALSE;
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
    list->len = 0;
    list->freq = 0;
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
    list->len += 1;
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
    list->len += 1;
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
        printf("%c", cur_event->actn);
        cur_event = cur_event->next;
    }
    printf("\n");
}
void print_all_trace(trace_list_t *trace_list)
{
    printf("---------------------\n");
    for (int i = 0; i < trace_list->num_traces; i++)
    {
        print_trace(trace_list->traces[i]);
    }
    printf("---------------------\n");
}