  static clock_t last1 = clock ();

  if (clock() - last1 > CLK_TCK)
  {
    static float angle = 0;
    static float astep = 0.5f;
    static float RADIUS = 15;
    
    lightsrc_t l = ls;

    l.light.pos.x = ls.light.pos.x + sin (angle) * RADIUS;
    l.light.pos.z = ls.light.pos.z + cos (angle) * RADIUS;
    l.light.dir   = normalize (render->GetModelEnv ()->GetBound (mdl [0]).sphere.center - l.light.pos);

    render->GetLightEnv()->ChangeLight   (light,l);

    angle += astep;

    last1  = clock ();
  }  
