class   Phys
{
  public:
    result_t     SoundHandler (uint msg,stream& s)
    {
      switch (msg)
      {
        case XXX: s>>x>>y>>z;
                  return true;
      }
    }

    result_t     GraphHandler (uint msg,...); ///msg 

};