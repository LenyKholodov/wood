uses Graph, Crt, Dos, Walk_6;
var fps_num : Longint;
    Hour, Min, Sec, Hund : Word;
    s_1 : String;
    Abs_X, VSpeed : Integer;
    Ch : Char;
    bo_Walking, bo_Jump, bo_StrifeLeft, bo_Show, bo_refresh, Quit : Boolean;
    bo_BreathIn, bo_Doing, bo_Walk, bo_Run, bo_Flying : Boolean;
    by_Run, by_Jump, by_JumpStage, by_RunStage, by_Strife,
     by_StrifeStage : Byte;
    Walk_forw : Boolean;
    r_Temp, r_JumpHight : Real;
    w_Heigth, Velocity : Word;
    tTurn : Longint;
    r_1, de_be : Real;
    de_x : Word;
{***************************************************************************}
function Height(Lg:Leg):Word;
var h : Real;
begin
 h:=Lg.Tazz.L*cos(Lg.Tazz.ga.nom);
 h:=h+Lg.Knee.L*cos(Lg.Knee.al.nom);
 h:=h+Lg.Foot.L*cos(Lg.Knee.al.nom+Lg.Foot.al.nom);
 h:=h+Lg.Fing.L*cos(Lg.Knee.al.nom+Lg.Foot.al.nom+Lg.Fing.al.nom);
 Height:=Round(h);
end;                         {Height}
{***************************************************************************}
procedure CalcStages;
begin
 NeckStage;
 ChstStage;
 HeadStage;
 TothStage;
 TorsStage(RH); TorsStage(LH);
 ShldStage(RH); ShldStage(LH);
 FArmStage(RH); FArmStage(LH);
 HandStage(RH); HandStage(LH);

 TazzStage(RL); TazzStage(LL);
 KneeStage(RL); KneeStage(LL);
 FootStage(RL); FootStage(LL);
 FingStage(RL); FingStage(LL);
 EdgeStage(RL); EdgeStage(LL);
end;                         {CalcStages}
{***************************************************************************}
procedure InitSceleton;
begin
 DetectGraph(GD,GM); InitGraph(GD,GM,' ');
 if GraphResult<>grOk then Halt(1);
{ Land:=340;}
 Pidiv2:=Pi/2;

 GC.x:=329; GC.y:=229;
 GC.al:=0; GC.be:=0; GC.ga:=0;

 Neck.L:=45;
 Neck.al.nom:=-Pi/2;  Neck.al.min:=-7*Pi/6; Neck.al.max:=Pi/3;
 Neck.be.nom:=0;      Neck.be.min:=0;     Neck.be.max:=0;
 Neck.ga.nom:=0;      Neck.ga.min:=-Pi/3; Neck.ga.max:=Pi/3;
 Neck.Lcolor:=10;
 Neck.Rcolor:=10;
 Chst.L:=20;
 Chst.al.nom:=11*Pi/12; Chst.al.min:=10*Pi/12;     Chst.al.max:=11*Pi/12;
 Chst.be.nom:=0;      Chst.be.min:=0;     Chst.be.max:=0;
 Chst.ga.nom:=0;      Chst.ga.min:=0;     Chst.ga.max:=0;
 Chst.Lcolor:=15;
 Chst.Rcolor:=15;
 Head.L:=10;
 Head.al.nom:=0;      Head.al.min:=0;     Head.al.max:=0;
 Head.be.nom:=0;      Head.be.min:=0;     Head.be.max:=0;
 Head.ga.nom:=0;      Head.ga.min:=0;     Head.ga.max:=0;
 Head.Lcolor:=10;
 Head.Rcolor:=10;
 Toth.L:=5;
 Toth.al.nom:=Pi/2;   Toth.al.min:=0;     Toth.al.max:=0;
 Toth.be.nom:=0;      Toth.be.min:=0;     Toth.be.max:=0;
 Toth.ga.nom:=0;      Toth.ga.min:=0;     Toth.ga.max:=0;
 Toth.Lcolor:=10;
 Toth.Rcolor:=10;

 {Left side}
 {Top}
 with LH do begin
  Tors.L:=20;
  Tors.al.nom:=0;     Tors.al.min:=-2*Pi/3; Tors.al.max:=Pi/2;
  Tors.be.nom:=0;     Tors.be.min:=0;     Tors.be.max:=0;
  Tors.ga.nom:=5*Pi/8;  Tors.ga.min:=0;     Tors.ga.max:=0;
  Shld.L:=30;
  Shld.al.nom:=Pi;    Shld.al.min:=-2*Pi; Shld.al.max:=2*Pi; {!!!!!!}
  Shld.be.nom:=0;     Shld.be.min:=-2*Pi/3; Shld.be.max:=Pi/3;
  Shld.ga.nom:=Pi/24;     Shld.ga.min:=Pi/24; Shld.ga.max:=Pi;
  FArm.L:=25;
  FArm.al.nom:=0; FArm.al.min:=-5*Pi/6; FArm.al.max:=0;
  FArm.be.nom:=0; FArm.be.min:=-Pi/6; FArm.be.max:=Pi/2;
  FArm.ga.nom:=0; FArm.ga.min:=0; FArm.ga.max:=0;
  Hand.L:=10;
  Hand.al.nom:=0; Hand.al.min:=-Pi/4; Hand.al.max:=Pi/4;
  Hand.be.nom:=0; Hand.be.min:=0; Hand.be.max:=0;
  Hand.ga.nom:=0; Hand.ga.min:=0; Hand.ga.max:=0;
  Lcolor:=11;
  Rcolor:=11;
 end; {with LH}
 {Bottom}
 with LL do begin
  Tazz.L:=20;
  Tazz.al.nom:=Pi/2;  Tazz.al.min:=0;     Tazz.al.max:=0;
  Tazz.be.nom:=0;     Tazz.be.min:=0;     Tazz.be.max:=0;
  Tazz.ga.nom:=Pi/3;  Tazz.ga.min:=Pi/4;  Tazz.ga.max:=Pi/2;
  Knee.L:=45;
  Knee.al.nom:=0;     Knee.al.min:=-2*Pi/3; Knee.al.max:=Pi/6;
  Knee.be.nom:=0;     Knee.be.min:=-Pi/3; Knee.be.max:=Pi/3;
  Knee.ga.nom:=0;     Knee.ga.min:=-Pi/6; Knee.ga.max:=Pi/2;
  Foot.L:=45;
  Foot.al.nom:=0;     Foot.al.min:=0;     Foot.al.max:=3*Pi/2;
  Foot.be.nom:=0;     Foot.be.min:=0;     Foot.be.max:=0;
  Foot.ga.nom:=0;     Foot.ga.min:=0;     Foot.ga.max:=0;
  Fing.L:=15;
  Fing.al.nom:=-Pi/2; Fing.al.min:=-2*Pi/3; Fing.al.max:=-Pi/6;
  Fing.be.nom:=0;     Fing.be.min:=0;     Fing.be.max:=0;
  Fing.ga.nom:=0;     Fing.ga.min:=0;     Fing.ga.max:=0;
{  Fing.x:=GC.x+Fing_L;
  Fing.y:=Land-1;}
  Edge.L:=7;
  Edge.al.nom:=0;     Edge.al.min:=-Pi/3; Edge.al.max:=0;
  Edge.be.nom:=0;     Edge.be.min:=0;     Edge.be.max:=0;
  Edge.ga.nom:=0;     Edge.ga.min:=0;     Edge.ga.max:=0;
{  Edge.x:=Fing.x+Edge_L;
  Edge.y:=Land-1;}
  Lcolor:=11;
  Rcolor:=11;
 end; {with LL}
 {Right side}
 {Top}
 with RH do begin
  Tors.L:=20;
  Tors.al.nom:=0; Tors.al.min:=-2*Pi/3; Tors.al.max:=Pi/2;
  Tors.be.nom:=0;     Tors.be.min:=0;     Tors.be.max:=0;
  Tors.ga.nom:=-5*Pi/8; Tors.ga.min:=0;     Tors.ga.max:=0;
  Shld.L:=30;
  Shld.al.nom:=Pi; Shld.al.min:=-2*Pi;   Shld.al.max:=2*Pi; {!!!!!!}
  Shld.be.nom:=0; Shld.be.min:=0; Shld.be.max:=0;
  Shld.ga.nom:=-Pi/24;     Shld.ga.min:=-Pi; Shld.ga.max:=-Pi/24;
  FArm.L:=25;
  FArm.al.nom:=0; FArm.al.min:=-5*Pi/6; FArm.al.max:=0;
  FArm.be.nom:=0; FArm.be.min:=-Pi/2; FArm.be.max:=Pi/6;
  FArm.ga.nom:=0; FArm.ga.min:=0; FArm.ga.max:=0;
  Hand.L:=10;
  Hand.al.nom:=0; Hand.al.min:=-Pi/4; Hand.al.max:=Pi/4;
  Hand.be.nom:=0; Hand.be.min:=0; Hand.be.max:=0;
  Hand.ga.nom:=0; Hand.ga.min:=0; Hand.ga.max:=0;
  Lcolor:=14;
  Rcolor:=14;
 end; {with RH}
 {Bottom}
 with RL do begin
  Tazz.L:=20;
  Tazz.al.nom:=Pi/2;  Tazz.al.min:=0;     Tazz.al.max:=0;
  Tazz.be.nom:=0;     Tazz.be.min:=0;     Tazz.be.max:=0;
  Tazz.ga.nom:=-Pi/3; Tazz.ga.min:=-Pi/2; Tazz.ga.max:=-Pi/4;
  Knee.L:=45;
  Knee.al.nom:=0;     Knee.al.min:=-2*Pi/3; Knee.al.max:=Pi/6;
  Knee.be.nom:=0;     Knee.be.min:=-Pi/3; Knee.be.max:=Pi/3;
  Knee.ga.nom:=0;     Knee.ga.min:=-Pi/6; Knee.ga.max:=Pi/2;
  Foot.L:=45;
  Foot.al.nom:=0;     Foot.al.min:=0;     Foot.al.max:=3*Pi/2;
  Foot.be.nom:=0;     Foot.be.min:=0;     Foot.be.max:=0;
  Foot.ga.nom:=0;     Foot.ga.min:=0;     Foot.ga.max:=0;
  Fing.L:=15;
  Fing.al.nom:=-Pi/2; Fing.al.min:=-2*Pi/3; Fing.al.max:=-Pi/6;
  Fing.be.nom:=0;     Fing.be.min:=0;     Fing.be.max:=0;
  Fing.ga.nom:=0;     Fing.ga.min:=0;     Fing.ga.max:=0;
{  Fing.x:=GC.x+Fing_L;
  Fing.y:=Land-1;}
  Edge.L:=7;
  Edge.al.nom:=0;     Edge.al.min:=-Pi/3; Edge.al.max:=0;
  Edge.be.nom:=0;     Edge.be.min:=0;     Edge.be.max:=0;
  Edge.ga.nom:=0;     Edge.ga.min:=0;     Edge.ga.max:=0;
 { Edge.x:=Fing.x+Edge_L;
  Edge.y:=Land-1;}
  Lcolor:=14;
  Rcolor:=14;
 end; {with RL}

 CalcStages;
 Land:=RL.Foot.x;
 ShowStages;
end;                         {InitSceleton}
{***************************************************************************}
procedure Calc_FPS;
var Hour1, Min1, Sec1, Hund1 : Word;
    _r0, _r1 : Real;
    s_0 : String;
begin
 GetTime(Hour1,Min1,Sec1,Hund1);
 _r0:=Hour;  _r0:=3600*_r0+60*Min+Sec+Hund*0.01;
 _r1:=Hour1; _r1:=3600*_r1+60*Min1+Sec1+Hund1*0.01;
 _r1:=fps_num/(_r1-_r0);
 Str(_r1:10:3,s_0); SetFillStyle(1,2); Bar(540,10,630,18);
 SetColor(10); OutTextXY(550,10,s_0);
 fps_num:=0;
 GetTime(Hour,Min,Sec,Hund);
end;                         {Calc_FPS}
{***************************************************************************}
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
procedure WalkLegs;
var _dy : Integer;
    w_HL, w_HR : Word;
begin
 if LLeg_forw
  then begin
        {Right Leg}
        Incr(RL.Knee.al,wVel);
        if RL.Knee.al.nom>0 then Decr(RL.Edge.al,wVel);
        {Left Leg}
        Decr(LL.Knee.al,wVel);
        if LL.Knee.al.nom>0
         then Incr(LL.Foot.al,wVel*2)
         else begin
               Decr(LL.Foot.al,wVel*2);
               Incr(LL.Edge.al,wVel*2);
              end;
        if LL.Fing.al.nom>0 then Decr(LL.Fing.al,wVel*2);
        if RL.Knee.al.nom>=Pi/6
         then LLeg_forw:=No;
       end
  else begin {not LLeg_forw}
        {Right Leg}
        Decr(RL.Knee.al,wVel);
        if RL.Knee.al.nom>0
         then Incr(RL.Foot.al,wVel*2)
         else begin
               Decr(RL.Foot.al,wVel*2);
               Incr(RL.Edge.al,wVel*2);
              end;
        if RL.Fing.al.nom>0 then Decr(RL.Fing.al,wVel*2);
        {Left Leg}
        Incr(LL.Knee.al,wVel);
        if LL.Knee.al.nom>0 then Decr(LL.Edge.al,wVel);
        if RL.Knee.al.nom<=-Pi/6
         then LLeg_forw:=Yes;
       end;
 w_HL:=Height(LL); w_HL:=Land-w_HL;
 w_HR:=Height(RL); w_HR:=Land-w_HR;
 if w_HR<w_HL then GC.y:=w_HR
              else GC.y:=w_HL;
end;                         {WalkLegs}
{***************************************************************************}
procedure WalkHands;
begin
 if LLeg_forw
  then begin
        {Right}
        Decr(RH.Tors.al,wVel/3);
        Decr(RH.Shld.al,wVel/2);
        Decr(RH.FArm.al,wVel/3);
        Decr(RH.FArm.be,wVel/2);
        {Left}
        Incr(LH.Tors.al,wVel/3);
        Incr(LH.Shld.al,wVel/2);
        Incr(LH.FArm.al,wVel/3);
        Decr(LH.FArm.be,wVel/2);
       end
  else begin
        {Right}
        Incr(RH.Tors.al,wVel/3);
        Incr(RH.Shld.al,wVel/2);
        Incr(RH.FArm.al,wVel/3);
        Incr(RH.FArm.be,wVel/2);
        {Left}
        Decr(LH.Tors.al,wVel/3);
        Decr(LH.Shld.al,wVel/2);
        Decr(LH.FArm.al,wVel/3);
        Incr(LH.FArm.be,wVel/2);
       end;
end;                         {WalkHands}
{***************************************************************************}
procedure StandUp;
begin

 bo_Walking:=False;
end;
{***************************************************************************}
procedure Walk;
begin
        WalkLegs;
        WalkHands;
 {       bo_Walking:=True;
       end
  else begin
        if bo_Walking then StandUp;
       end;}
end;                         {Walk}
{***************************************************************************}
procedure Jumping;
begin
 if by_Jump=0
  then begin
        Inc(by_Jump);
        by_JumpStage:=0;
       end;
 if by_Jump=1
  then if by_JumpStage=0
        then begin
              Incr(Neck.al,wVel);
              Decr(LH.Shld.al,wVel*2); {Top}
              Decr(LH.FArm.al,wVel*4);
              Incr(LH.FArm.be,wVel*2);
              Decr(RH.Shld.al,wVel*2);
              Decr(RH.FArm.al,wVel*4);
              Decr(RH.FArm.be,wVel*2);
              Decr(LL.Knee.al,wVel*2); {Bottom}
              Incr(LL.Foot.al,wVel*4);
              Decr(LL.Fing.al,wVel*2);
              Decr(RL.Knee.al,wVel*2);
              Incr(RL.Foot.al,wVel*4);
              Decr(RL.Fing.al,wVel*2);
              if LL.Knee.al.nom<-2.5*Pi/15
               then Inc(by_JumpStage);
             end
        else begin
              Decr(Neck.al,wVel);
              Incr(LH.Shld.al,wVel*2); {Top}
              Incr(LH.FArm.al,wVel*4);
              Decr(LH.FArm.be,wVel*2);
              Incr(RH.Shld.al,wVel*2);
              Incr(RH.FArm.al,wVel*4);
              Incr(RH.FArm.be,wVel*2);
              Incr(LL.Knee.al,wVel*2); {Bottom}
              Decr(LL.Foot.al,wVel*4);
              Incr(LL.Fing.al,wVel*2);
              Incr(RL.Knee.al,wVel*2);
              Decr(RL.Foot.al,wVel*4);
              Incr(RL.Fing.al,wVel*2);
              if LL.Knee.al.nom>-wVel
               then begin
                     Inc(by_Jump);
                     by_JumpStage:=0;
                     bo_Flying:=Yes;
                    end;
             end;
 if by_Jump=2
  then if by_JumpStage=0
        then begin
              Incr(Neck.al,wVel/2);
              Incr(LH.Shld.al,wVel*2); {Top}
              Incr(LH.Shld.ga,wVel);
              Decr(LH.FArm.al,wVel*5);
              Incr(RH.Shld.al,wVel);
              Decr(RH.Shld.ga,wVel);
              Decr(RH.FArm.al,wVel*4);
              Decr(LL.Knee.al,wVel*4); {Bottom}
              Decr(LL.Knee.be,wVel/2);
              Incr(LL.Foot.al,wVel*5);
              Incr(LL.Fing.al,wVel*2);
              Decr(RL.Knee.al,wVel*2);
              Incr(RL.Knee.be,wVel/2);
              Incr(RL.Foot.al,wVel*5);
              Incr(RL.Fing.al,wVel*2);
              Dec(GC.y,4);
              if LL.Knee.al.nom<-8*Pi/15
               then Inc(by_JumpStage);
             end
        else begin
              Decr(Neck.al,wVel/2);
              Decr(LH.Shld.al,wVel*2); {Top}
              Decr(LH.Shld.ga,wVel);
              Incr(LH.FArm.al,wVel*5);
              Decr(RH.Shld.al,wVel);
              Incr(RH.Shld.ga,wVel);
              Incr(RH.FArm.al,wVel*4);
              Incr(LL.Knee.al,wVel*4); {Bottom}
              Incr(LL.Knee.be,wVel/2);
              Decr(LL.Foot.al,wVel*5);
              Decr(LL.Fing.al,wVel*2);
              Incr(RL.Knee.al,wVel*2);
              Decr(RL.Knee.be,wVel/2);
              Decr(RL.Foot.al,wVel*5);
              Decr(RL.Fing.al,wVel*2);
              Inc(GC.y,4);
              if LL.Knee.al.nom>-wVel
               then begin
                     Inc(by_Jump);
                     by_JumpStage:=0;
                     bo_Flying:=No;
                    end;
             end;
 if by_Jump=3
  then if by_JumpStage=0
        then begin
              Incr(Neck.al,wVel);
              Decr(LH.Shld.al,wVel*2); {Top}
              Decr(LH.FArm.al,wVel*4);
              Incr(LH.FArm.be,wVel*2);
              Decr(RH.Shld.al,wVel*2);
              Decr(RH.FArm.al,wVel*4);
              Decr(RH.FArm.be,wVel*2);
              Decr(LL.Knee.al,wVel*2); {Bottom}
              Incr(LL.Foot.al,wVel*4);
              Decr(LL.Fing.al,wVel*2);
              Decr(RL.Knee.al,wVel*2);
              Incr(RL.Foot.al,wVel*4);
              Decr(RL.Fing.al,wVel*2);
              if LL.Knee.al.nom<-2.5*Pi/15
               then Inc(by_JumpStage);
             end
        else begin
              Decr(Neck.al,wVel);
              Incr(LH.Shld.al,wVel*2); {Top}
              Incr(LH.FArm.al,wVel*4);
              Decr(LH.FArm.be,wVel*2);
              Incr(RH.Shld.al,wVel*2);
              Incr(RH.FArm.al,wVel*4);
              Incr(RH.FArm.be,wVel*2);
              Incr(LL.Knee.al,wVel*2); {Bottom}
              Decr(LL.Foot.al,wVel*4);
              Incr(LL.Fing.al,wVel*2);
              Incr(RL.Knee.al,wVel*2);
              Decr(RL.Foot.al,wVel*4);
              Incr(RL.Fing.al,wVel*2);
              if LL.Knee.al.nom>-wVel
               then begin
                     by_Jump:=0;
                     bo_Jump:=No;
                    end;
             end;
 if not bo_Flying then
  begin
   w_Heigth:=Height(LL);
   w_Heigth:=Land-w_Heigth;
   GC.y:=w_Heigth;
  end;
end;                         {Jumping}
{***************************************************************************}
procedure StrifeLeft;
begin
 if by_Strife=0
  then begin
        Inc(by_Strife);
        by_StrifeStage:=0;
       end;
 if by_Strife=1
  then begin
        Inc(by_StrifeStage);
        if by_StrifeStage<11
         then begin
               Decr(Neck.ga,wVel/2);
               Incr(LH.Tors.al,wVel*3); {Top}
               Decr(RH.Tors.al,wVel*3);
               Incr(LH.Shld.ga,wVel);
               Decr(LH.FArm.al,wVel*5);
               Incr(LH.FArm.be,wVel*2);
               Decr(RH.FArm.al,wVel*2);
               Decr(RH.FArm.be,wVel*2);
               Incr(LL.Tazz.ga,wVel/2); {Bottom}
               Incr(RL.Tazz.ga,wVel/2);
               Decr(LL.Knee.al,wVel*2);
               Incr(LL.Foot.al,wVel*4);
               Incr(LL.Knee.ga,wVel);
               Decr(RL.Knee.ga,wVel);
              end
         else begin
               Incr(Neck.ga,wVel/2);
               Decr(LH.Tors.al,wVel*3); {Top}
               Incr(RH.Tors.al,wVel*3);
               Decr(LH.Shld.ga,wVel);
               Incr(LH.FArm.al,wVel*5);
               Decr(LH.FArm.be,wVel*2);
               Incr(RH.FArm.al,wVel*2);
               Incr(RH.FArm.be,wVel*2);
               Decr(LL.Tazz.ga,wVel/2); {Bottom}
               Decr(RL.Tazz.ga,wVel/2);
               Incr(LL.Knee.al,wVel*2);
               Incr(LL.Knee.ga,wVel);
               Decr(RL.Knee.ga,wVel);
               Decr(LL.Foot.al,wVel*4);
               if by_StrifeStage>=20
                then begin
                      Inc(by_Strife);
                      by_StrifeStage:=0;
                     end;
              end;
       end;
 if by_Strife=2
  then begin
        Inc(by_StrifeStage);
        if by_StrifeStage<11
         then begin
               Incr(Neck.ga,wVel/2);
               Decr(LH.Tors.al,wVel*3); {Top}
               Incr(RH.Tors.al,wVel*3);
               Decr(RH.Shld.ga,wVel);
               Decr(RH.FArm.al,wVel*5);
               Decr(RH.FArm.be,wVel*2);
               Decr(LH.FArm.al,wVel*2);
               Incr(LH.FArm.be,wVel*2);
               Decr(LL.Tazz.ga,wVel/2); {Bottom}
               Decr(RL.Tazz.ga,wVel/2);
               Decr(RL.Knee.al,wVel*2);
               Incr(RL.Foot.al,wVel*4);
               Decr(LL.Knee.ga,wVel);
               Incr(RL.Knee.ga,wVel);
              end
         else begin
               Decr(Neck.ga,wVel/2);
               Incr(LH.Tors.al,wVel*3); {Top}
               Decr(RH.Tors.al,wVel*3);
               Incr(RH.Shld.ga,wVel);
               Incr(RH.FArm.al,wVel*5);
               Incr(RH.FArm.be,wVel*2);
               Incr(LH.FArm.al,wVel*2);
               Decr(LH.FArm.be,wVel*2);
               Incr(LL.Tazz.ga,wVel/2); {Bottom}
               Incr(RL.Tazz.ga,wVel/2);
               Incr(RL.Knee.al,wVel*2);
               Decr(RL.Foot.al,wVel*4);
               Decr(LL.Knee.ga,wVel);
               Incr(RL.Knee.ga,wVel);
               if by_StrifeStage>=20
                then begin
                      by_Strife:=0;
                      bo_StrifeLeft:=No;
                     end;
              end;
       end;
end;                         {StrifeLeft}
{***************************************************************************}
procedure Breathing;
begin
 if bo_BreathIn
  then begin
        Decr(Chst.al,wVel/14);
        if Chst.al.nom<=Chst.al.min then bo_BreathIn:=No;
       end
  else begin
        Incr(Chst.al,wVel/14);
        if Chst.al.nom>=Chst.al.max then bo_BreathIn:=Yes;
       end
end;                         {Breathing}
{***************************************************************************}
procedure StartRun;
begin
 if LLeg_forw
  then begin
        Incr(Neck.al,wVel*r_Temp/4);
        Incr(LH.Tors.al,wVel*r_Temp/2);
        Decr(RH.Tors.al,wVel*r_Temp/2);
        Incr(LH.Shld.al,wVel*r_Temp/2);
        Decr(RH.Shld.al,wVel*r_Temp/2);
        Incr(LH.FArm.al,wVel*r_Temp/2);
        Decr(RH.FArm.al,wVel*r_Temp/2);
        Incr(LH.FArm.be,wVel*r_Temp);
        Decr(RH.FArm.be,wVel*r_Temp);
        Decr(LL.Knee.al,wVel*r_Temp*2); {Legs}
        Incr(LL.Foot.al,wVel*r_Temp*2);
        Incr(RL.Knee.al,wVel*r_Temp);
        Incr(RL.Fing.al,wVel*r_Temp);
        Decr(RL.Edge.al,wVel*r_Temp*2);
        if LL.Knee.al.nom<=-Pi/3
         then begin
               Inc(by_Run);
               bo_Flying:=Yes;
              end;
       end
  else begin {not LLeg_forw}
        Incr(Neck.al,wVel*r_Temp/4);
        Decr(LH.Tors.al,wVel*r_Temp/2);
        Incr(RH.Tors.al,wVel*r_Temp/2);
        Decr(LH.Shld.al,wVel*r_Temp/2);
        Incr(RH.Shld.al,wVel*r_Temp/2);
        Decr(LH.FArm.al,wVel*r_Temp/2);
        Incr(RH.FArm.al,wVel*r_Temp/2);
        Decr(LH.FArm.be,wVel*r_Temp);
        Incr(RH.FArm.be,wVel*r_Temp);
        Decr(RL.Knee.al,wVel*r_Temp*2); {Legs}
        Incr(RL.Foot.al,wVel*r_Temp*2);
        Incr(LL.Knee.al,wVel*r_Temp);
        Incr(LL.Fing.al,wVel*r_Temp);
        Decr(LL.Edge.al,wVel*r_Temp*2);
        if RL.Knee.al.nom<=-Pi/3
         then begin
               Inc(by_Run);
               bo_Flying:=Yes;
              end;
       end;
end;
{***************************************************************************}
procedure Running;
var w_H : Word;
begin
 if by_RunStage=0
  then if LLeg_forw
        then begin
              Incr(LH.Tors.al,wVel*r_Temp/3);
              Decr(RH.Tors.al,wVel*r_Temp/3);
              Incr(LH.Shld.al,wVel*r_Temp/3);
              Decr(RH.Shld.al,wVel*r_Temp/3);
              Incr(LH.FArm.al,wVel*r_Temp/2);
              Decr(RH.FArm.al,wVel*r_Temp/2);
 {             Decr(LL.Knee.al,wVel*1);} {Legs}
{              Incr(RL.Knee.al,wVel/2);}
              Decr(LL.Foot.al,wVel*r_Temp*2);
              Incr(RL.Foot.al,wVel*r_Temp*2);
{              if LL.Knee.al.nom<=-Pi/3}
              Dec(GC.y,1);
              if LL.Foot.al.nom<=Pi/12
               then begin
                     Inc(by_RunStage);
 {                    LLeg_forw:=not LLeg_forw;}
                    end;
             end
        else begin
              Decr(LH.Tors.al,wVel*r_Temp/3);
              Incr(RH.Tors.al,wVel*r_Temp/3);
              Decr(LH.Shld.al,wVel*r_Temp/3);
              Incr(RH.Shld.al,wVel*r_Temp/3);
              Decr(LH.FArm.al,wVel*r_Temp/2);
              Incr(RH.FArm.al,wVel*r_Temp/2);
{              Decr(RL.Knee.al,wVel*1);
              Incr(LL.Knee.al,wVel/2);}
              Decr(RL.Foot.al,wVel*r_Temp*2); {Legs}
              Incr(LL.Foot.al,wVel*r_Temp*2);
{              if RL.Knee.al.nom<=-Pi/3}
              Dec(GC.y,1);
              if RL.Foot.al.nom<=Pi/12
               then begin
                     Inc(by_RunStage);
{                     LLeg_forw:=not LLeg_forw;}
                    end;
             end;
 if by_RunStage=1
  then if LLeg_forw
        then begin
              Decr(LH.Tors.al,wVel*r_Temp*1);
              Incr(RH.Tors.al,wVel*r_Temp*1);
              Decr(LH.Shld.al,wVel*r_Temp/2);
              Incr(RH.Shld.al,wVel*r_Temp/2);
              Decr(LH.FArm.al,wVel*r_Temp);
              Incr(RH.FArm.al,wVel*r_Temp);

              Incr(LL.Knee.al,wVel*r_Temp*2);

{              if LL.Knee.al.nom<Pi/5 then begin   }
              Inc(GC.y,1);
              w_H:=Height(RL)+GC.y;
              if w_H>Land then bo_Flying:=No;
 {                                          end;  }

              if LL.Foot.al.nom>0 then Decr(LL.Foot.al,wVel*r_Temp*2);
              if LL.Knee.al.nom>=0
               then begin
                     Incr(LL.Fing.al,wVel*r_Temp*2);
                     Decr(LL.Edge.al,wVel*r_Temp*4);
                    end;
              Decr(RL.Knee.al,wVel*r_Temp*2);
              if RL.Knee.al.nom>0 then Incr(RL.Foot.al,wVel*r_Temp*2)
                                  else begin
                                        Decr(RL.Foot.al,wVel*r_Temp/2);
                                        if RL.Fing.al.nom>-Pi/2
                                         then Decr(RL.Fing.al,wVel*r_Temp*1)
                                         else RL.Fing.al.nom:=-Pi/2;
                                        if RL.Edge.al.nom<0
                                         then Incr(RL.Edge.al,wVel*r_Temp*2)
                                         else RL.Edge.al.nom:=0;
                                       end;
 {             Dec(GC.y);}
              if LL.Knee.al.nom>=Pi/6
               then begin
                     by_RunStage:=0;
                     LLeg_forw:=not LLeg_forw;
                     bo_Flying:=Yes;
{                     if not bo_Doing then Inc(by_Run);}       {/////////////}
                    end;
             end
        else begin
              Incr(LH.Tors.al,wVel*r_Temp*1);
              Decr(RH.Tors.al,wVel*r_Temp*1);
              Incr(LH.Shld.al,wVel*r_Temp/2);
              Decr(RH.Shld.al,wVel*r_Temp/2);
              Incr(LH.FArm.al,wVel*r_Temp);
              Decr(RH.FArm.al,wVel*r_Temp);

              Incr(RL.Knee.al,wVel*r_Temp*2);

{              if RL.Knee.al.nom<Pi/5 then begin}
              Inc(GC.y,1);
              w_H:=Height(LL)+GC.y;
              if w_H>Land then bo_Flying:=No;
 {                                          end;}

              if RL.Foot.al.nom>0 then Decr(RL.Foot.al,wVel*r_Temp*2);
              if RL.Knee.al.nom>=0
               then begin
                     Incr(RL.Fing.al,wVel*r_Temp*2);
                     Decr(RL.Edge.al,wVel*r_Temp*4);
                    end;
              Decr(LL.Knee.al,wVel*r_Temp*2);
              if LL.Knee.al.nom>0 then Incr(LL.Foot.al,wVel*r_Temp*2)
                                  else begin
                                        Decr(LL.Foot.al,wVel*r_Temp/2);
                                        if LL.Fing.al.nom>-Pi/2
                                         then Decr(LL.Fing.al,wVel*r_Temp*1)
                                         else LL.Fing.al.nom:=-Pi/2;
                                        if LL.Edge.al.nom<0
                                         then Incr(LL.Edge.al,wVel*r_Temp*2)
                                         else LL.Edge.al.nom:=0;
                                       end;
 {             Dec(GC.y);}
              if RL.Knee.al.nom>=Pi/6
               then begin
                     by_RunStage:=0;
                     LLeg_forw:=not LLeg_forw;
                     bo_Flying:=Yes;
{                     if not bo_Doing then Inc(by_Run);}       {/////////////}
                    end;
             end;
end;
{***************************************************************************}
procedure StopRun;
begin
 bo_Run:=No;
 by_Run:=0;
end;
{***************************************************************************}
procedure Run;
var w_HL, w_HR : Word;
begin
 if by_Run=0 then Inc(by_Run);
 if by_Run=1 then StartRun;
 if by_Run=2 then Running;
 if by_Run=3 then StopRun;
 if not bo_Flying
  then begin
        w_HL:=Height(LL); w_HL:=Land-w_HL;
        w_HR:=Height(RL); w_HR:=Land-w_HR;
        if w_HR<w_HL then GC.y:=w_HR
                     else GC.y:=w_HL;
       end;
end;                         {Run}
{***************************************************************************}
begin
 InitSceleton;
 SetColor(RL.Lcolor); OutTextXY(10,10,'right');
 SetColor(LL.Lcolor); OutTextXY(10,20,'left');
 SetColor(10); OutTextXY(500,10,'fps :');
               OutTextXY(500,20,'key :');
 OutTextXY(10,40,'w - View-up');
 OutTextXY(10,50,'s - View-down');
 OutTextXY(10,60,'a - View-left');
 OutTextXY(10,70,'d - View-right');
{ OutTextXY(10,85,'z - View-leftback');
 OutTextXY(10,95,'x - View-rightback');
 OutTextXY(10,130,'Walk like in Quike!');
 OutTextXY(10,160,'q - Land up');
 OutTextXY(10,170,'e - Land down');}
 OutTextXY(10,190,'v - View skeleton');
 OutTextXY(10,200,'r - refresh (on/off)');
 Quit:=No;
 Velocity:=7{7}; {14ppt~=5km/h - line velocity}
 {wVel:=Pi/40;}{0.08 0.078} {angle velocity}
 wVel:=Pi/150{150};
 tTurn:=6000;
{ r_1:=tTurn; Overload:=Round(r_1*50);}
 de_be:=0{0.01}; {rotating around y-axis}
 de_x:=0;
{ GC.x:=20;}
 r_Temp:=1.2;
 WalkStart:=Yes;
 LLeg_forw:=Yes;
 bo_BreathIn:=Yes;
{ w_Heigth:=Height;}
 bo_Jump:=No;       by_Jump:=0;
 r_JumpHight:=0.4;
 bo_StrifeLeft:=No; by_Strife:=0;
 bo_Run:=No;        by_Run:=0;
 {Fall:=No;}
 Abs_X:=GC.x-120;
 {}
{ GC.x:=50;}
 {}
 bo_refresh:=Yes;
 bo_Show:=Yes;
 fps_num:=0;
 GetTime(Hour,Min,Sec,Hund);
 bo_Flying:=No;
 bo_Doing:=No;
 repeat
  if KeyPressed
   then begin
         Ch:=ReadKey;
         case Ch of
          #13, #27 : Quit:=Yes;
          #32 : begin bo_Jump:=Yes; bo_Doing:=Yes; end;
          'O', 'o' : InitSceleton;
          'J', 'j' : begin bo_Jump:=Yes; bo_Doing:=Yes; end;
          'R', 'r' : bo_refresh:=not bo_refresh;
          'V', 'v' : bo_Show:=not bo_Show;
          'Z', 'z' : begin GC.ga:=GC.ga+0.05; bo_Doing:=No; end;
          'X', 'x' : begin GC.ga:=GC.ga-0.05; bo_Doing:=No; end;
          'W', 'w' : begin GC.al:=GC.al+0.05; bo_Doing:=No; end;
          'S', 's' : begin GC.al:=GC.al-0.05; bo_Doing:=No; end;
          'A', 'a' : begin GC.be:=GC.be-0.05; bo_Doing:=No; end;
          'D', 'd' : begin GC.be:=GC.be+0.05; bo_Doing:=No; end;
          'Q', 'q' : begin
                      SetColor(0); Line(0,Land,639,Land);
                      if Land>180 then Dec(Land);
                      SetColor(15); Line(0,Land,639,Land);
                     end;
          'E', 'e' : begin
                      SetColor(0); Line(0,Land,639,Land);
                      if Land<470 then Inc(Land);
                      SetColor(15); Line(0,Land,639,Land);
                     end;
          #0 : begin
                Ch:=ReadKey;
                case Ch of
                 #72 : begin bo_Run:=Yes; bo_Doing:=Yes; end;            {Up}
                 #80 : begin bo_Walk:=Yes; bo_Doing:=Yes; end;         {Down}
                 #75 : begin bo_StrifeLeft:=Yes; bo_Doing:=Yes; end;   {Left}
                 #77 : {GC.be:=GC.be+0.05};                           {Right}
                 #115 : ; {Ctrl+Left}
                 #116 : ; {Ctrl+Right}
                 else begin
                       Str(Ord(Ch),s_1);
                       SetFillStyle(1,2); Bar(540,20,580,28);
                       SetColor(10); OutTextXY(550,20,s_1);
                      end;
                end;
               end
         else begin
               {Walk_forw:=No;}
               bo_Doing:=Yes;
              end
         end;

{         if bo_Doing then
          if bo_Jump then Jumping;
         if bo_Doing then
          if bo_StrifeLeft then StrifeLeft;
         if bo_Doing then
          if bo_Run then Run;}

        end {case}
    else {Walk_forw:=No};
         {if b_refresh
          then begin
                SetFillStyle(1,2);
                Bar(200,Land-170,450,Land+10);
                SetColor(15); Line(0,Land,639,Land);
               end;}
       { end; KeyPressed}
  if bo_refresh
   then begin
         SetFillStyle(1,2);
         Bar(200,GC.y-70,450,GC.y+110);
         SetColor(15); Line(0,Land,639,Land);
        end;
    {Demo settings start}
{  GC.be:=GC.be+de_be;}
{  GC.x:=GC.x+de_x;}
    {Demo settings end}
  if bo_Jump then Jumping;
  if bo_StrifeLeft then StrifeLeft;
  if bo_Run then Run;
  if bo_Walk then begin
                   WalkLegs;
                   WalkHands;
                  end;

{ if Walk_Forw then}  {Walk;}
{ Run;}
{  WalkLegs;
  WalkHands;}
  CalcStages;
  if bo_Show then ShowStages;
  Breathing;
  Wait(tTurn);
  Inc(fps_num);
  if fps_num>=100 then Calc_FPS;
 until Quit;
{ CloseGraph;}
end.
