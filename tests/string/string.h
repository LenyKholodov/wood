#ifndef __UTILITES_STRING_CLASS__
#define __UTILITES_STRING_CLASS__

#include <types.h>
#include <iostream.h>

////////////////////////////////////////////////////////////////////////////////
///Строка (ASCIIZ)
////////////////////////////////////////////////////////////////////////////////
template <char* string>
class    BaseString
{
  public:
                        BaseString  (const char* = NULL);
                        BaseString  (const BaseString&);

//                        operator char*  () { dout<<"convert"<<endl; return string; }
                        operator char*  () { return string; }
                        operator bool   () { return string [0] != 0; }

              BaseString&   operator =      (const BaseString&);
              BaseString&   operator =      (const char*);  //copy to buffer

   friend     BaseString    operator +      (const BaseString&,const BaseString&);
   friend     BaseString    operator +      (const BaseString&,const char*);
   friend     BaseString    operator +      (const BaseString&,char);
   friend     BaseString    operator +      (const BaseString&,int);
   friend     BaseString    operator +      (const BaseString&,double);

   friend     BaseString    operator +      (const char*,const BaseString&);
   friend     BaseString    operator +      (int,const BaseString&);
   friend     BaseString    operator +      (double,const BaseString&);
   friend     BaseString    operator +      (char,const BaseString&);

              BaseString&   operator +=     (const BaseString&);
              BaseString&   operator +=     (const char*);
              BaseString&   operator +=     (int);
              BaseString&   operator +=     (char);
              BaseString&   operator +=     (double);

              bool      operator !=     (const BaseString&) const;
              bool      operator ==     (const BaseString&) const;
              bool      operator !=     (const char*)   const;
              bool      operator ==     (const char*)   const;
   friend     bool      operator ==     (const char*,const BaseString&);
   friend     bool      operator !=     (const char*,const BaseString&);

              size_t    length          () const; //string length 
              ulong     crc             () const; //CRC
              
              void      reset           ();       //reset string

   friend     size_t    length          (const BaseString&); //string length 
   friend     ulong     crc             (const BaseString&); //CRC

              void      upcase          ();
              void      locase          ();

   friend     BaseString    upcase          (const BaseString&);
   friend     BaseString    locase          (const BaseString&);

   friend     ostream&  operator <<     (ostream&,const BaseString&);
   friend     istream&  operator >>     (istream&,BaseString&);

   friend     bool      IsValid         (const BaseString*);

  protected:
              void      cnct            (const char*);
              void      cnct            (const BaseString&);
};

template <size_t ssize>
class String: public BaseString<String::mString>
{
  public:
                        String  (const char* str): BaseString (str) {}

  private:
    char        mString [ssize];
};

template <char* string>
BaseString<string>    upcase          (const BaseString<string>&);
template <char* string>
BaseString<string>    locase          (const BaseString<string>&);

template <char* string>
ostream&  operator <<     (ostream&,const BaseString<string>&);
template <char* string>
istream&  operator >>     (istream&,BaseString<string>&);

template <char* string>
BaseString<string>    operator +      (const BaseString<string>&,const BaseString<string>&);
template <char* string>
BaseString<string>    operator +      (const BaseString<string>&,const char*);
template <char* string>
BaseString<string>    operator +      (const BaseString<string>&,char);
template <char* string>
BaseString<string>    operator +      (const BaseString<string>&,int);
template <char* string>
BaseString<string>    operator +      (const BaseString<string>&,double);

template <char* string>
BaseString<string>    operator +      (const char*,const BaseString<string>&);
template <char* string>
BaseString<string>    operator +      (int,const BaseString<string>&);
template <char* string>
BaseString<string>    operator +      (double,const BaseString<string>&);
template <char* string>
BaseString<string>    operator +      (char,const BaseString<string>&);


template <char* string>
size_t    length          (const BaseString<string>&); //BaseString<string> length 
template <char* string>
ulong     crc             (const BaseString<string>&); //CRC

typedef   String<127>     Str128;
typedef   String<255>     Str256;
typedef   String<31>      Str32;
typedef   String<15>      Str16;
typedef   String<1023>    Str1024;

#include "string.inl"

#endif