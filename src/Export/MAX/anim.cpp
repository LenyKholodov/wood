#include <pch.h>

#include "anim.h"
#include "node.h"

template <class KeyType>
AnimKeysExport::EnumKeys<KeyType>::EnumKeys (IKeyControl& ctrl,const char* type,ostream& dump)
{
  dump<<"type "<<type<<endline;

  KeyType key;
  int     count = ctrl.GetNumKeys ();

  for (int i=0;i<count;i++)
  {
    ctrl.GetKey (i,&key);

    dump<<"key \t"<<key<<endline;
  }
}

void   AnimKeysExport::ExportPosKeys   (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (!ikc)
    return ;
  
  if (ctrl.ClassID() != Class_ID (TCBINTERP_POSITION_CLASS_ID,0)   &&
      ctrl.ClassID() != Class_ID (HYBRIDINTERP_POSITION_CLASS_ID,0) &&
      ctrl.ClassID() != Class_ID (LININTERP_POSITION_CLASS_ID,0))
        return;

  OpenFrame ("pos");

  if (ctrl.ClassID () == Class_ID (TCBINTERP_POSITION_CLASS_ID,0))
      EnumKeys<ITCBPoint3Key> enumKeys (*ikc,"tcb",dump);

  if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_POSITION_CLASS_ID,0))
      EnumKeys<IBezPoint3Key> enumKeys (*ikc,"bezie",dump);

  if (ctrl.ClassID () == Class_ID (LININTERP_POSITION_CLASS_ID,0))
      EnumKeys<ILinPoint3Key> enumKeys (*ikc,"linear",dump);

  CloseFrame ();
}

void   AnimKeysExport::ExportRotKeys   (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (!ikc)
    return ;
  
  if (ctrl.ClassID() != Class_ID (TCBINTERP_ROTATION_CLASS_ID,0)   &&
      ctrl.ClassID() != Class_ID (HYBRIDINTERP_ROTATION_CLASS_ID,0) &&
      ctrl.ClassID() != Class_ID (LININTERP_ROTATION_CLASS_ID,0))
        return;

  OpenFrame ("rot");

  if (ctrl.ClassID () == Class_ID (TCBINTERP_ROTATION_CLASS_ID,0))
      EnumKeys<ITCBRotKey> enumKeys (*ikc,"tcb",dump);

  if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_ROTATION_CLASS_ID,0))
      EnumKeys<IBezQuatKey> enumKeys (*ikc,"bezie",dump);

  if (ctrl.ClassID () == Class_ID (LININTERP_ROTATION_CLASS_ID,0))
      EnumKeys<ILinRotKey> enumKeys (*ikc,"linear",dump);

  CloseFrame ();
}

void   AnimKeysExport::ExportScaleKeys   (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (!ikc)
    return ;
  
  if (ctrl.ClassID() != Class_ID (TCBINTERP_SCALE_CLASS_ID,0)   &&
      ctrl.ClassID() != Class_ID (HYBRIDINTERP_SCALE_CLASS_ID,0) &&
      ctrl.ClassID() != Class_ID (LININTERP_SCALE_CLASS_ID,0))
        return;

  OpenFrame ("scale");

  if (ctrl.ClassID () == Class_ID (TCBINTERP_SCALE_CLASS_ID,0))
      EnumKeys<ITCBScaleKey> enumKeys (*ikc,"tcb",dump);

  if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_SCALE_CLASS_ID,0))
      EnumKeys<IBezScaleKey> enumKeys (*ikc,"bezie",dump);

  if (ctrl.ClassID () == Class_ID (LININTERP_SCALE_CLASS_ID,0))
      EnumKeys<ILinScaleKey> enumKeys (*ikc,"linear",dump);

  CloseFrame ();
}

bool AnimKeysExport::IsKnownController (Control* ctrl)
{
  if (!ctrl) return false;

  ulong partA = ctrl->ClassID ().PartA (), 
        partB = ctrl->ClassID ().PartB ();

//  if (partB != 0x00)
  if (partB)
    return false;

  switch (partA) 
  {
    case TCBINTERP_POSITION_CLASS_ID:
    case TCBINTERP_ROTATION_CLASS_ID:
    case TCBINTERP_SCALE_CLASS_ID:
    case HYBRIDINTERP_POSITION_CLASS_ID:
    case HYBRIDINTERP_ROTATION_CLASS_ID:
    case HYBRIDINTERP_SCALE_CLASS_ID:
    case LININTERP_POSITION_CLASS_ID:
    case LININTERP_ROTATION_CLASS_ID:
    case LININTERP_SCALE_CLASS_ID:
      return true;
  }

  return false;
}

bool AnimKeysExport::TestAnim (Control* ctrl)
{
  return ctrl && IsKnownController (ctrl) && GetKeyControlInterface (ctrl) && 
         GetKeyControlInterface (ctrl)->GetNumKeys ();
}

bool AnimKeysExport::TestAnim (bool& isPos,bool& isRot,bool& isScale)
{
  isPos   = TestAnim (node.GetTMController()->GetPositionController());
  isRot   = TestAnim (node.GetTMController()->GetRotationController());
  isScale = TestAnim (node.GetTMController()->GetScaleController());

  return isPos || isScale || isRot;
}

AnimKeysExport::AnimKeysExport (INodeExport& n)
               : BaseExport (NULL,n),
                 node       (n.GetNode ())
{
  bool isPos    = false,
       isRot    = false,
       isScale  = false;

  if (!TestAnim (isPos,isRot,isScale))
    return;

  OpenFrame ("anim_keys");

  dump<<"name\t"<<node.GetName ()<<endline;
  
  if (isPos)    ExportPosKeys   (*node.GetTMController()->GetPositionController());
  if (isRot)    ExportRotKeys   (*node.GetTMController()->GetRotationController());
  if (isScale)  ExportScaleKeys (*node.GetTMController()->GetScaleController());

  CloseFrame ();
}

/////////////////////////AnimMesh//////////////////////////////////

/*AnimMeshExport::AnimMeshExport (GeomExport& own)
               : BaseExport (NULL,own)
{
  ObjectState os = node->EvalWorldState (info.staticFrame);

  if (!os.obj)
      return;

  Interval animRange = info.i->GetAnimRange   ();
  Interval objRange  = os.obj->ObjectValidity (info.staticFrame);

  if (!objRange.InInterval(animRange)) 
  {
    OpenFrame ("anim_mesh");    
    
    TimeValue t = animRange.Start();
    
    while (1) 
    {
      os       = node->EvalWorldState   (t);
      objRange = os.obj->ObjectValidity (t);
      t        = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
      
      ExportMesh (*this,node,t);
      
      if (objRange.End() >= animRange.End()) 
              break;
      else 
        t++; //???
//        t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
    }

    CloseFrame ();
  }
}
  */      