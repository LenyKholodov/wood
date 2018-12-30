class MediaManager
{
  public:
        Bitmap*   LoadBitmap (const char*);
        Font*     LoadFont   (const char*);
        Sound*    LoadSound  (const char*);
        Video*    LoadVideo  (const char*);
};

struct Bitmap
{
  uint  width, height, bpp;
  void* data;  
};

struct Font
{
  //attribs
  HFont font;
};

class Sound
{
  public:
    void read (void*,size_t);
};

struct Video
{
  public:
    void GetVideo (???);

  private:
    SysHandle* handle;
};

struct Script
{
  public:
    VCode* GetCode ();
};

struct Table
{
  public:
    void GetValue (const char*,T&)
};
