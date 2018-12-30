#define ImplOperator(CLASS,OP) \
template <class T,const int P> \
Number& Value<T,P>::operator OP (const CLASS& n)    \
{ \
  return prior () < n.prior () ?  \
         (Number&)*new CLASS (x OP n.data ()) :  \
         (Number&)*new Value<T,P> (x OP (T)n.data ());   \
}

#define ImplementationOperator(OP) \
  EnumClassOper (ImplOperator,OP) \
template <class T,const int P> \
Number& Value<T,P>::operator OP (const Number& n)   \
{ \
  return (Number&)n OP *this; \
}

ImplementationOperator (+);
ImplementationOperator (-);
ImplementationOperator (*);
ImplementationOperator (/);


