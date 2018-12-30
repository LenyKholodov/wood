#ifndef __UTILITES_STRING_CLASS__
#define __UTILITES_STRING_CLASS__

#include <types.h>
#include <iostream.h>

////////////////////////////////////////////////////////////////////////////////
///��ப� (ASCIIZ)
////////////////////////////////////////////////////////////////////////////////
template <size_t ssize>
class    String
{
  public:
                        String  (const char* string = NULL);
                        String  (const String&);

//                        operator char*  () { dout<<"convert"<<endl; return mString; }
                        operator char*  () { return mString; }
                        operator bool   () const { return mString [0] != 0; }

              char*     string          () const { return (char*)mString; }

              String&   operator =      (const String&);
              String&   operator =      (const char*);  //copy to buffer

   friend     String    operator +      (const String&,const String&);
   friend     String    operator +      (const String&,const char*);
   friend     String    operator +      (const String&,char);
   friend     String    operator +      (const String&,int);
   friend     String    operator +      (const String&,double);

   friend     String    operator +      (const char*,const String&);
   friend     String    operator +      (int,const String&);
   friend     String    operator +      (double,const String&);
   friend     String    operator +      (char,const String&);

              String&   operator +=     (const String&);
              String&   operator +=     (const char*);
              String&   operator +=     (int);
              String&   operator +=     (char);
              String&   operator +=     (double);

              bool      operator !=     (const String&) const;
              bool      operator ==     (const String&) const;
              bool      operator !=     (const char*)   const;
              bool      operator ==     (const char*)   const;
   friend     bool      operator ==     (const char*,const String&);
   friend     bool      operator !=     (const char*,const String&);

              size_t    length          () const; //string length 
              ulong     crc             () const; //CRC
              
              void      reset           ();       //reset string

   friend     size_t    length          (const String&); //string length 
   friend     ulong     crc             (const String&); //CRC

              void      upcase          ();
              void      locase          ();

   friend     String    upcase          (const String&);
   friend     String    locase          (const String&);

   friend     ostream&  operator <<     (ostream&,const String&);
   friend     istream&  operator >>     (istream&,String&);

   friend     bool      IsValid         (const String*);

  protected:
              void      cnct            (const char*);
              void      cnct            (const String&);

  private:
        char   mString [ssize+1];
};

template <size_t ssize>
String<ssize>    upcase          (const String<ssize>&);
template <size_t ssize>
String<ssize>    locase          (const String<ssize>&);

template <size_t ssize>
ostream&  operator <<     (ostream&,const String<ssize>&);
template <size_t ssize>
istream&  operator >>     (istream&,String<ssize>&);

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>&,const String<ssize>&);
template <size_t ssize>
String<ssize>    operator +      (const String<ssize>&,const char*);
template <size_t ssize>
String<ssize>    operator +      (const String<ssize>&,char);
template <size_t ssize>
String<ssize>    operator +      (const String<ssize>&,int);
template <size_t ssize>
String<ssize>    operator +      (const String<ssize>&,double);

template <size_t ssize>
String<ssize>    operator +      (const char*,const String<ssize>&);
template <size_t ssize>
String<ssize>    operator +      (int,const String<ssize>&);
template <size_t ssize>
String<ssize>    operator +      (double,const String<ssize>&);
template <size_t ssize>
String<ssize>    operator +      (char,const String<ssize>&);


template <size_t ssize>
size_t    length          (const String<ssize>&); //String<ssize> length 
template <size_t ssize>
ulong     crc             (const String<ssize>&); //CRC

typedef   String<127>     Str128;
typedef   String<255>     Str256;
typedef   String<31>      Str32;
typedef   String<15>      Str16;
typedef   String<1023>    Str1024;

#include <misc\string.inl>

#endif