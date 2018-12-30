Unit Walk_6;
INTERFACE
{***************************************************************************}
type Stage = record
              x, y : Integer;
              al, be, ga : Real;
             end;
     Angle = record
              nom, min, max : Real;
             end;
     Stage1 = record
               x, y : Integer;
               L : Word;
               al, be, ga : Angle;
              end;
     Stage2 = record
               x, y : Integer;
               L : Word;
               al, be, ga : Angle;
               Lcolor, Rcolor : Byte;
              end;
     Hnd = record
             Tors, Shld, FArm, Hand : Stage1;
{             Hand_L, FArm_L, Shld_L, Tors_L : Word;}
             Lcolor, Rcolor : Byte;
            end;
     Leg = record
            Tazz, Knee, Foot, Fing, Edge : Stage1;
 {           Tazz_L, Knee_L, Foot_L, Fing_L, Edge_L : Word;}
            Lcolor, Rcolor : Byte;
           end;
var GD, GM: Integer;
    GC : Stage;
    Chst, Neck, Head, Toth : Stage2;
    RH, LH : Hnd;
    RL, LL : Leg;
    wVel, Pidiv2 : Real;
    Land : Word;
    WalkStart, LLeg_forw : Boolean;
const Yes : Boolean = True;
       No : Boolean = False;
procedure Rect(x,y,dx,dy:Word);
procedure Incr(var St:Angle;da:Real);
procedure Decr(var St:Angle;da:Real);
{procedure ShowHand(H:Hnd);}
procedure TorsStage(var Pt:Hnd);
procedure ShldStage(var Pt:Hnd);
procedure FArmStage(var Pt:Hnd);
procedure HandStage(var Pt:Hnd);
procedure ShowLeg(L:Leg);
procedure TazzStage(var Pt:Leg);
procedure KneeStage(var Pt:Leg);
procedure FootStage(var Pt:Leg);
procedure FingStage(var Pt:Leg);
procedure EdgeStage(var Pt:Leg);
procedure NeckStage;
procedure ChstStage;
procedure HeadStage;
procedure TothStage;
procedure ShowStages;
{procedure InitSceleton;
procedure WalkHands;}
procedure Wait(count:Longint);
{*************************************************************************}
IMPLEMENTATION
{*************************************************************************}
uses Graph;
{*************************************************************************}
procedure Rect(x,y,dx,dy:Word);
begin Rectangle(x,y,x+dx,y+dy); end;
{***************************************************************************}
procedure Incr(var St:Angle;da:Real);
var r : Real;
begin
 r:=St.nom+da;
 if r>St.max
  then St.nom:=St.max
  else St.nom:=r;
end;
{***************************************************************************}
procedure Decr(var St:Angle;da:Real);
var r : Real;
begin
 r:=St.nom-da;
 if r<St.min
  then St.nom:=St.min
  else St.nom:=r;
end;
{***************************************************************************}
procedure TorsStage(var Pt:Hnd);
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Neck.al.nom+Pt.Tors.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Pt.Tors.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+Pt.Tors.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Tors.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Tors.L*sin(al)*cos(ga);

 Pt.Tors.x:=Round(Neck.x+x);
 Pt.Tors.y:=Round(Neck.y+y);
end;                         {TorsStage}
{***************************************************************************}
procedure ShldStage(var Pt:Hnd);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Neck.al.nom+Pt.Tors.al.nom+Pt.Shld.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Pt.Tors.be.nom+Pt.Shld.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+{Pt.Tors.ga.nom+}Pt.Shld.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Shld.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Shld.L*sin(al)*cos(ga);

 Pt.Shld.x:=Round(Pt.Tors.x+x);
 Pt.Shld.y:=Round(Pt.Tors.y+y);
end;                         {ShldStage}
{***************************************************************************}
procedure FArmStage(var Pt:Hnd);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Neck.al.nom+Pt.Tors.al.nom+Pt.Shld.al.nom+Pt.FArm.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Pt.Tors.be.nom+Pt.Shld.be.nom+Pt.FArm.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+{Pt.Tors.ga.nom+}Pt.Shld.ga.nom+Pt.FArm.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.FArm.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.FArm.L*sin(al)*cos(ga);

 Pt.FArm.x:=Round(Pt.Shld.x+x);
 Pt.FArm.y:=Round(Pt.Shld.y+y);
end;                         {FArmStage}
{***************************************************************************}
procedure HandStage(var Pt:Hnd);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Neck.al.nom+Pt.Tors.al.nom+Pt.Shld.al.nom+Pt.FArm.al.nom
          +Pt.Hand.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Pt.Tors.be.nom+Pt.Shld.be.nom+Pt.FArm.be.nom
          +Pt.Hand.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+{Pt.Tors.ga.nom+}Pt.Shld.ga.nom+Pt.FArm.ga.nom
          +Pt.Hand.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Hand.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Hand.L*sin(al)*cos(ga);

 Pt.Hand.x:=Round(Pt.FArm.x+x);
 Pt.Hand.y:=Round(Pt.FArm.y+y);
end;                         {HandStage}
{***************************************************************************}
procedure ShowLeg(L:Leg);
var x1, x2, y1, y2 : Integer;
begin
 {Tazz}
 SetColor(L.Rcolor); Rect(L.Tazz.x-1,L.Tazz.y-1,  2,  2);
 SetColor(L.Lcolor); Line(GC.x,GC.y,L.Tazz.x,L.Tazz.y);
 {Bedr}
 SetColor(L.Rcolor); Rect(L.Knee.x-1,L.Knee.y-1,  2,  2);
 SetColor(L.Lcolor); Line(L.Tazz.x,L.Tazz.y,L.Knee.x,L.Knee.y);
 {Foot}
 SetColor(L.Rcolor); Rect(L.Foot.x-1,L.Foot.y-1,  2,  2);
 SetColor(L.Lcolor); Line(L.Knee.x,L.Knee.y,L.Foot.x,L.Foot.y);
 {Fing}
 SetColor(L.Rcolor); Rect(L.Fing.x-1,L.Fing.y-1,  2,  2);
 SetColor(L.Lcolor); Line(L.Foot.x,L.Foot.y,L.Fing.x,L.Fing.y);
 {Edge}
 SetColor(L.Rcolor); Rect(L.Edge.x-1,L.Edge.y-1,  2,  2);
 SetColor(L.Lcolor); Line(L.Fing.x,L.Fing.y,L.Edge.x,L.Edge.y);
end;
{***************************************************************************}
procedure TazzStage(var Pt:Leg);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Pt.Tazz.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Pt.Tazz.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Pt.Tazz.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Tazz.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Tazz.L*sin(al)*cos(ga);

 Pt.Tazz.x:=Round(GC.x+x);
 Pt.Tazz.y:=Round(GC.y+y);
end;                         {TazzStage}
{***************************************************************************}
procedure KneeStage(var Pt:Leg);
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Pt.Tazz.al.nom+Pt.Knee.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Pt.Tazz.be.nom+Pt.Knee.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+{Pt.Tazz.ga.nom+}Pt.Knee.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Knee.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Knee.L*sin(al)*cos(ga);

 Pt.Knee.x:=Round(Pt.Tazz.x+x);
 Pt.Knee.y:=Round(Pt.Tazz.y+y);
end;                         {KneeStage}
{***************************************************************************}
procedure FootStage(var Pt:Leg);
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Pt.Tazz.al.nom+Pt.Knee.al.nom+Pt.Foot.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Pt.Tazz.be.nom+Pt.Knee.be.nom+Pt.Foot.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+{Pt.Tazz.ga.nom+}Pt.Knee.ga.nom+Pt.Foot.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Foot.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Foot.L*sin(al)*cos(ga);

 Pt.Foot.x:=Round(Pt.Knee.x+x);
 Pt.Foot.y:=Round(Pt.Knee.y+y);
end;                         {FootStage}
{***************************************************************************}
procedure FingStage(var Pt:Leg);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Pt.Tazz.al.nom+Pt.Knee.al.nom+Pt.Foot.al.nom+Pt.Fing.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Pt.Tazz.be.nom+Pt.Knee.be.nom+Pt.Foot.be.nom+Pt.Fing.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+{Pt.Tazz.ga.nom+}Pt.Knee.ga.nom+Pt.Foot.ga.nom+Pt.Fing.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Fing.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Fing.L*sin(al)*cos(ga);

 Pt.Fing.x:=Round(Pt.Foot.x+x);
 Pt.Fing.y:=Round(Pt.Foot.y+y);
end;                         {FingStage}
{***************************************************************************}
procedure EdgeStage(var Pt:Leg);
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Pt.Tazz.al.nom+Pt.Knee.al.nom+Pt.Foot.al.nom+Pt.Fing.al.nom
     +Pt.Edge.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Pt.Tazz.be.nom+Pt.Knee.be.nom+Pt.Foot.be.nom+Pt.Fing.be.nom
     +Pt.Edge.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+{Pt.Tazz.ga.nom+}Pt.Knee.ga.nom+Pt.Foot.ga.nom+Pt.Fing.ga.nom
     +Pt.Edge.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Pt.Edge.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Pt.Edge.L*sin(al)*cos(ga);

 Pt.Edge.x:=Round(Pt.Fing.x+x);
 Pt.Edge.y:=Round(Pt.Fing.y+y);
end;                         {EdgeStage}
{***************************************************************************}
procedure NeckStage;
var al, be, ga, x, y : Real;
begin
 al:=GC.al+Neck.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Neck.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Neck.L*sin(al)*cos(ga);

 Neck.x:=Round(GC.x+x);
 Neck.y:=Round(GC.y+y);
end;                         {NeckStage}
{***************************************************************************}
procedure ChstStage;
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Neck.al.nom+Chst.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Chst.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+Chst.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Chst.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Chst.L*sin(al)*cos(ga);

 Chst.x:=Round(Neck.x+x);
 Chst.y:=Round(Neck.y+y);
end;                         {ChstStage}
{***************************************************************************}
procedure HeadStage;
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Neck.al.nom+Head.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Head.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+Head.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Head.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Head.L*sin(al)*cos(ga);

 Head.x:=Round(Neck.x+x);
 Head.y:=Round(Neck.y+y);
end;                         {HeadStage}
{***************************************************************************}
procedure TothStage;
var al, be, ga, x, y : Real;
begin

 al:=GC.al+Neck.al.nom+Head.al.nom+Toth.al.nom;
 while al>2*Pi do al:=al-2*Pi;
 while al<0 do al:=al+2*Pi;
 if al>Pi then al:=al-2*Pi; {0<=te<=Pi}

 be:=GC.be+Neck.be.nom+Head.be.nom+Toth.be.nom;
 while be>2*Pi do be:=be-2*Pi;
 while be<0 do be:=be+2*Pi;
{ if be>Pi then be:=be-2*Pi;} {0<=te<=Pi}

 ga:=GC.ga+Neck.ga.nom+Head.ga.nom+Toth.ga.nom;
 while ga>2*Pi do ga:=ga-2*Pi;
 while ga<0 do ga:=ga+2*Pi;
{ if ga>Pi then ga:=ga-2*Pi;} {0<=te<=Pi}

 x:=Toth.L*(sin(be)*sin(ga)+cos(al)*cos(be)*cos(ga));
 y:=Toth.L*sin(al)*cos(ga);

 Toth.x:=Round(Head.x+x);
 Toth.y:=Round(Head.y+y);
end;                         {TothStage}
{***************************************************************************}
procedure ShowStages;
begin
 {GC}
 SetColor(12); Rect(GC.x-2,GC.y-2,  4,  4);
 {Neck}
 SetColor(Neck.Rcolor); Rect(Neck.x-1,Neck.y-1,  2,  2);
 SetColor(Neck.Lcolor); Line(GC.x,GC.y,Neck.x,Neck.y);
 {Chst}
 SetColor(Chst.Rcolor); Rect(Chst.x-1,Chst.y-1,  2,  2);
 SetColor(Chst.Lcolor); Line(Neck.x,Neck.y,Chst.x,Chst.y);
                        Line(GC.x,GC.y,Chst.x,Chst.y);
 {Head}
 SetColor(Head.Rcolor); Rect(Head.x-1,Head.y-1,  2,  2);
 SetColor(Head.Lcolor); Line(Neck.x,Neck.y,Head.x,Head.y);
 {Toth}
 SetColor(Toth.Rcolor); Rect(Toth.x-1,Toth.y-1,  2,  2);
 SetColor(Toth.Lcolor); Line(Head.x,Head.y,Toth.x,Toth.y);
 {Right Top}
 {Tors}
 SetColor(RH.Rcolor); Rect(RH.Tors.x-1,RH.Tors.y-1,  2,  2);
 SetColor(RH.Lcolor); Line(Neck.x,Neck.y,RH.Tors.x,RH.Tors.y);
 {Shld}
 SetColor(RH.Rcolor); Rect(RH.Shld.x-1,RH.Shld.y-1,  2,  2);
 SetColor(RH.Lcolor); Line(RH.Tors.x,RH.Tors.y,RH.Shld.x,RH.Shld.y);
 {FArm}
 SetColor(RH.Rcolor); Rect(RH.FArm.x-1,RH.FArm.y-1,  2,  2);
 SetColor(RH.Lcolor); Line(RH.Shld.x,RH.Shld.y,RH.FArm.x,RH.FArm.y);
 {Hand}
 SetColor(RH.Rcolor); Rect(RH.Hand.x-1,RH.Hand.y-1,  2,  2);
 SetColor(RH.Lcolor); Line(RH.FArm.x,RH.FArm.y,RH.Hand.x,RH.Hand.y);

 {Left Top}
 {Tors}
 SetColor(LH.Rcolor); Rect(LH.Tors.x-1,LH.Tors.y-1,  2,  2);
 SetColor(LH.Lcolor); Line(Neck.x,Neck.y,LH.Tors.x,LH.Tors.y);
 {Shld}
 SetColor(LH.Rcolor); Rect(LH.Shld.x-1,LH.Shld.y-1,  2,  2);
 SetColor(LH.Lcolor); Line(LH.Tors.x,LH.Tors.y,LH.Shld.x,LH.Shld.y);
 {FArm}
 SetColor(LH.Rcolor); Rect(LH.FArm.x-1,LH.FArm.y-1,  2,  2);
 SetColor(LH.Lcolor); Line(LH.Shld.x,LH.Shld.y,LH.FArm.x,LH.FArm.y);
 {Hand}
 SetColor(LH.Rcolor); Rect(LH.Hand.x-1,LH.Hand.y-1,  2,  2);
 SetColor(LH.Lcolor); Line(LH.FArm.x,LH.FArm.y,LH.Hand.x,LH.Hand.y);

{ ShowHand(RH); ShowHand(LH);}
 ShowLeg(RL);  ShowLeg(LL);
end;
{***************************************************************************}
procedure Wait(count:Longint);
var a, b : Word;
    l : Longint;
begin
 l:=count and $FFFF0000; l:=l shr 16; a:=Word(l);
 l:=count and $0000FFFF; b:=Word(l);
 asm
  mov ah,$86
  mov cx,a
  mov dx,b
  int $15
 end
end;                         { Wait }
{*************************************************************************}
(*procedure StartWalking;
var _dy : Integer;
begin
        {Right Leg}
        Incr(RL.Knee.al,wVel);
        Incr(RL.Fing.al,wVel);
        {GC_Stage;}
        TazzStage(RL);
        KneeStage(RL);
        FootStage(RL);
        FingStage(RL);
        EdgeStage(RL);
        if RL.Fing.y<>Land
         then begin
               _dy:=RL.Fing.y-Land;
               RL.Fing.y:=Land;
               RL.Edge.y:=Land;
               RL.Foot.y:=RL.Foot.y-_dy;
               RL.Knee.y:=RL.Knee.y-_dy;
               GC.y:=GC.y-_dy;
              end;
        {Left Leg}
        Decr(LL.Knee.al,wVel);
        if LL.Knee.al.nom>LL.Knee.al.min/2
         then begin
               Incr(LL.Foot.al,2*wVel);
               Decr(LL.Fing.al,1.5*wVel);
              end
         else begin
               Decr(LL.Foot.al,2*wVel);
               Incr(LL.Fing.al,1.5*wVel);
              end;
        if LL.Knee.al.nom<=LL.Knee.al.min
         then begin
               LL.Foot.al.nom:=LL.Foot.al.min;
               WalkStart:=No;
               LLeg_forw:=No;
              end;
        TazzStage(LL);
        KneeStage(LL);
        FootStage(LL);
        FingStage(LL);
        EdgeStage(LL);
end;*)                         {StartWalking}
{***************************************************************************}
(*procedure LegForward(var L:Leg);
begin
               Decr(L.Knee.al,wVel);
               if L.Knee.al.nom>0
                then Incr(L.Foot.al,2*wVel)
                else Decr(L.Foot.al,2*wVel);
               if L.Fing.al.nom>0 then Decr(L.Fing.al,2*wVel);
               TazzStage(L);
               KneeStage(L);
               {FootStage(L);
               FingStage(L);
               EdgeStage(L);}
end;*)                         {LegForward}
{***************************************************************************}
(*procedure LegBackward(var L:Leg);
var _dy : Integer;
begin
               Incr(L.Knee.al,wVel);
               if L.Knee.al.nom>0
                then begin
                      Incr(L.Fing.al,wVel);
                      {GC_Stage;}
                      TazzStage(L);
                      KneeStage(L);
                      FootStage(L);
                      FingStage(L);
                      EdgeStage(L);
                      if L.Fing.y<>Land
                       then begin
                             _dy:=L.Fing.y-Land;
                             L.Fing.y:=Land;
                             L.Edge.y:=Land;
                             L.Foot.y:=L.Foot.y-_dy;
                             L.Knee.y:=L.Knee.y-_dy;
                             GC.y:=GC.y-_dy;
                            end;
                     end
                else begin
                      TazzStage(L);
                      KneeStage(L);
                      FootStage(L);
                      FingStage(L);
                      EdgeStage(L);
                      if L.Foot.y>Land
                       then begin
                             _dy:=L.Foot.y-Land;
                             L.Foot.y:=Land;
                             L.Knee.y:=L.Knee.y-_dy;
                             GC.y:=GC.y-_dy;
                            end;
                     end;
end;*)
{***************************************************************************}
(*procedure WalkLegs;
var _dy : Integer;
begin
 if WalkStart
  then StartWalking
  else begin {WalkStart}
        if LLeg_forw
         then begin
               {Right Leg}
               LegBackward(RL);
               {Left Leg}
               LegForward(LL);
               if RL.Knee.al.nom>=RL.Knee.al.max
                then LLeg_forw:=No;
              end
         else begin {not LLeg_forw}
               {Right Leg}
               LegForward(RL);
               {Left Leg}
               LegBackward(LL);
               if RL.Knee.al.nom<=RL.Knee.al.min
                then LLeg_forw:=Yes;
              end;
       end; {else WalkStart}
end;*)                         {WalkLegs}
{***************************************************************************}
(*procedure WalkLegs;
var _dy : Integer;
begin
 if WalkStart
  then StartWalking
  else begin {WalkStart}
        if LLeg_forw
         then begin
               {Right Leg}
               LegBackward(RL);
               {Left Leg}
               LegForward(LL);
               if RL.Knee.al.nom>=RL.Knee.al.max
                then LLeg_forw:=No;
              end
         else begin {not LLeg_forw}
               {Right Leg}
               LegForward(RL);
               {Left Leg}
               LegBackward(LL);
               if RL.Knee.al.nom<=RL.Knee.al.min
                then LLeg_forw:=Yes;
              end;
       end; {else WalkStart}
end;*)                         {WalkLegs}
{***************************************************************************}
(*procedure Walk;
var dy : Longint;
    s : String;
begin
 SetFillStyle(1,2); Bar(200,100,500,Land+2);
 SetColor(15); Str(num,s);
 OutTextXY(450,120,s);
 Dec(Abs_X,Velocity);
 if Abs_X<200 then Inc(Abs_X,20);
 SetColor(9);
 Line(200,Land,500,Land);
 SetColor(15);
 for dy:=0 to 28 do Line(Abs_X+10*dy,Land,Abs_X+10*dy,Land-2);
 for dy:=0 to 14 do Line(Abs_X+20*dy,Land,Abs_X+20*dy,Land-4);}

{ Circle(GC.x,GC.y,RL.Tazz_L+RL.Knee_L+RL.Foot_L);}

{ WalkLegs;
 WalkHands;
 ShowStages;}
end;*)                         {Walk}
{***************************************************************************}

{*************************************************************************}
BEGIN
END.