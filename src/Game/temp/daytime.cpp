  static GLint   amb [24][4] = {
          {54, 83, 147, 1},
          {54, 83, 147, 1},
          {87, 89, 110, 1},
          {122, 81, 68, 1},
          {133, 96, 85, 1},
          {135, 117, 94, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {129, 135, 125, 1},
          {135, 117, 94, 1},
          {133, 96, 85, 1},
          {122, 81, 68, 1},
          {87, 89, 110, 1},
          {54, 83, 147, 1}
    };

  static GLint sunLight [24][4] = {
           {63, 93, 157, 1},
           {63, 93, 157, 1},
           {92, 122, 165, 1},
           {171, 131, 119, 1},
           {255, 140, 104, 1},
           {255, 198, 129, 1},
           {255, 248, 207, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 255, 255, 1},
           {255, 248, 207, 1},
           {255, 202, 136, 1},
           {255, 123, 82, 1},
           {166, 128, 116, 1},
           {92, 122, 165, 1},
           {63, 93, 157, 1}
    };           
    
  static GLint sky [24][4] = {
      {0, 0, 118, 1},
      {0, 0, 118, 1},
      {76, 78, 97, 1},
      {151, 80, 58, 1},
      {196, 89, 56, 1},
      {175, 150, 126, 1},
      {138, 176, 177, 1},
      {53, 182, 198, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {46, 181, 217, 1},
      {103, 181, 183, 1},
      {174, 168, 157, 1},
      {210, 154, 101, 1},
      {210, 95, 36, 1},
      {136, 81, 81, 1},
      {76, 78, 97, 1},
      {0, 0, 118, 1}
  };
  

void     SetDayTime ()
{
  static  float   time1 = 0;
  static  float   step  = 0.08f;
  static  clock_t last  = clock ();

  if (clock () - last > CLK_TCK >> 4)
  {
    time1 += step;

    if (time1 >= 24.0f) time1 = 0.0f;

    GLfloat x [4] = {1.0f};
    GLfloat y [4] = {0.0f};
    GLfloat z [4] = {0};
    int     hour  = time1;

    LinearInterpolation (x,time1 - float(hour),sunLight [hour],sunLight [(hour+1)%24]);
    LinearInterpolation (y,time1 - float(hour),sky [hour],sky [(hour+1)%24]);
    LinearInterpolation (z,time1 - float(hour),amb [hour],amb [(hour+1)%24]);

    glLightfv (GL_LIGHT0,GL_DIFFUSE,x);
    glLightfv (GL_LIGHT0,GL_SPECULAR,x);
    glLightfv (GL_LIGHT0,GL_AMBIENT,z);

    float k = sqrt (x [0] * x [0] + x [1] * x [1] + x [2] * x [2]) / sqrt (3.0f * 0.5f * 0.5f);

//    glFogf    (GL_FOG_END,RENDER_VIEW_DEPTH*0.55*k*k);
//    glFogf    (GL_FOG_START,RENDER_VIEW_DEPTH*0.1*k*k);
//    glFogf    (GL_FOG_MODE,GL_EXP);
//    glFogf    (GL_FOG_DENSITY,0.01f);

    if (glIsEnabled (GL_FOG))
    {
      GLfloat fogColor [4] = {0.5,0.5,0.5,1.0};

      glFogi      (GL_FOG_MODE, GL_EXP2);
      glFogfv     (GL_FOG_COLOR, fogColor); 
      glFogf      (GL_FOG_DENSITY, 0.05f);
      glHint      (GL_FOG_HINT, GL_DONT_CARE);

      render->GetSkyBox()->SetSkyColor (vector3d_t (0.5f));

      glClearColor (0.5f,0.5f,0.5f,0.5);
    }  
    else
    {
      glClearColor     (y [0],y [1],y [2],0.5);
      render->GetSkyBox()->SetSkyColor (vector3d_t (y [0],y [1],y [2]));
    }  

    last = clock ();
  }
}

