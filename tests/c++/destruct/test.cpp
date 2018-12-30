template <class T> struct X
{
  T x;

  void  test () { x.~T (); }
};

void    main ()
{
  X<int> x;
  x.test ();
}