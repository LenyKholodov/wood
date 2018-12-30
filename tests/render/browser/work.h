class Browser
{
  class Render
  {
    TextureManager;
    BufferManager;
    MemoryManager;    

    class Scene
    {
      class Node;

      Camera* CreateCamera ();
      Light*  CreateLight  ();
      Shape*  CreateShape  (ShapeType,FillMode);
      ParticleSystem* CreateParticleSystem (???);
      Mesh*   CreateMesh   ();
      Curve*  CreateCurve  ();
      Patch*  CreatePatch  ();
    };

    class Frame
    {
      Scene*   scene;
      Camera*  eye;

      ObjectList; //???
    };

    Material
    {
      Texture
      {
        Texture1D, Texture2D, Texture3D, TextureCubeMap;
      };   
    };
    VertexBuffer;
    IndexBuffer;
    Shadow;
  };

  class SoundPlayer
  {
  };

  class Input
  {
  };
};
