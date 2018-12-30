struct cross
{
  template< class ta_a, class ta_b > 
  static const float Evaluate( const int i, const ta_a& A, const ta_b& B )
  { 
    return A.Evaluate ((i+ta_dimension-2)%ta_dimension) * 
           B.Evaluate ((i+ta_dimension-1)%ta_dimension) - 
           A.Evaluate ((i+ta_dimension-1)%ta_dimension) * 
           B.Evaluate ((i+ta_dimension-2)%ta_dimension); 
  }

};

//template< class ta_c1, class ta_c2 > inline 
//        const vecexp_2< const ta_c1, const ta_c2, ta_c2::cross > 
//        operator ^ ( const ta_c1& Pa, const ta_c2& Pb )
//{
//        return vecexp_2< const ta_c1, const ta_c2, ta_c1::cross >( Pa, Pb );
//}
