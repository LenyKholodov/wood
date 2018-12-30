  ls.light.pos     = c + vector3d_t (35,1000,0);
  ls.light.dir     = vector3d_t (0,-1,0);
  ls.light.angle   = 3.14 / 2.0;
  ls.light.diffuse = vector3d_t (0.8);
  ls.light.farlight= FALSE;
  ls.shiness       = 1;

//  light  = render->GetLightEnv ()->NewLight (ls,TLE_GLOBAL_LIGHT|TLE_ENABLE_LIGHT);
  light  = render->GetLightEnv ()->NewLight (ls,TLE_GLOBAL_LIGHT);

  mdl [0] = me->CreateModel (md [0],c+vector3d_t (35,14,0),matrix_t::scale (vector3d_t (1)));
  mdl [5] = me->CreateModel (md [6],c+vector3d_t (35,24,20),matrix_t::scale (vector3d_t (1)));
  mdl [6] = me->CreateModel (md [5],c+vector3d_t (35,24,1),matrix_t::scale (vector3d_t (1)));
  mdl [3] = me->CreateModel (md [3],c+vector3d_t (35,20,4),matrix_t::scale (vector3d_t (1)));

  shadow = render->GetShadowEnv ()->CreateShadow (light,mdl [0]);
  render->GetShadowEnv ()->EnableShadow (shadow);
  render->GetShadowEnv ()->refresh      (shadow);

  shadow1 = render->GetShadowEnv ()->CreateShadow (light,mdl [5]);
  render->GetShadowEnv ()->EnableShadow (shadow1);
  render->GetShadowEnv ()->refresh      (shadow1);
//  render->GetShadowEnv ()->InsertModel  (shadow,mdl [0]);
//  render->GetShadowEnv ()->InsertModel  (shadow,mdl [6]);
//  render->GetShadowEnv ()->InsertModel  (shadow,mdl [3]);

//  mdl [0] = me->CreateModel (md [0],c-vector3d_t (-12,-6,70),matrix_t (1));
//  mdl [1] = me->CreateModel (md [1],c-vector3d_t (4,-4,14),matrix_t (1));
//  mdl [2] = me->CreateModel (md [2],c+vector3d_t (-                 70,0,-44),matrix_t (1));

//  mdl [4] = me->CreateModel (md [4],c+vector3d_t (12,8,12),0,4l,4l);

//  mdl [7] = me->CreateModel (md [7],c+vector3d_t (10,18,15),matrix_t (1));
//  mdl [8] = me->CreateModel (md [8],c+vector3d_t (-5,20,9),matrix_t (1));

SHADOW     shadow   = -1;
SHADOW     shadow1  = -1;
lightsrc_t ls;
LIGHTSRC  light;

  static clock_t last2 = clock ();

  if (clock() - last2 > CLK_TCK >> 3)
  {
    render->GetShadowEnv()->refresh (shadow);
    render->GetShadowEnv()->refresh (shadow1);
    last2 = clock ();
  }  