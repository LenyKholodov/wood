              class dataiterator;
              class iterator: public ::iterator<ringnode_t,T>
              {
                friend class RingTmpl<T>;
                protected:
                                iterator (ringnode_t* node) { mCurrent = node; }
                public:
                                iterator (const RingTmpl<T>& r) { mCurrent = r.mFirst; }

                      iterator& operator =  (const RingTmpl& ring);
                      operator dataiterator& () const { return (dataiterator&)*this; }

                private:
                      ringnode_t*&   ptr     () const { return (ringnode_t*)mCurrent; }  
              };

              class dataiterator: public iterator
              {
                public:
                                dataiterator () {}
                                dataiterator (const RingTmpl& ring):iterator (ring)  {}

                      T*        operator -> () const { return &data(); } 
              };