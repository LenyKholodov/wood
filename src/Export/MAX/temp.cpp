class    ExportNode;

typedef Ring<ExportNode*>       ExportList;

/////////////////////////////////////////////////////////////////////////////////////////////
///Протокласс для экспортеров
/////////////////////////////////////////////////////////////////////////////////////////////
class    ExportNode: public MemManager
{
  public:
                      ExportNode (ExportNode* owner);
        virtual       ~ExportNode ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Управление памятью
/////////////////////////////////////////////////////////////////////////////////////////////
        void*         operator new (size_t size,Pool* pool = mExportPool) { return MemManager::operator new (size,pool); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Экспорт
/////////////////////////////////////////////////////////////////////////////////////////////
                void  dump      (ostream&,uint flags);
                uint  flags     () const { return mFlags | NodeFlags (); }

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Дамп в потомке \ информация об экспорте в потомке
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void  DumpNode  (ostream&,uint flags) = 0;
        virtual uint  NodeFlags () const = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
///Управление памятью
/////////////////////////////////////////////////////////////////////////////////////////////
        Pool*         GetPool   () const { return mExportPool; }
        Pool*         M_GetPool () const { return mExportPool; }

  private:
        void          insert    (ExportNode*);
        void          erase     (ExportNode*);
        
        uint          ComputeFlags () const;
        uint          ComputeFlags (ExportNode*) const;

  protected:
        ExportList      mExportList;
        ExportNode*     mExportOwner;

  private:
        uint            mFlags;

        static Pool*    mExportPool;
        static bool     mFlagsInherit;
};

///////////////////////////////////////////////////////////////////////////////////////

void  ExportNode::dump      (ostream& os,uint f)
{
  if (f & NodeFlags ()) DumpNode (os,f & NodeFlags ());

  ExportList::iter iter  = mExportList;
  int              count = mExportList.count ();

  for (int i=0;i<count;i++,iter++)
    if (iter->flags () & f)
      iter->dump (os,f & iter->flags ());
}

uint  ExportNode::ComputeFlags (ExportNode* node) const
{
  uint flags = NodeFlags ();

  ExportList::iter iter  = mExportList;
  int              count = mExportList.count (); 

  for (int i=0;i<count;i++,iter++)
    flags |= ComputeFlags (iter);

  return flags;
}

uint  ExportNode::ComputeFlags () const
{
  return mFlags = ComputeFlags (this);
}

void  ExportNode::insert    (ExportNode* node)
{
  if (node)
  {
    mExportList.insert (node);
    node->mExportOwner = this;

    if (mFlagsInherit)
    {
      uint f = node->flags ();

      do
      {
        node                = node->mExportOwner;
        node->mExportFlags |= f;      
      }while (node);
    }
  }  
}

void  ExportNode::erase     (ExportNode* node)
{
  if (node && node->mExportOwner == this)
  {
    node->mExportOwner = NULL;
    mExportList.erase (node);

    if (mFlagsInherit)
      ComputeFlags ();
  }
}

ExportNode::ExportNode (ExportNode* owner)
           : mExportList  (mExportPool),        
             mExportOwner (owner),
             mFlags       (0)             
{
  if (owner) 
    owner->insert (this);  
}

ExportNode::~ExportNode ()
{
  uint oldState = mFlagsInherit;

  mFlagsInherit = false;

  ExportList::iter iter  = mExportList;
  int              count = mExportList.count ();

  for (int i=0;i<count;i++,iter++)
    delete iter;

  mFlagsInherit = oldState;

  if (mExportOwner)
    mExportOwner->erase (this);      
}


ostream& operator << (ostream& os,const Indent& i)
{
  return os<<tab (i);
}

void   BaseExport::OpenFrame   (const char* tag_name)
{
  if (tag_name)
  {
    dump<<indent<<tag_name<<" {"<<endl;
  }  
}

bool   BaseExport::CloseTag  ()
{
  if (mOwner)
  {
    if (indent > mOwner->indent + 1)   
    {
      dump<<indent<<"}"<<endl;
      return true;
    }  
    else return false;
  }
  else 
  {
    if (indent) 
    {
      dump<<indent<<"}"<<endl;   
      return true;
    }  
    else return false;
  }  
}


/////////////////////////////////////////////////////////////////////////////////////////////
///Корневой экспортёр
/////////////////////////////////////////////////////////////////////////////////////////////
class RootExport: public BaseExport
{
  public:
                RootExport (ostream&
                           );
//                RootExport (const char* log_out, 
//                            Pool*       pool = ::M_GetPool (),
//                            size_t      size = 0
//                           );
//    virtual     ~RootExport ();

//    Interface&        GetInterface () { return *mInterface; }
//    const Interface&  GetInterface () const { return *mInterface; }

//    ExpInterface&        GetExpInterface () { return *mExpInterface; }
//    const ExpInterface&  GetExpInterface () const { return *mExpInterface; }

//    Interface*     mInterface;
//    ExpInterface*  mExpInterface;
};


RootExport::RootExport (ostream& os,Interface* i,ExpInterface* ei)
           : BaseExport (os,mInfo),
             mOs   (log), 
             mInfo (i,ei,)
{
}



template <class KeyType>

        else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) {
                int numKeys;
                if (numKeys = ikc->GetNumKeys()) {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_LINEAR); 
                        for (i=0; i<numKeys; i++) {
                                ILinScaleKey key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
                                        indent.data(),
                                        ID_SCALE_KEY,
                                        key.time,
                                        Format(key.val));
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }

        else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) {
                int numKeys;
                if (numKeys = ikc->GetNumKeys()) {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_ROT_LINEAR); 
                        for (i=0; i<numKeys; i++) {
                                ILinRotKey key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
                                        indent.data(), 
                                         // Quaternions are converted to AngAxis when written to file
                                        ID_ROT_KEY,
                                        key.time,
                                        Format(key.val));
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }

        else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) {
                int numKeys;
                if(numKeys = ikc->GetNumKeys())
                {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_LINEAR); 
                        for (i=0; i<numKeys; i++) {
                                ILinPoint3Key key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
                                        indent.data(),
                                        ID_POS_KEY,
                                        key.time,
                                        Format(key.val));
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }




        // Bezier position
        else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
        {
                int numKeys;
                if(numKeys = ikc->GetNumKeys())
                {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_BEZIER); 
                        for (i=0; i<numKeys; i++) {
                                IBezPoint3Key key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s",
                                        indent.data(), 
                                        ID_BEZIER_POS_KEY,
                                        key.time,
                                        Format(key.val));
                                fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }

        else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
        {
                int numKeys;
                if (numKeys = ikc->GetNumKeys()) {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_ROT_BEZIER); 
                        for (i=0; i<numKeys; i++) {
                                IBezQuatKey key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s\n", 
                                        indent.data(),
                                         // Quaternions are converted to AngAxis when written to file
                                        ID_ROT_KEY,
                                        key.time,
                                        Format(key.val));
                                // There is no intan/outtan for Quat Rotations
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }

        else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) {
                int numKeys;
                if (numKeys = ikc->GetNumKeys()) {
                        fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_BEZIER); 
                        for (i=0; i<numKeys; i++) {
                                IBezScaleKey key;
                                ikc->GetKey(i, &key);
                                fprintf(pStream, "%s\t\t\t%s %d\t%s",
                                        indent.data(),
                                        ID_BEZIER_SCALE_KEY,
                                        key.time,
                                        Format(key.val));
                                fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
                        }
                        fprintf(pStream,"%s\t\t}\n", indent.data());
                }
        }

     ostream&   ExportPos    (const ITCBPoint3Key&);
     ostream&   ExportPos    (const ILinPoint3Key&);
     ostream&   ExportPos    (const IBezPoint3Key&);

     ostream&   ExportRot    (const ITCBRotKey&);
     ostream&   ExportRot    (const IBezRotKey&);
     ostream&   ExportRot    (const ILinRotKey&);

     ostream&   ExportScale  (const ITCBScaleKey&);
     ostream&   ExportScale  (const IBezScaleKey&);
     ostream&   ExportScale  (const ILinScaleKey&);

bool ExportAnimKeys::CheckForAnimation (bool& isPos,bool& isRot,bool& isScale)
{
  isPos = isRot = isScale = false;

  TimeValue start = info.i->GetAnimRange ().Start ();
  TimeValue end   = info.i->GetAnimRange ().End ();

  Matrix3      tm;
  AffineParts  ap;
  Point3       firstPos;
  float        rotAngle, firstRotAngle;
  Point3       rotAxis, firstRotAxis;
  Point3       firstScaleFactor;

  for (TimeValue t=start;t<=end;t++) 
  {
          tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

          decomp_affine(tm, &ap);

          AngAxisFromQ(ap.q, &rotAngle, rotAxis);

          if (t != start) {
                  if (!isPos) {
                          if (!EqualPoint3(ap.t, firstPos)) {
                                  isPos = TRUE;
                                  }
                          }
                  // MAX 2.x:
                  // We examine the rotation angle to see if the rotation component
                  // has changed.
                  // Although not entierly true, it should work.
                  // It is rare that the rotation axis is animated without
                  // the rotation angle being somewhat affected.
                  // MAX 3.x:
                  // The above did not work, I have a repro scene that doesn't export a rotation track
                  // because of this. I fixed it to also compare the axis.
                  if (!isRot) {
                          if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO) {
                                  isRot = TRUE;
                                  }
                          else if (!EqualPoint3(rotAxis, firstRotAxis)) {
                                  isRot = TRUE;
                                  }
                          }

                  if (!isScale) {
                          if (!EqualPoint3(ap.k, firstScaleFactor)) {
                                  isScale = TRUE;
                                  }
                          }
                  }
          else {
                  firstPos = ap.t;
                  firstRotAngle = rotAngle;
                  firstRotAxis = rotAxis;
                  firstScaleFactor = ap.k;
                  }

          // No need to continue looping if all components are animated
          if (isPos && isRot && isScale)
                  break;
          }

  return isPos || isRot || isScale;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///Поток вывода
/////////////////////////////////////////////////////////////////////////////////////////////

class   ExportStream
{
  public:
                        ExportStream (ostream&); 

/////////////////////////////////////////////////////////////////////////////////////////////
///Далее небольшой трюк
/////////////////////////////////////////////////////////////////////////////////////////////
        template <class Type>
        ExportStream&   operator << (ExportStream&,const Type&);

  private:
                ostream&        log;                        
};

template <class Type>
ExportStream<Type>&   ExportStream<Type>::operator << (ExportStream<Type>& s,const Type& t)
{
  log<<t<<"\t";

  return *this;
}

void  MeshExport::TexVertexNFacesExport (Mesh& mesh,int* order)
{
  if (!mesh.getNumTVerts())
    return;

  bool res = false;

  if (info.bin.check (BIN_TEXCOORDS))
  {
    typedef uint tpatch_t [3];
    tcoord_t* tcoord = ext_new tcoord_t [mesh.getNumTVerts()];
    tpatch_t* index  = ext_new tpatch_t [mesh.getNumFaces()];

    if (tcoord && index)
    {
      tcoord_t* t      = tcoord;
      int       numTVx = mesh.getNumTVerts();

      for (int i=0;i<numTVx;i++,t++) 
      {
        t->u = mesh.tVerts [i].x;
        t->v = 1.0-mesh.tVerts [i].y;
      }            

      tpatch_t* ind = index;

      for (i=0;i<mesh.getNumFaces();i++,ind++) 
      {
        (*ind) [0] = mesh.tvFace[i].t[order [0]];
        (*ind) [1] = mesh.tvFace[i].t[order [1]];
        (*ind) [2] = mesh.tvFace[i].t[order [2]];
      }

      OpenFrame ("tex_coords");    
      dump.WriteArray ("tex_coord",tcoord,numTVx);
      CloseFrame ();
      OpenFrame ("texmap_faces");
      dump.WriteArray ("tex_index",index,mesh.getNumFaces());
      CloseFrame ();

      delete [] index;
      delete [] tcoord;

      res = true;
    }
    else
    {
      if (tcoord) delete [] tcoord;
      if (index)  delete [] index;
    }    
  }

  if (!res)
  {
    int numTVx = mesh.getNumTVerts();

    OpenFrame ("tex_coords");    

    for (int i=0;i<numTVx;i++) 
    {
      UVVert tv = mesh.tVerts [i];
      dump<<"tex_coord "<<tv.x<<"\t"  <<(1-tv.y)<<endline;
    }

    CloseFrame (); 

    OpenFrame ("texmap_faces");

    for (i=0;i<mesh.getNumFaces(); i++) 
    {
      dump<<"tex_index "<<mesh.tvFace[i].t[order [0]]
          <<"\t"<<mesh.tvFace[i].t[order [1]]
          <<"\t"<<mesh.tvFace[i].t[order [2]]
          <<endline;
    }

    CloseFrame ();     
  }    
}