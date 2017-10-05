#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct fList
{
  int freq,val;
  struct fList * nxt;
} Node;
typedef struct TN{
  int charac,w;
  struct TN * left,*right;
} TN;
typedef struct LN{
  struct LN * nxt;
  TN * T_ptr;
} LN;
void frequency(char *file, int * freq);
int pad(FILE * fp, unsigned char * wbit, unsigned char * cbyte);
int heightHelp(TN * tn, int height);
Node * Linsert(Node * head, int value, int i);
void Lfree(Node * head);
void binaryhelp(TN * tn, int ** bin, int * row, int column);
Node * Lcreate(int value, int i);
Node * Lmake(int * freqs);
void binary(int ** bin,TN * rot);
LN * LN_create(TN * tn);
LN * LN_insert(LN * head, LN * wn);
LN * LN_make(Node * fList);
void Tfree(TN * node);
TN * TNcreate(int val, int w_val);
void TLhelp(TN * tn, int * num);
void headhelp(TN * tn, FILE * outfp, unsigned char * wbit, unsigned char * cbyte);
void head(TN * tn, char * file, unsigned int nchar); //TN * tn, unsigned int nchar, char * file
int TL(TN * tn);
TN * T_merge(TN * tn1, TN * tn2);
int Bit(FILE * fp, unsigned char bit, unsigned char * wbit, unsigned char * cbyte);
void cbits(FILE * outfp, int ch, unsigned char * cbyte, unsigned char * wbit); 
int huff(char * ifile, char * ofile, int * m, int ** bin); 
int main(int argc, char **argv)
{
  char * f = argv[1];
  int array[127], filesize = strlen(f);
  char * ofile = malloc(filesize+5 * sizeof(char));
  frequency(f, array);
  strcpy(ofile, f);
  ofile = strcat(ofile, ".huff");
  Node * list = Lmake(array);
  LN * T_list = LN_make(list);
  while(T_list -> nxt != NULL)
  {
    LN * sec = T_list -> nxt,* th = sec -> nxt;
    TN * tn1 = T_list -> T_ptr,* tn2 = sec -> T_ptr;
    free(sec);
    free(T_list);
    T_list = th;
    TN * fin = T_merge(tn1, tn2);
    LN * wn = LN_create(fin);
    T_list = LN_insert(T_list, wn);
  }
  TN * rot = T_list -> T_ptr; 
  Lfree(list);
  free(T_list);
  int Rows = TL(rot), row,Columns = heightHelp(rot,0)+1,** bin, column,z, ix, m[127], ix2;
  unsigned int nchar = 0;
  bin = malloc(sizeof(int*) * Rows);
  for(row = 0; row < Rows; ++row)
  {
    bin[row] = malloc(sizeof(int) * Columns);
    for(column = 0; column < Columns; ++column)
    {
      bin[row][column] = -1;
    }
  }
  binary(bin,rot);
  for(z = 0; z < 127; ++z)
  {
    nchar += array[z];
  }
  for(ix = 0; ix < 127; ++ix)
  {
    m[ix] = -1;
    for(ix2 = 0; ix2 < Rows; ++ix2 )
    {
      if(bin[ix2][0] == ix)
      {
        m[ix] = ix2;
      }
    }
  }
  head(rot, ofile, nchar);
  huff(f, ofile, m,bin);
  return 0;
}
int huff(char * ifile, char * ofile, int *m, int ** bin)
{
  FILE * infp = fopen(ifile, "r"),*outfp = fopen(ofile, "a"); 
  unsigned char wbit = 0, cbyte = 0;
  int c, ix, ix2;
  while(!feof(infp))
  {
    c = fgetc(infp);
    if(c != EOF)
    {
      ix = m[c];
      ix2 = 1;
      while(bin[ix][ix2] != -1)
      {
        Bit(outfp, bin[ix][ix2]==1, &wbit, &cbyte);
        ++ix2;
      }
    }
  }
  pad(outfp, &wbit, &cbyte);
  fclose(outfp);
  fclose(infp);
  return 0;
}
TN * TNcreate(int val, int w_val)
{
  TN * node = malloc(sizeof(TN));
  node -> left = NULL;
  node -> right = NULL;
  node -> charac = val;
  node -> w = w_val;
  return node;
}
//merges TNs
TN * T_merge(TN * tn1, TN * tn2)
{
  TN * tn = malloc(sizeof(TN));
  tn -> left = tn1;
  tn -> charac = 0;
  tn -> right = tn2;
  tn -> w = (tn1 -> w) + (tn2 -> w);
  return tn;
}
LN * LN_create(TN * tn)
{
  LN * wn = malloc(sizeof(LN));
  wn -> T_ptr = tn;
  wn -> nxt = NULL;
  return wn;
}
//insert node
LN * LN_insert(LN * head, LN * wn)
{
  if(head == NULL)
  {
    return wn;
  }
  int w1 = (head -> T_ptr) -> w,w2 = (wn -> T_ptr) -> w;
  if (w2 < w1)
  {
    wn -> nxt = head;
    return wn;
  }
  head -> nxt = LN_insert(head -> nxt, wn);
  return head;
}
//creates nodes from freqs
LN * LN_make(Node * fList)
{
  LN * head = NULL;
  while(fList != NULL)
  {
    TN * tn = TNcreate(fList -> val, fList -> freq);
    LN * wn = LN_create(tn);
    head = LN_insert(head, wn);
    fList = fList -> nxt;
  }
  return head;
}
int heightHelp(TN * tn, int height)
{
  if (tn == 0)
  {
    return height;
  }
  int left_h = heightHelp(tn -> left, height + 1);
  int right_h = heightHelp(tn -> right, height + 1);
  if (left_h < right_h)
  {
    return right_h;
  }
  if (left_h > right_h)
  {
    return left_h;
  }
  return left_h;
}
void TLhelp(TN * tn, int * num)
{
  if (tn == 0)
  {
    return;
  }
  TN * lc = tn -> left;
  TN * rc = tn -> right;
  if((lc == NULL) && (rc == NULL))
  {
    ++(*num);
    return;
  }
  TLhelp(lc, num);
  TLhelp(rc, num);
}
int TL(TN * tn)
{
  int num = 0;
  TLhelp(tn, &num);
  return num;
}
void binaryhelp(TN * tn, int ** bin, int * row, int column)
{
  if (tn == NULL)
  {
    return;
  }
  TN * lc = tn -> left;
  TN * rc = tn -> right;
  if((lc == NULL) && (rc == NULL))
  {
    bin[*row][0] = tn -> charac;
    ++(*row);
    return;
  }
  if(lc != NULL)
  {
    int Rows = TL(lc);
    int ix;
    for(ix = * row; ix < (*row) + Rows; ++ix)
    {
      bin[ix][column] = 0;
    }
    binaryhelp(lc, bin, row, column + 1);
  }
  if(rc != NULL)
  {
    int Rows = TL(rc);
    int ix;
    for(ix = *row; ix < (*row) + Rows; ++ix)
    {
      bin[ix][column] = 1;
    }
    binaryhelp(rc, bin, row, column + 1);
  }
}
void binary(int ** bin,TN * rot)
{
  int r = 0;
  binaryhelp(rot, bin, &r, 1);
}
void cbits(FILE * outfp, int ch, unsigned char * cbyte, unsigned char * wbit)
{
  unsigned char m = '@'; 
  while(0 < m)
  {
    Bit(outfp, (ch & m) == m, wbit, cbyte);
    m >>= 1;
  }
}
int Bit(FILE * fp, unsigned char bit, unsigned char * wbit, unsigned char * cbyte)
{
  if((*wbit) == 0)
  {
    *cbyte = 0;
  }
  unsigned char tmp = bit << (7 - (*wbit));
  *cbyte += tmp;
  int val = 0;
  if ((*wbit) == 7)
  {
    int ret = fwrite(cbyte, sizeof(unsigned char), 1, fp);
    if(ret == 1)
    {
      val = 1;
    }
    else
    {
      val = -1;
    }
  }
  *wbit = (++(*wbit)) % 8;
  return val;
}
void headhelp(TN * tn, FILE * outfp, unsigned char * wbit, unsigned char * cbyte)
{
  if (tn == NULL)
  {
    return;
  }
  TN * rc = tn -> right;
  TN * lc = tn -> left;
  if((lc == NULL) && (rc == NULL))
  {
    Bit(outfp, 1, wbit, cbyte);
    cbits(outfp, tn -> charac, cbyte,wbit);
    return;
  }
  headhelp(lc, outfp, wbit, cbyte);
  headhelp(rc, outfp, wbit, cbyte);
  Bit(outfp, 0, wbit, cbyte);
}
void head(TN * tn, char * file, unsigned int nchar)
{
  FILE * outfp = fopen(file, "w");
  unsigned char wbit = 0,cbyte = 0,new = '\n';
  headhelp(tn, outfp, &wbit, &cbyte);
  Bit(outfp, 0, &wbit, &cbyte);
  pad(outfp, &wbit, &cbyte);
  fwrite(&nchar, sizeof(unsigned int), 1, outfp);
  fwrite(&new, sizeof(unsigned char), 1, outfp);
  fclose(outfp);
}
int pad(FILE * fp, unsigned char * wbit, unsigned char * cbyte)
{
  int r = 0;
  while(*wbit != 0)
  {
    r = Bit(fp, 0, wbit, cbyte);
    if(r == -1)
    {
      return -1;
    }
  }
  return r;
}
void frequency(char *file, int * freq)
{
  int i;
  FILE * fp = fopen(file, "r"); 
  for(i = 0; i < 127; ++i) 
  {
    freq[i] = 0;
  }
  while (1) 
  {
    i = fgetc(fp);
    if(i == EOF)
    {
      break;
    }
    ++freq[i]; 
  }
  fclose(fp); 
}
//creates list of freqs
Node * Lmake(int * freqs)
{
  int z = 0,i;
  Node * ln;
  while(freqs[z] == 0)
  {
    ++z;
  }
  ln = Lcreate(z, freqs[z]);
  ++z;
  i = z;
  while(i < 127)
  { 
    if (0<freqs[i])
    {
      ln = Linsert(ln, i, freqs[i]);
    }
    ++i;
  }
  return ln;
}
Node * Lcreate(int value, int i)
{
  Node * ln = malloc(sizeof(Node));
  ln -> val = value;
  ln -> freq = i;
  ln -> nxt = NULL;
  return ln;
}
//sort list
Node * Linsert(Node * head, int value, int i)
{
  Node * tmp,* pres;
  if (head == NULL || head -> freq >= i)
  {
    pres = Lcreate(value, i);
    pres -> nxt = head;
    return pres;
  }
  else
  {
    pres = head;
    while(pres -> nxt != NULL && pres -> freq < i)
    {
      pres = pres -> nxt;
    }
    tmp = Lcreate(value, i);
    tmp -> nxt = pres -> nxt;
    pres -> nxt = tmp;
  }
  return head;
}
//free list
void Lfree(Node * head)
{
  if(head != NULL)
  {
    Lfree(head -> nxt);
  }
  free(head);
}
//free T
void Tfree(TN * node)
{
  if(node == NULL)
  {
    return;
  }
  Tfree(node -> left);
  Tfree(node -> right);
  free(node);
}
