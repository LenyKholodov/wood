class Buffer
{
  public:
    typedef void* map_ptr;
    typedef uint offset_t;

    enum Access { READ_ONLY, WRITE_ONLY, READ_WRITE };

    map_ptr MapBuffer    (offset_t,size_t,Access); //sync for read
    void    UnmapBuffer  (map_ptr); //async
    void    UpdateMap    (map_ptr); //sync

    void    SetData      (offset_t,size_t,const void* data); //async
    void    GetData      (offset_t,size_t,void* data); //sync
};

class Render //interface
{
  public:  
    void DrawMesh      (const Mesh&);
    void DrawPatch     (const Patch&);
    void DrawSpline    (const Spline&);
    void DrawBillboard (const Billboard&);

    void SetPass       (uint pass);
    void SetVertexes   (const VB*);
    void SetIndeces    (const IB*);
    void SetMaterial   (const Material*);
    void SetClipPlanes (uint num,plane_t*);
    void SetLightZone  (???);
    void SetTransform  (const Matrix&);

    void BeginEnum     ();
    void EndEnum       ();

    void Clear         (mask);

    VB*  CreateVB      ();
    IB*  CreateIB      ();
    Texture2D* CreateTexture2D ();
    Texture3D* CreateTexture3D ();
    TextureCubemap* CreateTextureCubemap ();
      //etc.

    void BindTarget    (DisplayList&);
    void BindTarget    (ColorBuffer&);
    void BindTarget    (VB&);

    void Enable        (mode);
    void Disable       (mode);

//    void Set           ();
//    void Get           ();
};
