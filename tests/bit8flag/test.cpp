#define  DEBUG
#include <iostream.h>
#include <time.h>
#include <types.h>

const    int    TOTAL_SIZE = 32*1024*1024;
const    int    TABLE_SIZE = (TOTAL_SIZE / 8) + 1;
const    int    TEST       = 32*1024*1024;

static char     flag [TABLE_SIZE];
static char     mask  [8] = {1,2,4,8,16,32,64,128};

inline void            set   (int);
inline BOOL            check (int);

void   main ()
{
  cout<<"Set speed..."<<endl;

  double start = clock ();

  for (int i=0;i<TEST;i++)
    set (i);

  double end   = clock ();

  cout<<"...Time: "<<(end-start)/double(CLK_TCK)<<endl;
  cout<<"...FPS:  "<<double(TEST)/(end-start)*float(CLK_TCK)<<endl;

  cout<<endl<<"Check speed..."<<endl;

  start = clock ();

  for (i=0;i<TEST;i++)
    check (i);

  end   = clock ();

  cout<<"...Time: "<<(end-start)/float(CLK_TCK)<<endl;
  cout<<"...FPS:  "<<float(TEST)/(end-start)*float(CLK_TCK)<<endl;
}

inline void            set   (int i)
{
  flag [i>>3] |= mask [i&7];
}

inline BOOL            check (int i)
{
  return flag [i>>3] & mask [i&7];
}
