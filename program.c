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
#define DECIMAL_TO_PERCENT 100
#define WEIGHT_C 50
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
    trace_t **traces; // an array of traces
    int num_traces;   // the number of traces in this log
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
    trace_list_t *trace_list;
    trace_t **most_freq_trc;
    int max_freq;
    int num_max_freq_trcs;
    event_freq_t *event_freq;
    log_t *log;
} trace_stats_t;
typedef struct
{
    action_t x;
    action_t y;
    int freq;
} sup_t;
typedef struct
{
    int **values;      // two dimensional array of int
    action_t *rows;    // action for corresponidng rows
    action_t *columns; // action for corresponding columns
    int n_rows;        // number of rows
    int n_columns;     // number of columns
} sup_matrix_t;
typedef struct
{
    sup_t *sup;
    int pd;
    int w;
} candidate_t;
typedef struct
{
    candidate_t *candidates;
    int num;
} candidate_list_t;
typedef struct
{
    sup_t seq;
    action_t code;
    sup_matrix_t *sup_matrix;
} stg2_stats_t;

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
// Function Definitions
trace_list_t *read_all_traces();
int get_trace(trace_t *cur_trace);
void add_new_trace(trace_list_t *trace_list, trace_t *cur_trace, int index);
void sort_traces(trace_list_t *trace_list_t);
int trace_cmp(trace_t *A, trace_t *B);
void trace_swap(trace_list_t *trace_list, int index_A, int index_B);
trace_stats_t calc_stg_0(trace_list_t *trace_list);
trace_stats_t *calc_trc_stats(trace_stats_t *stats);
trace_stats_t *calc_evt_stats(trace_stats_t *stats);
int is_event_exist(event_freq_t *event_freq_list, int n_events,
                   action_t action);
int add_event_freq(event_freq_t *event_freq_list, int tot_events,
                   action_t action, int num_actn);
int event_cmp(const void *A, const void *B);
void print_stg_0(trace_stats_t *stats);

void calc_stg_1(trace_stats_t *stats);
int **init_matrix(int rows, int columns);
sup_matrix_t *generate_seq_matrix(trace_list_t *log, trace_stats_t *stats);
candidate_list_t *find_potential_seq(sup_matrix_t *sup_matrix);
stg2_stats_t *del_seq(trace_stats_t *stats, candidate_list_t *can_list,
                      sup_matrix_t *sup_matrix);
void print_matrix(sup_matrix_t *sup_matrix);
int calc_pd(sup_t *xy, sup_t *yx);
int calc_w(sup_t *xy, sup_t *yx, int pd);
int find_row_index(action_t action, action_t *rows, int total_tows);
void print_stg2(stg2_stats_t *);
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
int max(int x, int y);
int main(int argc, char *argv[])
{

    // freopen("test0.txt", "r", stdin);

    trace_list_t *trace_list = read_all_traces();
    sort_traces(trace_list);

    trace_stats_t stats = calc_stg_0(trace_list);
    calc_stg_1(&stats);
    // print_all_trace(trace_list);
    for (int i = 0; i < trace_list->num_traces; i++)
    {
        free_list(trace_list->traces[i]);
    }

    free(trace_list);
    return EXIT_SUCCESS; // remember, algorithms are fun!!!
}
/***************************************************************
this function reads all traces from stdin and stores them in heap,
returning the pointer of type trace_list_t
*/
trace_list_t *read_all_traces()
{

    // printf("reading:\n");
    int cur_code;
    trace_t *cur_trace = make_empty_list();
    trace_list_t *trace_list = (trace_list_t *)malloc(sizeof(*trace_list));
    int cur_trace_index = 0;
    // get trace from stdin if number of trace exceeds maximum
    while (cur_trace_index < MAX_TRACE_NUM)
    {
        // save new trace to trace_list
        cur_code = get_trace(cur_trace);
        add_new_trace(trace_list, cur_trace, cur_trace_index);
        cur_trace_index++;
        if (cur_code == EOF)
            break; // reached end of file
        // initialise empty linked list for next round
        cur_trace = make_empty_list();
    }
    trace_list->num_traces = cur_trace_index;
    return trace_list;
}
/***************************************************************
this function reads all characters from stdin and add the trace to 'cur_trace'
return TRACE_END if trace is found
return EOF if reached end of file
*/
int get_trace(trace_t *cur_trace)
{
    char cur_char;
    // check if reached end of file
    while ((cur_char = getchar()) != EOF)
    {
        // skip all non-alphanumeric characters
        if (isalnum(cur_char))
        {
            // found a character, add to trace
            insert_at_foot(cur_trace, (action_t)cur_char);
        }
        if (cur_char == '\n')
            return TRACE_END; // end of trace found
    }
    return EOF;
}

void add_new_trace(trace_list_t *trace_list, trace_t *cur_trace, int index)
{
    assert(cur_trace != NULL);
    assert(trace_list != NULL);
    if (index == 0)
    {
        trace_list->traces = (trace_t **)malloc(sizeof(trace_t *));
    }
    else
    {
        trace_list->traces = (trace_t **)realloc(
            trace_list->traces, sizeof(trace_t *) * (index + 1));
    }
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
trace_stats_t calc_stg_0(trace_list_t *trace_list)
{
    trace_stats_t stats = {0, 0, 0, 0, NULL, NULL, 0, 0, NULL};
    stats.n_traces = trace_list->num_traces;
    stats.most_freq_trc = (trace_t **)malloc(sizeof(trace_t *) *
                                             stats.n_traces);
    stats.trace_list = trace_list;
    calc_trc_stats(&stats);
    calc_evt_stats(&stats);
    print_stg_0(&stats);

    return stats;
}
trace_stats_t *calc_trc_stats(trace_stats_t *stats)
{
    assert(stats != NULL);
    trace_list_t *trace_list = stats->trace_list; // an array of sorted trcs
    // intialize log to store distinct traces
    log_t *log = (log_t *)malloc(sizeof(log_t));
    log->trcs = (trace_t **)malloc(sizeof(trace_t *) * MAX_TRACE_NUM);
    log->ndtr = 0;
    log->cpct = MAX_TRACE_NUM;
    int log_index = 0;
    int most_freq_trc_index = 0;
    trace_t *prev_trace = trace_list->traces[0];
    trace_t *cur_log_trace;
    // go throught the sorted trace list
    for (int i = 0; i < trace_list->num_traces; i++)
    {
        trace_t *cur_trace = trace_list->traces[i];
        // initialise variables when i=0
        if (i == 0)
        {
            stats->n_dis_traces = 1;
            log->trcs[0] = cur_trace;
            cur_log_trace = log->trcs[0];
        }
        // determine whether current trace is same as previous trace
        int is_same = (trace_cmp(cur_trace, prev_trace) == 0);
        if (is_same != TRUE)
        {
            // if not same, add to log
            log_index++;
            log->trcs[log_index] = cur_trace;
            cur_log_trace = log->trcs[log_index];
            stats->n_dis_traces += 1;
        }
        // update stats
        cur_log_trace->freq++;
        stats->n_events += cur_trace->len;
        // check if current trace is most frequent
        if (cur_log_trace->freq > stats->max_freq)
        {
            // it is most frequent, update stats
            stats->max_freq = cur_log_trace->freq;
            most_freq_trc_index = 0;
            stats->most_freq_trc[most_freq_trc_index] = cur_trace;
        }
        // check if current trace has same frequency as max frequency
        if (cur_log_trace->freq == stats->max_freq)
        {
            // check if current trace is already in most_freq_trc array
            if (stats->most_freq_trc[most_freq_trc_index] != cur_trace)
            {
                // add current trace to most_freq_trc array
                most_freq_trc_index++;
                stats->most_freq_trc[most_freq_trc_index] = cur_trace;
            }
        }
        // end of the loop, update prev_trace
        prev_trace = cur_trace;
    }
    log->ndtr = stats->n_dis_traces;
    stats->num_max_freq_trcs = most_freq_trc_index + 1;
    stats->log = log;
    return stats;
}
trace_stats_t *calc_evt_stats(trace_stats_t *stats)
{
    // Calculate distinct events
    // initialise event_freq array with maximum elements of total events
    event_freq_t *event_freq =
        (event_freq_t *)malloc(sizeof(event_freq_t) * stats->n_events);
    int event_freq_index = 0;
    for (int i = 0; i < stats->n_dis_traces; i++)
    {
        trace_t *cur_trace = stats->log->trcs[i];
        event_t *cur_event = cur_trace->head;
        while (cur_event != NULL)
        {
            action_t cur_action = cur_event->actn;
            if (is_event_exist(event_freq, event_freq_index + 1,
                               cur_action) == FALSE)
            {
                (event_freq + event_freq_index)->action = cur_action;
                (event_freq + event_freq_index)->freq = cur_trace->freq;
                event_freq_index++;
            }
            else
            {
                add_event_freq(event_freq, event_freq_index + 1, cur_action,
                               cur_trace->freq);
            }

            cur_event = cur_event->next;
        }
    }
    event_cmp(event_freq, event_freq + 1);
    stats->n_dis_events = event_freq_index;
    qsort(event_freq, stats->n_dis_events, sizeof(event_freq_t), event_cmp);
    stats->event_freq = event_freq;
    return stats;
}
void print_stg_0(trace_stats_t *stats)
{
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", stats->n_dis_events);
    printf("Number of distinct traces: %d\n", stats->n_dis_traces);
    printf("Total number of events: %d\n", stats->n_events);
    printf("Total number of traces: %d\n", stats->n_traces);
    printf("Most frequent trace frequency: %d\n", stats->max_freq);
    for (int i = 0; i < stats->num_max_freq_trcs; i++)
    {
        print_trace(stats->most_freq_trc[i]);
    }
    for (int i = 0; i < stats->n_dis_events; i++)
    {
        printf("%c = %d\n", (stats->event_freq + i)->action,
               (stats->event_freq + i)->freq);
    }
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
void calc_stg_1(trace_stats_t *stats)
{
    sup_matrix_t *sup_matrix = generate_seq_matrix(stats->trace_list, stats);
    print_matrix(sup_matrix);
    candidate_list_t *can_list = find_potential_seq(sup_matrix);
    //  del_seq(stats, can_list, sup_matrix);
}
sup_matrix_t *generate_seq_matrix(trace_list_t *log, trace_stats_t *stats)
{

    sup_matrix_t *sup_matrix = (sup_matrix_t *)malloc(sizeof(*sup_matrix));
    sup_matrix->rows = (action_t *)malloc(sizeof(action_t) *
                                          stats->n_dis_events);
    sup_matrix->columns = (action_t *)malloc(sizeof(action_t) *
                                             stats->n_dis_events);
    int row_index = 0;
    // find all rows and columns
    for (int i = 0; i < log->num_traces; i++)
    {
        trace_t *cur_trace = log->traces[i];
        event_t *cur_event = cur_trace->head;

        while (cur_event != NULL)
        {
            action_t cur_action = cur_event->actn;
            if (find_row_index(cur_action, sup_matrix->rows, row_index + 1) == -1)
            {
                sup_matrix->rows[row_index] = cur_action;
                sup_matrix->columns[row_index] = cur_action;
                row_index++;
            }
            cur_event = cur_event->next;
        }
    }
    sup_matrix->rows = (action_t *)realloc(sup_matrix->rows,
                                           sizeof(action_t) * row_index);
    sup_matrix->columns = (action_t *)realloc(sup_matrix->columns,
                                              sizeof(action_t) * row_index);
    sup_matrix->values = init_matrix(row_index, row_index);
    for (int i = 0; i < log->num_traces; i++)
    {
        trace_t *cur_trace = log->traces[i];

        event_t *prev_event = cur_trace->head;
        event_t *cur_event = cur_trace->head->next;

        while (cur_event != NULL)
        {
            int x_index = find_row_index(prev_event->actn,
                                         sup_matrix->rows, row_index);
            int y_index = find_row_index(cur_event->actn,
                                         sup_matrix->columns, row_index);
            sup_matrix->values[x_index][y_index] += 1;
            prev_event = cur_event;
            cur_event = cur_event->next;
        }
    }
    sup_matrix->n_rows = row_index;
    sup_matrix->n_columns = row_index;
    return sup_matrix;
}
void print_matrix(sup_matrix_t *sup_matrix)
{
    printf("==STAGE 1============================\n");
    printf("     ");
    for (int i = 0; i < sup_matrix->n_rows; i++)
    {
        printf("%5c", sup_matrix->rows[i]);
    }
    printf("\n");
    for (int i = 0; i < sup_matrix->n_rows; i++)
    {
        printf("%5c", sup_matrix->rows[i]);
        for (int j = 0; j < sup_matrix->n_columns; j++)
        {
            printf("%5d", sup_matrix->values[i][j]);
        }
        printf("\n");
    }
}
int **init_matrix(int rows, int columns)
{
    int **values = (int **)malloc(sizeof(int *) * rows);
    for (int i = 0; i < rows; i++)
    {
        values[i] = (int *)malloc(sizeof(int) * columns);
        for (int j = 0; j < columns; j++)
        {
            values[i][j] = 0;
        }
    }

    return values;
}
int find_row_index(action_t action, action_t *rows, int total_tows)
{
    for (int i = 0; i < total_tows; i++)
    {
        if (rows[i] == action)
            return i;
    }
    return -1;
}
candidate_list_t *find_potential_seq(sup_matrix_t *sup_matrix)
{
    candidate_list_t *can_list = (candidate_list_t *)malloc(
        sizeof(candidate_list_t));
    int n_rows = sup_matrix->n_rows;
    // looping over the rows
    for (int i = 0; i < n_rows; i++)
    {
        // looping over the columns
        for (int j = 0; j < n_rows; j++)
        {
            sup_t xy = {sup_matrix->rows[i], sup_matrix->rows[j],
                        sup_matrix->values[i][j]};
            sup_t yx = {sup_matrix->rows[j], sup_matrix->rows[i],
                        sup_matrix->values[j][i]};
            int pd = calc_pd(&xy, &yx);
            int w = calc_w(&xy, &yx, pd);
            printf("seq(%c,%c) pd=%d w=%d\n", xy.x, xy.y, pd, w);
        }
    }
}
int calc_pd(sup_t *xy, sup_t *yx)
{
    int result = (DECIMAL_TO_PERCENT * abs(xy->freq - yx->freq)) /
                 max(xy->freq, yx->freq);
    return result;
}

int calc_w(sup_t *xy, sup_t *yx, int pd)
{
    int result = abs(WEIGHT_C - pd) * max(xy->freq, yx->freq);
    return result;
}
stg2_stats_t *del_seq(trace_stats_t *stats, candidate_list_t *can_list,
                      sup_matrix_t *sup_matrix);
void print_stg2(stg2_stats_t *);
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

// Operations for maths
int max(int x, int y)
{
    if (x - y >= 0)
    {
        return x;
    }
    return y;
}