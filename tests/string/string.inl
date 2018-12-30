#include <string.h>
#include <stdio.h>

#include <misc\misc.h>

template <char* string>
size_t    length          (const BaseString<string>& s) { return s.length (); }

template <char* string>
ulong     crc             (const BaseString<string>& s) { return s.crc ();    } 

template <char* string>
BaseString<string>    upcase          (const BaseString<string>& s1) 
{ 
  BaseString<string> s (s1);
  s.upcase (); 
  return s;
}

template <char* string>
BaseString<string>    locase          (const BaseString<string>& s1)
{ 
  BaseString<string> s (s1);
  s.locase (); 
  return s;
}

template <char* string>
size_t    BaseString<string>::length  () const { return strlen (string);}

template <char* string>
ulong     BaseString<string>::crc     () const { return CRC32 (string,length ()); }

template <char* string>
BaseString<string>&   BaseString<string>::operator =      (const BaseString<string>& s)
{
  strcpy (string,s.string);

  return *this;
}

template <char* string>
BaseString<string>&   BaseString<string>::operator =      (const char* str)
{
  strcpy (string,str);

  return *this;
}

template <char* string>
void      BaseString<string>::cnct            (const char* str)
{
  strcat (string,str);
}

template <char* string>
void      BaseString<string>::cnct            (const BaseString<string>& s)
{
  strcat (string,s.string);
}

template <char* string>
BaseString<string>&   BaseString<string>::operator +=     (const BaseString<string>& s)
{
  cnct (s);

  return *this;
}

template <char* string>
BaseString<string>&   BaseString<string>::operator +=     (const char* str)
{
  cnct (str);

  return *this;
}

template <char* string>
BaseString<string>&   BaseString<string>::operator +=     (int i)
{
  char tmp [16];

  sprintf (tmp,"%i",i);
  cnct    (tmp);

  return *this;
}

template <char* string>
BaseString<string>&   BaseString<string>::operator +=     (char c)
{
  char tmp [16];

  sprintf (tmp,"%c",c);
  cnct    (tmp);

  return *this;
}

template <char* string>
BaseString<string>&   BaseString<string>::operator +=     (double f)
{
  char tmp [16];

  sprintf (tmp,"%f",f); //double
  cnct    (tmp);

  return *this;
}

template <char* string>
BaseString<string>    operator +      (const BaseString<string>& s1,const BaseString<string>& s2)
{
  BaseString<string> s (s1);
  s += s2;
  return s;
}

template <char* string>
BaseString<string>    operator +      (const BaseString<string>& s1,const char* str)
{
  BaseString<string> s (s1);
  s += str;
  return s;
}

template <char* string>
BaseString<string>    operator +      (const BaseString<string>& s1,char c)
{
  BaseString<string> s (s1);
  s += c;
  return s;
}

template <char* string>
BaseString<string>    operator +      (const BaseString<string>& s1,int i)
{
  BaseString<string> s (s1);
  s += i;
  return s;
}

template <char* string>
BaseString<string>    operator +      (const BaseString<string>& s1,double f)
{
  BaseString<string> s (s1);
  s += f;
  return s;
}

template <char* string>
BaseString<string>    operator +      (const char* str,const BaseString<string>& s1)
{
  BaseString<string> s (s1);
  s += str;
  return s;
}

template <char* string>
BaseString<string>    operator +      (int i,const BaseString<string>& s1)
{
  BaseString<string> s (s1);
  s += i;
  return s;
}

template <char* string>
BaseString<string>    operator +      (double f,const BaseString<string>& s1)
{
  BaseString<string> s (s1);
  s += f;
  return s;
}

template <char* string>
BaseString<string>    operator +      (char c,const BaseString<string>& s1)
{
  BaseString<string> s (s1);
  s += c;
  return s;
}

template <char* string>
bool      BaseString<string>::operator !=     (const BaseString<string>& s) const
{
  return strcmp (string,s.string);
}

template <char* string>
bool      BaseString<string>::operator ==     (const BaseString<string>& s) const
{
  return !strcmp (string,s.string);
}

template <char* string>
bool      BaseString<string>::operator !=     (const char* s) const
{
  return s && strcmp (string,s);
}

template <char* string>
bool      BaseString<string>::operator ==     (const char* s)   const
{
  return s && !strcmp (string,s);
}

template <char* string>
bool      operator ==     (const char* str,const BaseString<string>& s)
{
  return str && !strcmp (s.string,str);
}

template <char* string>
bool      operator !=     (const char* str,const BaseString<string>& s)
{
  return str && strcmp (s.string,str);
}

template <char* string>
void      BaseString<string>::reset           ()
{
  string [0] = 0;
}

template <char* string>
void      BaseString<string>::upcase          ()
{
  ::upcase (string,string);
}

template <char* string>
void      BaseString<string>::locase          ()
{
  if (string) ::locase (string,string);
}

template <char* string>
ostream&  operator <<     (ostream& os,const BaseString<string>& s)
{
  os<<s.string;
  return os;  
}

template <char* string>
istream&  operator >>     (istream& is,BaseString<string>& s)
{
  is.getline (s.string,s.mSize-1);
  return is;
}

template <char* string>
BaseString<string>::BaseString<string>  (const char* s)
{  
  if (s)  strcpy (string,string);
  else    string [0] = 0;
}

template <char* string>
BaseString<string>::BaseString<string>  (const BaseString<string>& s)
{
  strcpy (string,s.string);
}
