class Program
{
  public:
    class Data;

    void (*Filter)(uint count,const Primitive* primitives,
                   const Attrib* attributes,
                   Context& context                   
                  );

    void NewProgram ();
    void EndProgram ();    

    void UseFilter  (uint stage_name,Filter);
};

class Accum
{
  public:
    
};