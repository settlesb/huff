#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct TN {
  int charac,amount;
  struct TN *left, *right;
} TN;
typedef struct LN {
  struct LN * next;
  TN * T_ptr;
} LN;
typedef struct fList
{
  int f,c_val;
  struct fList *next;
} Node;
TN * Header(FILE * iptr);
LN * MergeLN(int type, LN * head); 
LN * LN_create(TN * tn);
LN * LN_insert(LN * head, LN * win, int type); 
void freeT(TN * node);
TN * TN_create(int amount_val, int c_val);
TN * T_merge(TN * tn1, TN * tn2);
int Bit(FILE * fptr, unsigned char * bit, unsigned char * wbit, unsigned char * cbyte);
int decompress(char * ofile, char * ifile);

int main(int argc, char **argv)
{
  char * filename = argv[1];
  int filename_size = strlen(filename);
  char * ofile = (char *)malloc(filename_size+7 * sizeof(char));
  strcpy(ofile, filename);
  ofile = strcat(ofile, ".unhuff");
  decompress(ofile,filename);
  return 0;
}
TN * Header(FILE * iptr)
{
  int quit = 0, bitc;
  unsigned char wbit = 0, cbyte = 0, bit1 = 0, val = 0;
  LN * head = NULL;
  while(quit == 0)
  {
    Bit(iptr, &bit1, &wbit, &cbyte);
    if(bit1 == 1)
    {
      val = 0;
      for(bitc = 0; bitc < 7; ++bitc)
      {
        val <<= 1; 
        Bit(iptr, &bit1, &wbit, &cbyte);
        val += bit1;
      }
      TN * tn = TN_create(0,val);
      LN * win = LN_create(tn);
      head = LN_insert(head, win, 1);
    }
    else
    {
      if((head -> next) == NULL)
      {
        quit = 1;
      }
      else
      {
        head = MergeLN(1, head);
      }
    }
  }
  TN * rot = head -> T_ptr;
  free(head);
  return rot;
}
LN * MergeLN(int end,LN * head)
{
  LN * sec = head -> next,* thi = sec -> next;
  TN * tn2 = sec -> T_ptr, *finale, * tn1 = head -> T_ptr;
  free(head);
  free(sec);
  head = thi;
  if (end == 0)
  {
    finale = T_merge(tn1, tn2);
  }
  else
  {
    finale = T_merge(tn2, tn1);
  }
  LN * win = LN_create(finale);
  if (end == 0)
  {
    head = LN_insert(head, win, 2);
  }
  else
  {
    head = LN_insert(head, win, 1);
  }
  return head;
}
LN * LN_insert(LN * head, LN * win, int type)
{
  if(head == NULL)
  {
    return win;
  }
  else if (type == 1)
  {
    win -> next = head;
    return win;
  }
  else if (type == 0)
  {
    head -> next = LN_insert(head -> next, win, type);
    return head;
  }
  int amount1 = (head -> T_ptr) -> amount, amount2 = (win -> T_ptr) -> amount;
  if (amount1 > amount2)
  {
    win -> next = head;
    return win;
  }
  head -> next = LN_insert(head -> next, win, type);
  return head;
}
LN * LN_create(TN * tn)
{
  LN * win = malloc(sizeof(LN));
  win -> T_ptr = tn;
  win -> next = NULL;
  return win;
}
TN * TN_create(int amount_val,int c_val)
{
  TN * node = malloc(sizeof(TN));
  node -> left = NULL;
  node -> right = NULL;
  node -> charac = c_val;
  node -> amount = amount_val;
  return node;
}
TN * T_merge(TN * tn1, TN * tn2)
{
  TN * tn = malloc(sizeof(TN));
  tn -> right = tn2;
  tn -> amount = (tn1 -> amount) + (tn2 -> amount);
  tn -> left = tn1;
  tn -> charac = 0;
  return tn;
}
int decompress(char * ofile, char * ifile)
{
  FILE * iptr = fopen(ifile, "r");
  FILE * outfptr = fopen(ofile, "w");
  TN * rot = Header(iptr);
  unsigned int numChar = 0;
  unsigned char newline, wbit = 0, bit1 = 0, cbyte = 0;
  fread(&numChar, sizeof(unsigned int), 1, iptr);
  fread(&newline, sizeof(unsigned char), 1, iptr);
  while(numChar != 0)
  {
    TN * tn = rot;
    while ((tn -> left) != NULL)
    {
      Bit(iptr, &bit1, &wbit, &cbyte);
      if(bit1 == 0)
      {
        tn = tn -> left;
      }
      else
      {
        tn = tn -> right;
      }
    }
    fprintf(outfptr, "%c", tn -> charac);
    --numChar;
  }
  freeT(rot);
  fclose(iptr);
  fclose(outfptr);
  return 0;
}
void freeT(TN * node)
{
  if(node == NULL)
  {
    return;
  }
  freeT(node -> left);
  freeT(node -> right);
  free(node);
}
int Bit(FILE * fptr, unsigned char * bit, unsigned char * wbit, unsigned char * cbyte)
{
  int ret = 1;
  if((*wbit) == 0)
  {
    ret = fread(cbyte, sizeof(unsigned char), 1, fptr);
  }
  if(ret != 1)
  {
    return -1;
  }
  unsigned char temp = (*cbyte) >> (7 - (*wbit));
  temp = temp & 0X01;
  *wbit = ((*wbit) + 1) % 8;
  *bit = temp;
  return 0;
}
