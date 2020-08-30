/********************** CREDIT CARD FRAUD DETECTOR ***************************
   *
   Written by Yunzhi Li (Student ID: 880371) for assignment 2 of 
   COMP10002 Foundations of Algorithms
   *
   This code contains original and modified functions from listops.c found in 
   "Programming, Problem Solving, and Abstraction with C" by Alistair Moffat.
   Any functions of such will be indicated in the comments
   *
   ALGORITHMS ARE FUN!!! <33333 UwU *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#define STAGE_ONE 1
#define STAGE_TWO 2
#define STAGE_THREE 3
#define STAGE_FOUR 4

#define MAX_NUM_CARDS 100
#define CARD_ID_LEN 8
#define TRANSAC_ID_LEN 12

typedef struct {
    int total_transac;
    int year;
    int mon;
    int day;
} daily_transac_t;

typedef struct {
    char card_id[CARD_ID_LEN+1];
    int daily_lim;
    int transac_lim;
    daily_transac_t one_day;
} card_t;

typedef struct {
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
} time_t;

typedef struct {
    char transac_id[TRANSAC_ID_LEN+1];
    char card_id[CARD_ID_LEN+1];
    time_t transac_time;
    int transac_amount;
} transac_t;

typedef struct {
    transac_t *transac;
    int not_eof;
} read_transac_t;


//===========================================================================
// taken or modified from listops.c
typedef struct node node_t;
struct node {
	transac_t *data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t; 

list_t *make_empty_list(void);
void free_list(list_t *list);
list_t *insert_at_foot(list_t *list, transac_t *value);
//===========================================================================


/*+++++++++++++++++++++ my function declarations uwu +++++++++++++++++++++++*/
void print_stage(int stage);
void stage_one(card_t cards[], int *card_num);
void stage_two(card_t cards[], int *card_num);
list_t *stage_three();
void stage_four(list_t *transac_list, card_t cards[], int card_num);

void read_cards(card_t cards[], int *card_num, int *total_daily_lim, 
                int *largest_transac_lim);
int read_one_card(card_t *one_card);
char *read_id(int id_len);
list_t *read_transacs();
read_transac_t read_one_transac();
char *check_fraud(transac_t *one_transac, card_t cards[], int card_num);
int id_cmp(const void *id, const void *card);
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//########################## main function ###################################
int 
main(int argc, char *argv[]) {
    
    /* initialise some values */
    card_t cards[MAX_NUM_CARDS];
    int card_num=0;
    
    /* Stages */
    stage_one(cards, &card_num);
    
    stage_two(cards, &card_num);
    
    //char *s = "3tu2iywy";
    //printf("%d\n", id_cmp(&s, cards[0]));
    
    list_t *transac_list = stage_three();
    
    stage_four(transac_list, cards, card_num);
    
    /* free memory */
    free_list(transac_list);
    transac_list = NULL;
    
    return 0;
}
//############################################################################


/////////////////////////////// Stage 1&2 ///////////////////////////////////
void 
stage_one(card_t cards[], int *card_num) {
    /* Stage 1 - Reading One Credit Card Record */
    print_stage(STAGE_ONE);
    
    /* read one card */
    //cards[*card_num] = malloc(sizeof(card_t));
    //assert(new_card != NULL);
    read_one_card(&(cards[*card_num]));
    
    /* print out result */
    printf("Card ID: %s\n", cards[*card_num].card_id);
    printf("Daily limit: %d\n", cards[*card_num].daily_lim);
    printf("Transaction limit: %d\n", cards[*card_num].transac_lim);
    
    *card_num += 1;
    printf("\n");
}

void 
stage_two(card_t cards[], int *card_num){
    /* Stage 2 - Reading All Credit Card Records */
    print_stage(STAGE_TWO);
    
    /* read in rest of card records */
    int *total_daily_lim = malloc(sizeof(int*)), 
    *largest_transac_lim = malloc(sizeof(int*));
    *total_daily_lim = cards[(*card_num)-1].daily_lim;
    *largest_transac_lim = (*card_num)-1;
    
    read_cards(cards, card_num, total_daily_lim, largest_transac_lim);
    
    /* print out desired outputs */
    printf("Number of credit cards: %d\n", *card_num);
    printf("Average daily limit: %.2f\n", (double)*total_daily_lim / *card_num);
    printf("Card with the largest transaction limit: %s\n", 
            cards[*largest_transac_lim].card_id);
    
    free(total_daily_lim);
    total_daily_lim = NULL;
    free(largest_transac_lim);
    largest_transac_lim = NULL;
    
    printf("\n");
}

void 
read_cards(card_t cards[], int *card_num, int *total_daily_lim, 
                int *largest_transac_lim) {
    /* function to read all the credit card records up to the %%%%%%%%%% line*/
    
    //cards[*card_num] = malloc(sizeof(card_t));
    //assert(new_card != NULL);
    
    while (read_one_card(&(cards[*card_num]))) {
        *total_daily_lim += cards[*card_num].daily_lim;
        if (cards[*card_num].transac_lim > 
            cards[*largest_transac_lim].transac_lim) {
            *largest_transac_lim = *card_num;
        }
        *card_num += 1;
        
        //cards[*card_num] = malloc(sizeof(card_t));
        //assert(new_card != NULL);
    }
}

int
read_one_card(card_t *one_card) {
    /* reads in the card id, daily limit and transaction limit of one card */
    /* return 1 if successfully read card record, else return 0 */
    char special_line[] = "%%%%%%%%%%";
    
    /* read in the card id */
    char *c_id;
    c_id = read_id(CARD_ID_LEN);
    
    /* check if reading in special line */
    if (strcmp(c_id, special_line) == 0) {
        return 0;
    }
    
    strcpy(one_card->card_id, c_id);
    
    free(c_id);
    c_id = NULL;
    
    /* read in the daily limit and transaction limit */
    scanf("%d %d", &one_card->daily_lim, &one_card->transac_lim);
    getchar(); //'\n'
    
    // for use in stage 4: initialise the information about transacs each day
    one_card->one_day.total_transac = one_card->one_day.year = 
    one_card->one_day.mon = one_card->one_day.day = 0;

    return 1;
}



/////////////////////////////// Stage 3 /////////////////////////////////////
list_t
*stage_three(){
    /* Stage 3 - Reading the Transactions */
    print_stage(STAGE_THREE);
    
    list_t *transac_list = read_transacs();
    
    printf("\n");
    return transac_list;
}

list_t 
*read_transacs() {
    /* read all the transactions into a list and return the list pointer*/
    list_t *transac_list = make_empty_list(); 
    
    /* read each transac and print each transac ID */
    read_transac_t one_transac;
    while ((one_transac = read_one_transac()).not_eof) {
        insert_at_foot(transac_list, one_transac.transac);
        printf("%s\n", one_transac.transac->transac_id);
    }
    insert_at_foot(transac_list, one_transac.transac);
    printf("%s\n", one_transac.transac->transac_id);
    
    return transac_list;
}

read_transac_t
read_one_transac(){ 
    /* reads the transac id, card id, card id, transac time and transac amount
       for one transaction. Returns zero at EOF */
    
    transac_t *curr_transac = malloc(sizeof(transac_t));
    assert(curr_transac != NULL);
    
    read_transac_t one_transac;
    
    /* read in transac ID and card ID */
    strcpy(curr_transac->transac_id, read_id(TRANSAC_ID_LEN));
    strcpy(curr_transac->card_id, read_id(CARD_ID_LEN));
    
    /* read in time of transac and transac amount */
    scanf("%d:%d:%d:%d:%d:%d %d", &curr_transac->transac_time.year, 
            &curr_transac->transac_time.mon, &curr_transac->transac_time.day,
            &curr_transac->transac_time.hour, &curr_transac->transac_time.min,
            &curr_transac->transac_time.sec, &curr_transac->transac_amount);
    
    one_transac.transac = curr_transac;
    
    char n = getchar();
    if (n == EOF) {
        one_transac.not_eof = 0;
    } else {
        one_transac.not_eof =  1;
    }
    return one_transac;
}



/////////////////////////////// Stage 4 /////////////////////////////////////
void 
stage_four(list_t *transac_list, card_t cards[], int card_num){
    /*Stage 4 - Checking for Fraudulent Transactions*/
    print_stage(STAGE_FOUR);
    
    /* check whether each transaction is valid, print the result */
    node_t *curr;
    curr = transac_list->head;
    char *result;
    while (curr != NULL) {
        result = check_fraud(curr->data, cards, card_num);
        printf("%s             %s\n", curr->data->transac_id, result);
        curr = curr->next;
    }
}

char 
*check_fraud(transac_t *one_transac, card_t cards[], int card_num) {
    /* check whether each transaction lies within bound of the two limits
       and return the corresponding string to be printed */
    
    // find position of the card id in cards
    card_t *card_pos;
    card_pos = bsearch(one_transac->card_id, cards, card_num, 
                        sizeof(card_t), id_cmp);
    if (card_pos==NULL) { ////////////
        printf("***Not Found\n"); /////////////
    } /////////////
    
    // see if it is the first transaction of the day
    if ((one_transac->transac_time.year == card_pos->one_day.year) &&
        (one_transac->transac_time.mon == card_pos->one_day.mon) &&
        (one_transac->transac_time.day == card_pos->one_day.day)) {
        
        card_pos->one_day.total_transac += one_transac->transac_amount;
    } else {
        card_pos->one_day.year = one_transac->transac_time.year;
        card_pos->one_day.mon = one_transac->transac_time.mon;
        card_pos->one_day.day = one_transac->transac_time.day;
        card_pos->one_day.total_transac = one_transac->transac_amount;
    }
    
    // find if limits are exceeded
    int over_daily = 0, over_transac = 0;
    if (card_pos->one_day.total_transac > card_pos->daily_lim) {
        over_daily = 1;
    }
    if (one_transac->transac_amount > card_pos->transac_lim) {
        over_transac = 1;
    }
    
    //printf("daily: %d, trans: %d\n", over_daily, over_transac);
    
    // return according to whether any limits are exceeded 
    char *return_str;
    if ((over_daily == 0) && (over_transac == 0)) {
        return_str = "IN_BOTH_LIMITS";
    } else if ((over_daily == 1) && (over_transac == 0)) {
        return_str = "OVER_DAILY_LIMIT";
    } else if ((over_daily == 0) && (over_transac == 1)) {
        return_str = "OVER_TRANS_LIMIT";
    } else {
        return_str = "OVER_BOTH_LIMITS";
    }
    return return_str;
}

int 
id_cmp(const void *id, const void *card) {
    return strcmp(id, ((const card_t *)card)->card_id);
}

/////////////////////////////////////////////////////////////////////////////


//$$$$$$$$$$$$$$$$$$$$ Functions used in multiple stages $$$$$$$$$$$$$$$$$$$$
void 
print_stage(int stage) {
    printf("=========================Stage %d=========================\n", 
            stage);
}

char 
*read_id(int id_len) {
    /* read in one string id and return the pointer to the id*/
    char c, *id = malloc((id_len+1)*sizeof(char));
    assert(id != NULL);
    int i=0;
    while (((c = getchar()) != EOF) && (c != '\n') && (c != ' ')) {
        id[i++] = c;
    }
    id[i] = '\0';
    return id;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


//=============================================================================
// taken or modified from listops.c
list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

list_t
*insert_at_foot(list_t *list, transac_t *value) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->data = value;
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}
//=============================================================================


/******************************************************************************
    Explanation for stage 4:
    
    bsearch in check_fraud has an average runtime of logn for n credit card
    records. All of the other operations such as comparing the dates of
    transactions or changing the card struct's date would have runtime of 
    constants. Therefore check_fraud has runtime O(logn).
    
    Since check_fraud is called m times in stage_four for m transactions, 
    and since the runtimes of all other operations inside and outside the while 
    loop in stage_four are constants, stage_four has runtime O(mlogn).
    
******************************************************************************/

