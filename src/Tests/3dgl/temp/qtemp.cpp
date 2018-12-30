quatseg_t*  LoadQuatAnim   (BinFile* file,Pool* pool,int& count)
{
  D_Try
  {
    typedef rot_t rotarray_t [2];

    trackrot3ds_t  rot;    
    quat_t         q;
    quatseg_t*     anim = NULL;
    rotarray_t*    r    = NULL;

    file->read (&rot.main,sizeof (rot.main));

    rot.rot    = (rotate3ds_t*)pool->alloc (sizeof (rotate3ds_t) * rot.main.num);
    anim       = rot.main.num ? 
                 (quatseg3ds_t*)pool->alloc (rot.main.num * sizeof (quatseg3ds_t)) :
                 NULL;
    r          = rot.main.num ?
                 (rotarray_t*)pool->alloc (rot.main.num * sizeof (rotarray_t)) :
                 NULL;
    count      = rot.main.num;

    for (int i=0;i<rot.main.num;i++)
    {
      uint   frame;
      uint16 flags;
      uint   mask = 1;

      file->read (&frame,sizeof (frame));
      file->read (&flags,sizeof (flags));

      rot.rot [i].frame = frame;
      rot.rot [i].flags = flags;

      for (int j=0;j<5;j++,mask<<=1)
        if (flags & mask) 
          file->read (&rot.rot [i].spline [j],sizeof (float));
        else              rot.rot [i].spline [j] = 0;

      file->read (&rot.rot [i].angle,sizeof (float)); 
      file->read (&rot.rot [i].ort,sizeof (vector3ds_t)); 

      vector3d_t ort (rot.rot [i].ort.x,rot.rot [i].ort.y,rot.rot [i].ort.z);

      if (i)
      {
        q                    = quat_t (ort,rot.rot [i].angle) * q; 
        anim [i].v1          = q;
        anim [i-1].v2        = q;

        r [i][0].angle       = rot.rot [i].angle;
        r [i][0].vec         = ort;

        r [i-1][1].angle     = rot.rot [i].angle;
        r [i-1][1].vec       = ort;
      }
      else
      {
        q                     = quat_t (ort,rot.rot [i].angle);

        anim [i].v1           = q;            

        r [i][0].angle        = rot.rot [i].angle;
        r [i][0].vec          = ort;
      }              

      if (i == rot.main.num-1)
      {
        r [i][0].angle = rot.rot [i].angle;
        r [i][0].vec   = ort;

        r [i][1].angle = rot.rot [i].angle;
        r [i][1].vec   = ort;
      }
    }

    anim [rot.main.num-1].v2 = q;

//cur.ra = slerp(cur, g3, (tension-1));
//cur.rb = slerp(cur, g3, -(tension-1));
//q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);
//qn.ra = slerp(pn, p(n-1), (1-tension)*(1-continuity*bias)/3.0);

        //Копировать производные
        //Убрать r        

    if (rot.main.num)
    {
      for (int i=0;i<rot.main.num;i++)
      {
        quatseg3ds_t& seg = anim [i];

        seg.easeFrom     = rot.rot [i].spline [EASE_FROM];
        seg.easeTo       = rot.rot [i].spline [EASE_TO];
        seg.start        = rot.rot [i].frame;

        if (i != rot.main.num - 1)
        {
          quat_t g1,g2,g3,g4;
          float  bias       = rot.rot [i].spline [BIAS],
                 tension    = rot.rot [i].spline [TENSION], 
                 continuity = rot.rot [i].spline [CONTINUITY];

          seg.end           = rot.rot [i+1].frame;
          seg.locTime       = 1.0f/float(seg.end-seg.start);

          if (i)
          {
            quat_t* cur  = &seg.v1;
            quat_t* prev = &anim [i-1].v1;
            quat_t* next = &seg.v2;

//cur.ra = slerp(cur, g3, (tension-1));
//cur.rb = slerp(cur, g3, -(tension-1));

            slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
            slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
            slerp (g3,g1,g2,0.5f + 0.5f * continuity);
            slerp (g4,g1,g2,0.5f - 0.5f * continuity);
            slerp (seg.c1,*cur,g3,(tension-1.0f));
            slerp (seg.c2,*cur,g3,-(tension-1.0f));

//qn.ra = slerp(pn, p(n-1), (1-tension)*(1-continuity*bias)/3.0);

//            if (i == rot.main.num - 1)
              slerp(seg.c1,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);

//            if (i+1 != rot.main.num - 1)
//            {
//              prev = cur;
//              cur  = next;
//              next = &anim [i+1].v2;

//              slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
//              slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
//              slerp (g3,g1,g2,0.5f + 0.5f * continuity);
//              slerp (g4,g1,g2,0.5f - 0.5f * continuity);
////              slerp (seg.c2,*cur,g3,(tension-1.0f));
//              slerp (seg.c2,*cur,g3,-(tension-1.0f));
//            }
//            else
//              slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);
          }
          else
          {
            quat_t* cur  = &seg.v1;
            quat_t* next = &seg.v2;
            quat_t* prev = NULL;

//q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);

//            slerp(seg.c1,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);
            slerp(seg.c2,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);

            if (i+1 != rot.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = &anim [i+1].v2;

              slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
              slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
              slerp (g3,g1,g2,0.5f + 0.5f * continuity);
              slerp (g4,g1,g2,0.5f - 0.5f * continuity);
//              slerp (seg.c2,*cur,g3,(tension-1.0f));
              slerp (seg.c2,*cur,g3,-(tension-1.0f));
            }
            else
              slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);
          }
        }
        else
        {
          seg.c1         = quat_t ();
          seg.c2         = quat_t ();
          seg.end        = rot.rot [i].frame;
          seg.locTime    = 1.0f;
        }            
      }  
    }

    pool->free (r);
    pool->free (rot.rot);

    return anim;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at LoadQuatAnim "<<endl;
    #endif
    return NULL;
  }
}

/*          else
          {
            quat_t* cur  = &seg.v1;
            quat_t* next = &seg.v2;
            quat_t* prev = NULL;

//cur.ra = slerp(cur, g3, (tension-1));
//cur.rb = slerp(cur, g3, -(tension-1));
//q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);
//qn.ra = slerp(pn, p(n-1), (1-tension)*(1-continuity*bias)/3.0);

//            slerp(seg.c1,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);
            slerp(seg.c2,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);

            if (i+1 != rot.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = &anim [i+1].v2;

              slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
              slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
              slerp (g3,g1,g2,0.5f + 0.5f * continuity);
              slerp (g4,g1,g2,0.5f - 0.5f * continuity);
              slerp (seg.c2,*cur,g3,(tension-1.0f));
            }
            else
              slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);
          }*/