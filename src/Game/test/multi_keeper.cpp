
  MODELKEEPER kpr = me->BindKeeper (new (M_GetPool()) MultiModelKeeper (M_GetPool(),me,RENDER_SMPMODEL_MESH_NUM));                   
  MODELDATA   md  = me->LoadModelData (kpr,FALSE,"tree3.mdl");
  MODEL       mdl = me->CreateModel (md,c+vector3d_t (12,0,0),matrix_t::scale (vector3d_t (0.02)));