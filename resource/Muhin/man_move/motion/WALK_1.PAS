Unit Walk_1;
INTERFACE
{*************************************************************************}
type Stage = record
              x, y : Integer;
              a : Real;
             end;
     Angle = record
              a, amin, amax : Real;
             end;
     Stage1 = record
               x, y : Integer;
               al : Angle; {(+)-clockwise, (-)-conterclockwise}
                          {'0'-to bottom, screen y-direct}
                          {foot &so - (a-Pi/4)}
               {forw : Boolean;}
              end;
     Hand = record
             Tors, Shld, FArm, Hand : Stage1;
             Hand_L, FArm_L, Shld_L, Tors_L : Word;
             Lcolor, Rcolor : Byte;
            end;
     Leg = record
            Tazz, Knee, Foot, Fing, Edge : Stage1;
            Tazz_L, Knee_L, Foot_L, Fing_L, Edge_L : Word;
            Lcolor, Rcolor : Byte;
           end;

var GD, GM: Integer;
    GC : Stage;
    RH, LH : Hand;
    RL, LL : Leg;
    wVel, Pidiv2 : Real;
    Land : Word;
    WalkStart, LLeg_forw : Boolean;
const Yes : Boolean = True;
       No : Boolean = False;
procedure Rect(x,y,dx,dy:Word);
procedure Incr(var St:Angle;da:Real);
procedure Decr(var St:Angle;da:Real);
procedure ShowHand(H:Hand);
procedure TorsStage(var H:Hand);
procedure ShldStage(var H:Hand);
procedure FArmStage(var H:Hand);
procedure HandStage(var H:Hand);
procedure ShowLeg(L:Leg);
procedure TazzStage(var L:Leg);
procedure KneeStage(var L:Leg);
procedure FootStage(var L:Leg);
procedure FingStage(var L:Leg);
procedure EdgeStage(var L:Leg);
procedure ShowGC;
procedure ShowStages;
procedure InitSceleton;
procedure WalkHands;
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
 r:=St.a+da;
 if r>St.amax
  then St.a:=St.amax
  else St.a:=r;
end;
{***************************************************************************}
procedure Decr(var St:Angle;da:Real);
var r : Real;
begin
 r:=St.a-da;
 if r<St.amin
  then St.a:=St.amin
  else St.a:=r;
end;
{***************************************************************************}
procedure ShowHand(H:Hand);
begin
 {Tors}
 SetColor(H.Rcolor); Rect(H.Tors.x-1,H.Tors.y-1,  2,  2);
 SetColor(H.Lcolor); Line(GC.x,GC.y,H.Tors.x,H.Tors.y);
 {Shld}
 SetColor(H.Rcolor); Rect(H.Shld.x-1,H.Shld.y-1,  2,  2);
 SetColor(H.Lcolor); Line(H.Tors.x,H.Tors.y,H.Shld.x,H.Shld.y);
 {FArm}
 SetColor(H.Rcolor); Rect(H.FArm.x-1,H.FArm.y-1,  2,  2);
 SetColor(H.Lcolor); Line(H.Shld.x,H.Shld.y,H.FArm.x,H.FArm.y);
 {Hand}
 SetColor(H.Rcolor); Rect(H.Hand.x-1,H.Hand.y-1,  2,  2);
 SetColor(H.Lcolor); Line(H.FArm.x,H.FArm.y,H.Hand.x,H.Hand.y);
end;
{***************************************************************************}
procedure TorsStage(var H:Hand);
var dx, dy : Longint;
begin
 dx:=Round(H.Tors_L*sin(GC.a-H.Tors.al.a));
 dy:=Round(H.Tors_L*cos(GC.a-H.Tors.al.a));
 H.Tors.x:=GC.x-dx;
 H.Tors.y:=GC.y-dy;
end;
{***************************************************************************}
procedure ShldStage(var H:Hand);
var dx, dy : Longint;
begin
 dx:=Round(H.Shld_L*sin(GC.a-H.Tors.al.a-H.Shld.al.a));
 dy:=Round(H.Shld_L*cos(GC.a-H.Tors.al.a-H.Shld.al.a));
 H.Shld.x:=H.Tors.x-dx;
 H.Shld.y:=H.Tors.y-dy;
end;
{***************************************************************************}
procedure FArmStage(var H:Hand);
var dx, dy : Longint;
begin
 dx:=Round(H.FArm_L*sin(GC.a-H.Tors.al.a-H.Shld.al.a-H.FArm.al.a));
 dy:=Round(H.FArm_L*cos(GC.a-H.Tors.al.a-H.Shld.al.a-H.FArm.al.a));
 H.FArm.x:=H.Shld.x-dx;
 H.FArm.y:=H.Shld.y-dy;
end;
{***************************************************************************}
procedure HandStage(var H:Hand);
var dx, dy : Longint;
begin
 dx:=Round(H.Hand_L*sin(GC.a-H.Tors.al.a-H.Shld.al.a-H.FArm.al.a-H.Hand.al.a));
 dy:=Round(H.Hand_L*cos(GC.a-H.Tors.al.a-H.Shld.al.a-H.FArm.al.a-H.Hand.al.a));
 H.Hand.x:=H.FArm.x-dx;
 H.Hand.y:=H.FArm.y-dy;
end;
{***************************************************************************}
procedure ShowLeg(L:Leg);
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
procedure TazzStage(var L:Leg);
var dx, dy : Longint;
begin
 dx:=Round(L.Tazz_L*sin(GC.a+L.Tazz.al.a));
 dy:=Round(L.Tazz_L*cos(GC.a+L.Tazz.al.a));
 L.Tazz.x:=GC.x-dx;
 L.Tazz.y:=GC.y+dy;
end;
{***************************************************************************}
procedure KneeStage(var L:Leg);
var dx, dy : Longint;
begin
 dx:=Round(L.Knee_L*sin(GC.a+L.Tazz.al.a+L.Knee.al.a));
 dy:=Round(L.Knee_L*cos(GC.a+L.Tazz.al.a+L.Knee.al.a));
 L.Knee.x:=L.Tazz.x-dx;
 L.Knee.y:=L.Tazz.y+dy;
end;
{***************************************************************************}
procedure FootStage(var L:Leg);
var dx, dy : Longint;
begin
 dx:=Round(L.Foot_L*sin(GC.a+L.Tazz.al.a+L.Knee.al.a+L.Foot.al.a));
 dy:=Round(L.Foot_L*cos(GC.a+L.Tazz.al.a+L.Knee.al.a+L.Foot.al.a));
 L.Foot.x:=L.Knee.x-dx;
 L.Foot.y:=L.Knee.y+dy;
end;
{***************************************************************************}
procedure FingStage(var L:Leg);
var dx, dy : Longint;
begin
 dx:=Round(L.Fing_L*sin(GC.a+L.Tazz.al.a+L.Knee.al.a+L.Foot.al.a+L.Fing.al.a
           -Pidiv2));
 dy:=Round(L.Fing_L*cos(GC.a+L.Tazz.al.a+L.Knee.al.a+L.Foot.al.a+L.Fing.al.a
           -Pidiv2));
 L.Fing.x:=L.Foot.x-dx;
 L.Fing.y:=L.Foot.y+dy;
end;
{***************************************************************************}
procedure EdgeStage(var L:Leg);
var dx, dy : Longint;
    a : Real;
begin
 a:=GC.a+L.Tazz.al.a+L.Knee.al.a+L.Foot.al.a+L.Fing.al.a-Pidiv2+L.Edge.al.a;
 if a>-Pidiv2 then a:=-Pidiv2;
 dx:=Round(L.Edge_L*sin(a));
 dy:=Round(L.Edge_L*cos(a));
 L.Edge.x:=L.Fing.x-dx;
 L.Edge.y:=L.Fing.y+dy;
end;
{***************************************************************************}
procedure ShowGC;
begin
 SetColor(12); Rect(GC.x-2,GC.y-2,  4,  4);
end;
{***************************************************************************}
procedure ShowStages;
begin
 ShowGC;
 ShowHand(RH); ShowHand(LH);
 ShowLeg(RL);  ShowLeg(LL);
end;
{***************************************************************************}
procedure InitSceleton;
begin
 DetectGraph (GD,GM);
 InitGraph   (GD,GM,'d:\dos\bc\bgi');
 if GraphResult<>grOk then Halt(1);
 Land:=340;
 Pidiv2:=Pi/2;

 GC.x:=329; GC.y:=229; GC.a:=0;
 {Left side}
 {Top}
 LH.Tors_L:=40;
 LH.Tors.al.a:=0; LH.Tors.al.amin:=-Pi/6; LH.Tors.al.amax:=Pi/2;
 LH.Shld_L:=30;
 LH.Shld.al.a:=Pi; LH.Shld.al.amin:=-2*Pi; LH.Shld.al.amax:=2*Pi; {!!!!!!}
 LH.FArm_L:=25;
 LH.FArm.al.a:=0; LH.FArm.al.amin:=-5*Pi/6; LH.FArm.al.amax:=0;
 LH.Hand_L:=10;
 LH.Hand.al.a:=0; LH.Hand.al.amin:=-Pi/4; LH.Hand.al.amax:=Pi/4;
 LH.Lcolor:=9;
 LH.Rcolor:=12;
 {Bottom}
 LL.Tazz_L:=15;
 LL.Tazz.al.a:=0; LL.Tazz.al.amin:=0;     LL.Tazz.al.amax:=0;
 LL.Knee_L:=45;
 LL.Knee.al.a:=0; LL.Knee.al.amin:=-Pi/6; LL.Knee.al.amax:=Pi/6;
 LL.Foot_L:=45;
 LL.Foot.al.a:=0; LL.Foot.al.amin:=0;     LL.Foot.al.amax:=3*Pi/2;
 LL.Fing_L:=15;
 LL.Fing.al.a:=0; LL.Fing.al.amin:=-Pi/6; LL.Fing.al.amax:=Pi/3;
 LL.Fing.x:=GC.x+LL.Fing_L;
 LL.Fing.y:=Land-1;
 LL.Edge_L:=7;
 LL.Edge.al.a:=0; LL.Edge.al.amin:=-Pi/6; LL.Edge.al.amax:=0;
 LL.Edge.x:=LL.Fing.x+LL.Edge_L;
 LL.Edge.y:=Land-1;
 LL.Lcolor:=10;
 LL.Rcolor:=6;
 {Right side}
 {Top}
 RH.Tors_L:=40;
 RH.Tors.al.a:=0; RH.Tors.al.amin:=-Pi/6; RH.Tors.al.amax:=Pi/2;
 RH.Shld_L:=30;
 RH.Shld.al.a:=Pi; RH.Shld.al.amin:=-2*Pi; RH.Shld.al.amax:=2*Pi; {!!!!!!}
 RH.FArm_L:=25;
 RH.FArm.al.a:=0; RH.FArm.al.amin:=-5*Pi/6; RH.FArm.al.amax:=0;
 RH.Hand_L:=10;
 RH.Hand.al.a:=0; RH.Hand.al.amin:=-Pi/4; RH.Hand.al.amax:=Pi/4;
 RH.Lcolor:=10;
 RH.Rcolor:=15;
 {Bottom}
 RL.Tazz_L:=15;
 RL.Tazz.al.a:=0; RL.Tazz.al.amin:=0;     RL.Tazz.al.amax:=0;
 RL.Knee_L:=45;
 RL.Knee.al.a:=0; RL.Knee.al.amin:=-Pi/6; RL.Knee.al.amax:=Pi/6;
 RL.Foot_L:=45;
 RL.Foot.al.a:=0; RL.Foot.al.amin:=0;     RL.Foot.al.amax:=3*Pi/2;
 RL.Fing_L:=15;
 RL.Fing.al.a:=0; RL.Fing.al.amin:=-Pi/6; RL.Fing.al.amax:=Pi/3;
 RL.Fing.x:=GC.x+RL.Fing_L;
 RL.Fing.y:=Land-1;
 RL.Edge_L:=7;
 RL.Edge.al.a:=0; RL.Edge.al.amin:=-Pi/6; RL.Edge.al.amax:=0;
 RL.Edge.x:=RL.Fing.x+RL.Edge_L;
 RL.Edge.y:=Land-1;
 RL.Lcolor:=14;
 RL.Rcolor:=15;

 TorsStage(RH); TorsStage(LH);
 ShldStage(RH); ShldStage(LH);
 FArmStage(RH); FArmStage(LH);
 HandStage(RH); HandStage(LH);

 TazzStage(RL); TazzStage(LL);
 KneeStage(RL); KneeStage(LL);
 FootStage(RL); FootStage(LL);
 FingStage(RL); FingStage(LL);
 EdgeStage(RL); EdgeStage(LL);

 ShowStages;
end;
{***************************************************************************}
procedure WalkHands;
begin
 if LLeg_forw
  then begin
        {Right}
        Incr(RH.Tors.al,0.01);
        Decr(RH.Shld.al,wVel/2);
        Decr(RH.FArm.al,wVel/2);
        {Left}
        Decr(LH.Tors.al,0.01);
        Incr(LH.Shld.al,wVel/2);
        Incr(LH.FArm.al,wVel/2);
       end
  else begin
        {Right}
        Decr(RH.Tors.al,0.01);
        Incr(RH.Shld.al,wVel/2);
        Incr(RH.FArm.al,wVel/2);
        {Left}
        Incr(LH.Tors.al,0.01);
        Decr(LH.Shld.al,wVel/2);
        Decr(LH.FArm.al,wVel/2);
       end;
 TorsStage(RH); TorsStage(LH);
 ShldStage(RH); ShldStage(LH);
 FArmStage(RH); FArmStage(LH);
 HandStage(RH); HandStage(LH);
end;                         {WalkHands}
{***************************************************************************}

{*************************************************************************}
BEGIN
END.