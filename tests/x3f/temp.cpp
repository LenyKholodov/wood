void CALLBACK   Draw    ()
{
//  glMatrixMode (GL_PROJECTION);        
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f (1,0,0);     

  Object3D::Node node  = obj->main ().node ();
  int            first = 0;
  int            count = 30000;  

  for (int k=0;k<first && node.valid();node = node.next(),k++);

  for (first=0;node.valid() && first<count;node=node.next (),first++)
  {
    if (!node.facet () || !node.vertex ())
      continue;

    glBegin (GL_TRIANGLES);
      facet_t*   f = node.facet  ();
      vertex_t*  v = node.vertex ();

      for (int i=0;i<node.count (Object3D::FACET);i++,f++)
      {
        facet_t&          facet = *f;
        const material_t* m     = &node.owner()->GetMaterial(facet.texture).m;

        if (facet.texture == -1) m = &mat;

        for (int j=0;j<3;j++)
        {
          vertex_t& vertex = v [facet.index [j]];
          glColor3fv ((GLfloat*)&env.compute (vertex,facet.n,*m));
          glVertex3f (vertex.x,vertex.y,vertex.z);
        }
      }
  }

  glEnd   ();
  glFinish ();

  auxSwapBuffers ();
}


void            Draw    (Object3D::Node node)
{
  glPushMatrix ();

  animseg_t* anim = (animseg_t*)node.anim ();

  if (anim)
  {
    vecseg_t   *trTrans = (vecseg_t*)node.trans (),  
               *trScale = (vecseg_t*)node.scale ();
    quatseg_t* trRot    = (quatseg_t*)node.rotate ();
    vector3d_t scale, trans;
    vector3d_t rot;
    float      angle;
    matrix_t   m        = anim->start;
        
    for (int i=0;i<anim->trans.count && trTrans->end < frame;i++,trTrans=i<anim->trans.count?trTrans+1:trTrans);
    for (i=0    ;i<anim->scale.count && trScale->end < frame;i++,trScale=i<anim->scale.count?trScale+1:trScale);
    for (i=0    ;i<anim->rot.count && trRot->end < frame;i++,trRot=i<anim->rot.count?trRot+1:trRot);

    Recalc (rot,angle,*trRot);
    Recalc (trans,*trTrans);
    Recalc (scale,*trScale);

    glTranslatef   (-anim->offs.x,-anim->offs.y,-anim->offs.z);
    m.transpose    ();
    glMultMatrixf  ((GLfloat*)&m);
    glRotatef      (angle*57.3f,rot.x,rot.y,rot.z);

/*    glDisable      (GL_LIGHTING);
    glPointSize   (10);
    glBegin (GL_POINTS);
      glColor3f  (1,1,1);
      glVertex3f (-anim->pivot.x,-anim->pivot.y,-anim->pivot.z);
    glEnd   ();*/

    glTranslatef   (anim->offs.x,anim->offs.y,anim->offs.z);

//    glTranslatef   (-anim->offs.x,-anim->offs.y,-anim->offs.z);

//    glTranslatef   (-anim->offs.x,-anim->offs.y,-anim->offs.z);

//    glRotatef      (angle*57.3f,rot.x,rot.y,rot.z);

    glTranslatef   (-anim->pivot.x,-anim->pivot.y,-anim->pivot.z);    
    glTranslatef   (anim->offs.x,anim->offs.y,anim->offs.z);
    glTranslatef   (trans.x,trans.y,trans.z);
  }

  if (node.vertex() && node.facet())
  {
    vertex_t*  vertex = node.vertex ();
    facet_t*   facet  = node.facet  ();

    const material_t* m     = &node.owner()->GetMaterial(facet [0].texture).m;    

    if (m) glColor3fv ((GLfloat*)&m->diffuse);

    glBegin (GL_TRIANGLES);
      for (int i=0;i<node.count (Object3D::FACET);i++,facet++)
      {
        glNormal3fv ((GLfloat*)&facet->n);
        for (int j=0;j<3;j++)
        {
          glVertex3fv    ((GLfloat*)&vertex [facet->index [j]]);
          glTexCoord2fv ((GLfloat*)&facet->tindex [j]);
        }  
      }
    glEnd   ();
  }

  for (Object3D::Node iter = node.node();iter.valid();iter=iter.next ())
    Draw (iter);

  glPopMatrix ();
}


    m  *= matrix_t::translate (trans);

    if (i>=anim->rot.count) i = anim->rot.count-1;

    for (;i>=0;i--,trRot--)
      m  *= matrix_t::rotate    (trRot->rot[0].vec,trRot->rot[0].angle);

    m  *= matrix_t::translate (-anim->pivot);
    m  *= start;
    m  *= matrix_t::translate (anim->offs);






    m   *= parent;  //!!!!!

    matrix_t x (1);

    x   *= matrix_t::translate (trans);

    x   *= matrix_t::rotate    (rot,angle);
    x   *= matrix_t::translate (-anim->pivot);
    x   *= start;
    x   *= matrix_t::translate (anim->offs);

    m   *= x;

//    sam  = parent * 
//           matrix_t::translate (-anim->offs) *
//           matrix_t::rotate (rot,angle);
    sam  = parent * x;



//    if (panim)
    if (0)  
    {
      matrix_t inv = anim->start; inv.invert ();
//      matrix_t p   = panim->start * inv * matrix_t::translate (-(panim->offs-anim->offs));
      matrix_t p   = panim->start * inv * matrix_t::translate (panim->offs);

      m           *= parent * p ;//* matrix_t::translate (panim->pivot);
    }

          if (vertex [facet->index [j]].x < min.x) min.x = vertex [facet->index [j]].x;
          if (vertex [facet->index [j]].y < min.y) min.y = vertex [facet->index [j]].y;
          if (vertex [facet->index [j]].z < min.z) min.z = vertex [facet->index [j]].z;
          if (vertex [facet->index [j]].x > max.x) max.x = vertex [facet->index [j]].x;
          if (vertex [facet->index [j]].y > max.y) max.y = vertex [facet->index [j]].y;
          if (vertex [facet->index [j]].z > max.z) max.z = vertex [facet->index [j]].z;


    vertex_t min = vertex [facet->index [0]],
             max = vertex [facet->index [0]];
