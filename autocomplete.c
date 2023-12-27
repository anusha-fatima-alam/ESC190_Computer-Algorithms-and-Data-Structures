#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "autocomplete.h"
/* Project 1_Autocomplete*/

/* Part 1 _ Comparison Function for Comparing strings */
int lexicographicallysort(const void *a, const void *b) {
   return(strcmp(((struct term*)a)->term, ((struct term*)b)->term));
}

/* Function for determining length*/
int determinelength(char* str){
    int length = 0;
    while (isalpha(str[length])!=0){
        length++;
    }
    return length;
}

/* Part A */
void read_in_terms(struct term **terms, int *pnterms, char *filename){
    // Open the file and read in terms
    char line[200];
    FILE *fp = fopen(filename, "r");
    fgets(line, sizeof(line), fp);
    *pnterms = atoi(line);
    *terms = (struct term*)malloc(sizeof(struct term)*(*pnterms));

    for(int m = 0; m < *pnterms; m++){
        fgets(line, sizeof(line), fp);
        int y = 0;
        strcpy((*terms+m)->term, line);
        while ((isspace((*terms+m)->term[y]))!=0){
            y++;
        }
        int start = y;
        while ((isspace((*terms+m)->term[y]))==0){
            y++;
        }

        int endweight = -1+y;
        int s_term = 1+y;
        int e_term = - 2 + strlen((*terms+m)->term);

        char weight[200];
        strncpy(weight,(*terms+m)->term+start, endweight);
        (*terms+m)->weight = (double)atoll(weight);

        char term[200];
        strncpy(term, (*terms+m)->term+s_term, e_term);
        strcpy((*terms+m)->term, term);
        int length = strlen((*terms+m)->term);
        if ((*terms+m)->term[length-1] == '\n'){   
            (*terms+m)->term[length-1] = '\0';
        }
    }
    qsort(*terms, *pnterms, sizeof(struct term), lexicographicallysort);   
    fclose(fp);
}

/* Part B */
int lowest_match(struct term *terms, int nterms, char *substr)
{
    int l= 0;
    int u = nterms;
    int x = -2147483647;
    int length = determinelength(substr);    // Call the function determine length
    // if lower is less then upper 
    while(l<u){
        int central= (l+u)/2;
        if (strcmp((terms+central)->term, substr)<0 && strncmp(substr, (terms+central)->term, length)!=0){
            l = central+1;
        } 
        else if (strcmp((terms+central)->term, substr)>0 && strncmp(substr, (terms+central)->term, length)!=0 ){
            u = central;
        }
        else if(strncmp(substr, (terms+central)->term, length)==0 && central>0){
            u = central;
        }
        else {
            return central;
        }
    }
    if (l==u){
        return l;
    }
    return x;
}

int highest_match(struct term *terms, int nterms, char *substr){
    int half = nterms/2;
    int x = -2147483647;
    // Call the function determine length
    int length = determinelength(substr);
    if (nterms<=2){   
        if (strncmp((terms+half)->term, substr, length)==0){
            return half;
        }
        else if (strncmp((terms+half-1)->term, substr, length)==0){
            return half-1;
        }
        else{
            return x;
        }
    }
    else if (strncmp((terms+half)->term, substr, length)==0){
        if (strncmp((terms+half)->term, substr, length)>0){
            return (highest_match(terms, half, substr));
        }
        else{
            struct term* newterm = (terms+half+1);
            return (half+1+highest_match(newterm, half, substr));
        }

    }
    else if (strcmp((terms+half)->term, substr)<0){   
        struct term* newterm = (terms+half+1);
        return (half+1+highest_match(newterm, half, substr));
    }
    else if (strcmp((terms+half)->term, substr)>0){
        return (highest_match(terms, half, substr));
    }
    return half;
}

/* Part 4 _ Comparison Function*/
int sortbyweight(const void *a, const void *b){
   return (-((struct term *)a)->weight + ((struct term*)b)->weight);
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    int low = lowest_match(terms, nterms, substr);
    int hi = highest_match(terms, nterms, substr);
    
    if (hi<0 || low<0){
        *n_answer = 0;
        return;
    }

    *n_answer = 1+(hi-low);
    *answer = (struct term*)malloc(sizeof(struct term) * (*n_answer));
    int count = 0;

    for (int m=low; m<low+*n_answer; m++){
        strcpy((*answer+count)->term, (terms+m)->term);
        (*answer+count)->weight = (terms+m)->weight;
        count++;
    }
    qsort(*answer, *n_answer, sizeof(struct term), sortbyweight);
}