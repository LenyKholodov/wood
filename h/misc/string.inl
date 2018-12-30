#include <string.h>
#include <stdio.h>

#include <misc\misc.h>

template <size_t ssize>
size_t    length          (const String<ssize>& s) { return s.length (); }

template <size_t ssize>
ulong     crc             (const String<ssize>& s) { return s.crc ();    } 

template <size_t ssize>
String<ssize>    upcase          (const String<ssize>& s1) 
{ 
  String<ssize> s (s1);
  s.upcase (); 
  return s;
}

template <size_t ssize>
String<ssize>    locase          (const String<ssize>& s1)
{ 
  String<ssize> s (s1);
  s.locase (); 
  return s;
}

template <size_t ssize>
size_t    String<ssize>::length  () const { return strlen (mString);}

template <size_t ssize>
ulong     String<ssize>::crc     () const { return CRC32 (mString,length ()); }

template <size_t ssize>
String<ssize>&   String<ssize>::operator =      (const String<ssize>& s)
{
  if (s.length () < ssize)  strcpy (mString,s.mString);
  else
  {
    strncpy (mString,s.mString,ssize);
    mString [ssize-1] = 0;
  }

  return *this;
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator =      (const char* str)
{
  if (str)
  {
    if (strlen (str) < ssize)  strcpy (mString,str);
    else
    {
      strncpy (mString,str,ssize);
      mString [ssize-1] = 0;
    }
  }
  else mString [0] = 0;

  return *this;
}

template <size_t ssize>
void      String<ssize>::cnct            (const char* str)
{
  if (!str)
    return;

  uint len  = length (),
       len1 = strlen (str);

  if      (ssize - len >= len1)   strcat (mString,str);
  else 
  {
    for (int i=0;i<len1;i++)
      mString [i+len] = str [i];
    mString [ssize] = 0;
  }
}

template <size_t ssize>
void      String<ssize>::cnct            (const String<ssize>& s)
{
  uint len  = length (),
       len1 = s.length ();

  if      (ssize - len >= len1)   strcat (mString,s.mString);
  else
  {
    for (int i=0;i<len1;i++)
      mString [i+len] = s.mString [i];
    mString [ssize] = 0;
  }
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator +=     (const String<ssize>& s)
{
  cnct (s);

  return *this;
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator +=     (const char* str)
{
  cnct (str);

  return *this;
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator +=     (int i)
{
  char tmp [16];

  sprintf (tmp,"%i",i);
  cnct    (tmp);

  return *this;
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator +=     (char c)
{
  char tmp [16];

  sprintf (tmp,"%c",c);
  cnct    (tmp);

  return *this;
}

template <size_t ssize>
String<ssize>&   String<ssize>::operator +=     (double f)
{
  char tmp [16];

  sprintf (tmp,"%f",f); //double
  cnct    (tmp);

  return *this;
}

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>& s1,const String<ssize>& s2)
{
  String<ssize> s (s1);
  s += s2;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>& s1,const char* str)
{
  String<ssize> s (s1);
  s += str;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>& s1,char c)
{
  String<ssize> s (s1);
  s += c;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>& s1,int i)
{
  String<ssize> s (s1);
  s += i;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (const String<ssize>& s1,double f)
{
  String<ssize> s (s1);
  s += f;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (const char* str,const String<ssize>& s1)
{
  String<ssize> s (s1);
  s += str;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (int i,const String<ssize>& s1)
{
  String<ssize> s (s1);
  s += i;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (double f,const String<ssize>& s1)
{
  String<ssize> s (s1);
  s += f;
  return s;
}

template <size_t ssize>
String<ssize>    operator +      (char c,const String<ssize>& s1)
{
  String<ssize> s (s1);
  s += c;
  return s;
}

template <size_t ssize>
bool      String<ssize>::operator !=     (const String<ssize>& s) const
{
  return strcmp (mString,s.mString);
}

template <size_t ssize>
bool      String<ssize>::operator ==     (const String<ssize>& s) const
{
  return !strcmp (mString,s.mString);
}

template <size_t ssize>
bool      String<ssize>::operator !=     (const char* s) const
{
  return s && strcmp (mString,s);
}

template <size_t ssize>
bool      String<ssize>::operator ==     (const char* s)   const
{
  return s && !strcmp (mString,s);
}

template <size_t ssize>
bool      operator ==     (const char* str,const String<ssize>& s)
{
  return str && !strcmp (s.mString,str);
}

template <size_t ssize>
bool      operator !=     (const char* str,const String<ssize>& s)
{
  return str && strcmp (s.mString,str);
}

template <size_t ssize>
void      String<ssize>::reset           ()
{
  mString [0] = 0;
}

template <size_t ssize>
void      String<ssize>::upcase          ()
{
  ::upcase (mString,mString);
}

template <size_t ssize>
void      String<ssize>::locase          ()
{
  if (mString) ::locase (mString,mString);
}

template <size_t ssize>
std::ostream&  operator <<     (std::ostream& os,const String<ssize>& s)
{
  os<<s.mString;
  return os;  
}

template <size_t ssize>
std::istream&  operator >>     (std::istream& is,String<ssize>& s)
{
  return is>>s.mString;
//  is.getline (s.mString,ssize-1);
//  return is;
}

template <size_t ssize>
String<ssize>::String<ssize>  (const char* str)
{   
  if (str)
  {
    if (strlen (str) < ssize)  strcpy (mString,str);
    else
    {
      strncpy (mString,str,ssize);
      mString [ssize-1] = 0;
    }
  }
  else mString [0] = 0;
}

template <size_t ssize>
String<ssize>::String<ssize>  (const String<ssize>& s)
{
  if (s.length () < ssize)  strcpy (mString,s.mString);
  else
  {
    strncpy (mString,s.mString,ssize);
    mString [ssize-1] = 0;
  }
}
