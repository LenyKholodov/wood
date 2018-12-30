  MODELKEEPER kpr = me->BindKeeper (new (M_GetPool()) SimpleModelKeeper (M_GetPool(),me,RENDER_SMPMODEL_MESH_NUM));           
                   
  MODELDATA  md [3] = {
    me->LoadModelData (kpr,FALSE,"models\\tree.mdl"),
    me->LoadModelData (kpr,FALSE,"models\\tree0.mdl"),
    me->LoadModelData (kpr,FALSE,"models\\tree1.mdl")
  };  

  MODEL      mdl [4] = {
    me->CreateModel (md [0],c+vector3d_t (12,0,0),matrix_t (1)),
    me->CreateModel (md [1],c+vector3d_t (0,4,0),matrix_t (1)),
    me->CreateModel (md [2],c+vector3d_t (0,0,4),matrix_t::scale (vector3d_t (0.1))),
    me->CreateModel (md [2],c+vector3d_t (0,0,-4),matrix_t::scale (vector3d_t (0.1)))
  };    