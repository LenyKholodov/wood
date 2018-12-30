struct   treeprecompheader_t
{
  size_t        size;
  
  ulong         modelsOffs;
};

  treemdlheader_t hdr;

  hdr.mat.offs  = mFile->getpos ();
  hdr.mat.count = matDict.count ();
  hdr.mdl.offs  = sizeof (treedesc_t) * count + sizeof (treemdlheader_t);
  hdr.mdl.count = count;

struct   treemdlheader_t
{
  struct {
    ulong         offs;
    ulong         count;
  }mat, mdl;
};

  int            count = comp.treecount ();
  treedesc_t*    desc  = new treedesc_t [count];
  ulong          start = mFile->getpos ();

//    BinFile*      dest = F_OpenFile (name,BinFile::OPEN_RW);