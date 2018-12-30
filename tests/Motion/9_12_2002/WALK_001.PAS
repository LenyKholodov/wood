{***************************************************************************}
{  _Doing_Command:                               Priority:                  }
{  ~~~~~~~~~~~~~~~                               ~~~~~~~~~                  }
{ 000 - standing (nothing)                       stand                      }
{ 001 - walking fw                               walk                       }
{ 002 - walking bw                               run                        }
{ 003 - running fw                               jump                       }
{ 004 - running bw                               crouch                     }
{ --- - slide left                                                          }
{ --- - slide right                                                         }
{ 005 - jump                                     +jump (?)                  }

{ --- - crouch down                              +turn (?)                  }
{ --- - crouch up                                inv-cin (?)                }
{ --- - walk crouch fw                           +inv-cin (?)               }
{ --- - walk crouch bw                                                      }
{ --- - lean left                                                           }
{ --- - lean right                                                          }
{---------------------------------------------------------------------------}
{ Contents of movements:                                                    }
{ ~~~~~~~~~~~~~~~~~~~~~~                                                    }
{ standing - outstanding :) movement                                        }
{ walking = start walking + walking + finish walking                        }
{ run = srart running + running + finish running                            }
{ crouch = crouch up / crouch down                                          }
{ jump = start jumping (int-d) + jumping up (!int-d) + jumping down (!int-d)}
{        + finish jumping (!int-d)                                          }
{ *int-d - interrupted                                                      }
{ runjump = jumping up (!int-d) + jumping down (!int-d)                     }
{---------------------------------------------------------------------------}
{  _Doing_Now:                                                              }
{  ~~~~~~~~~~~                                                              }
{ 000 - stand still ("s-stand" - downsteps) (ss)                  (-stage-)û}
{ 001 - s-stand -> walking fw (ss-wf)                          (-movement-)û}
{ 002 - s-stand -> walking bw (ss-wb)                          (-movement-)û}
{ 003 - s-stand -> running fw (ss-rf)                          (-movement-)û}
{ 004 - s-stand -> running bw (ss-rb)                          (-movement-)û}
{ 005 - s-stand -> slide left  (ss-sl)                         (-movement-) }
{ 006 - slide left -> s-stand (sl-ss)                          (-movement-) }
{ 007 - s-stand -> slide right (ss-sr)                         (-movement-) }
{ 008 - slide right -> s-stand (sr-ss)                         (-movement-) }
{ 009 - s-stand -> turn left  (sstl)                           (-movement-) }
{ 010 - s-stand -> turn right (sstr)                           (-movement-) }
{ 011 - s-stand -> jump start (ss-js)                          (-movement-) }
{ 012 - stand still -> crouch (ss-c)                           (-movement-) }
{ 013 - s-stand -> lean left  (ss-ll)                          (-movement-) }
{ 014 - s-stand -> lean right (ss-lr)                          (-movement-) }
{ 015 - s-stand lean left  (ssll)                                 (-stage-) }
{ 016 - lean left -> s-stand (ll-ss)                           (-movement-) }
{ 017 - s-stand lean right (sslr)                                 (-stage-) }
{ 018 - lean right -> s-stand (lr-ss)                          (-movement-) }

{ 101 - s-stand crouch (ssc)                                      (-stage-) }
{ 102 - s-stand crouch -> stand still (ssc-ss)                 (-movement-) }
{ 103 - s-stand crouch -> walking crouch fw (ssc-wfc)          (-movement-) }
{ 104 - s-stand crouch -> walking crouch bw (ssc-wbc)          (-movement-) }
{ 105 - s-stand crouch -> slide left crouch (ssc-slc)          (-movement-) }
{ 106 - slide left crouch -> s-stand crouch (slc-ssc)          (-movement-) }
{ 107 - s-stand crouch -> slide right crouch (ssc-src)         (-movement-) }
{ 108 - slide right crouch -> s-stand crouch (src-ssc)         (-movement-) }
{ 109 - s-stand crouch -> turn left (ssctl)                    (-movement-) }
{ 110 - s-stand crouch -> turn right (ssctr)                   (-movement-) }
{ 111 - s-stand crouch -> jump up (ssc-ju)                     (-movement-) }
{ 112 - s-stand crouch -> lean left crouch (ssc-llc)           (-movement-) }
{ 113 - s-stand crouch -> lean right crouch (ssc-lrc)          (-movement-) }
{ 114 - s-stand crouch lean left (sscll)                          (-stage-) }
{ 115 - s-stand crouch lean left -> s-stand crouch (sscll-ssc) (-movement-) }
{ 116 - s-stand crouch lean right (ssclr)                         (-stage-) }
{ 117 - s-stand crouch lean right -> s-stand crouch (ssclr-ssc)(-movement-) }

{ 200 - walking forward (wkfw)                                 (-movement-) }
{ 201 - walking backward (wkbw)                                (-movement-) }
{ 202 - walking fw -> s-stand (wkfw-ss)                        (-movement-) }
{ 203 - walking bw -> s-stand (wkbw-ss)                        (-movement-) }
{ 204 - walking fw -> running fw (wkfw-rnfw)                   (-movement-) }
{ 205 - walking bw -> running bw (wkbw-rnbw)                   (-movement-) }
{ 206 - walking fw -> turn left (wkfwtl)                       (-movement-) }
{ 207 - walking fw -> turn right (wkfwtr)                      (-movement-) }
{ 208 - walking bw -> turn left (wkbwtl)                       (-movement-) }
{ 209 - walking bw -> turn right (wkbwtr)                      (-movement-) }
{ 210 - walking fw -> jump up (wkfw-ju)                        (-movement-) }
{ 211 - walking bw -> jump up (wkbw-ju)                 (???)  (-movement-) }
{ 212 - walking fw -> lean left (wkfw-ll)                      (-movement-) }
{ 213 - walking fw -> lean right (wkfw-lr)                     (-movement-) }
{ 214 - walking bw -> lean left (wkfw-ll)                      (-movement-) }
{ 215 - walking bw -> lean right (wkfw-lr)                     (-movement-) }
{ 216 - walking fw lean left (wkfwll)                          (-movement-) }
{ 217 - walking fw lean right (wkfwlr)                         (-movement-) }
{ 218 - walking fw lean left -> walking fw (wkfwll-wkfw)       (-movement-) }
{ 219 - walking fw lean right -> walking fw (wkfwlr-wkfw)      (-movement-) }
{ 220 - walking bw lean left (wkbwll)                          (-movement-) }
{ 221 - walking bw lean right (wkbwlr)                         (-movement-) }
{ 222 - walking bw lean left -> walking bw (wkbwll-wkbw)       (-movement-) }
{ 223 - walking bw lean right -> walking bw (wkbwlr-wkbw)      (-movement-) }

{ 400 - walking crouch forward (wkcfw)                         (-movement-) }
{...}
{ 500 - walking crouch backward (wkcbw)                        (-movement-) }
{...}

{ 601 - running forward (rnfw)                                 (-movement-) }
{ 602 - running backward (rnbw)                                (-movement-) }
{ 603 - running fw -> walking fw (rnfw-wkfw)                   (-movement-) }
{ 604 - running bw -> walking bw (rnbw-wkbw)                   (-movement-) }
{ 605 - running fw -> turn left (rnfwtl)                       (-movement-) }
{ 606 - running fw -> turn right (rnfwtr)                      (-movement-) }
{ 607 - running bw -> turn left (rnbwtl)                       (-movement-) }
{ 608 - running bw -> turn right (rnbwtr)                      (-movement-) }
{ 609 - running fw -> jump up (rnfw-ju)                        (-movement-) }
{ 610 - running bw -> jump up (rnbw-ju)                 (???)  (-movement-) }

{ NB: strife left-right !!!                                                 }
{...}

{                                                                           }
{***************************************************************************}
uses Crt;
var _Quit : Boolean;
    {_Movement : String;}
    _Ch : Char;
    _Doing_Command : Byte;      {input command}
    _Doing_Now : Word;          {current movement}
    _Need_To_Do : Word;         {movement next to current}
    _Done_Movement_Flag : Integer; {-1 when current move is finished}
const Yes : Boolean = True;
       No : Boolean = False;
{***************************************************************************}
procedure Show(_New_Move:Integer);
begin
 {start new move - _New_Move:=_Need_To_Do}
end;
{***************************************************************************}
procedure ContinueMoving;
begin
 {when current move is done:}
{ _Done_Movement_Flag:=-1; }
end;
{***************************************************************************}
BEGIN
 _Quit:=False;
{ _Movement:='';}
 repeat
  {_Doing:=0;}
  _Doing_Now:=0;
  _Done_Movement_Flag:=-1;
 {--AI engine--key-emulation--} {-------------------------------------------}
  if KeyPressed
   then begin
         _Ch:=ReadKey;
         case _Ch of
          #13, #27 : _Quit:=Yes;
          'R', 'r' : {bo_refresh:=not bo_refresh};
          'V', 'v' : {bo_Show:=not bo_Show};
          #32 : _Doing_Command:=5;  {Jump}
          'W' : _Doing_Command:=3;  {Run Forward}
          'w' : _Doing_Command:=1;  {Walk Forward}
          'S' : _Doing_Command:=4;  {Run Backward}
          's' : _Doing_Command:=2;  {Walk Backward}
          (*
          'A' : _Doing_Command:={Run Left};
          'a' : _Doing_Command:={Walk Left};
          'D' : _Doing_Command:={Run Right};
          'd' : _Doing_Command:={Walk Right};
          'Q' : _Doing_Command:={Lean Left};
          'q' : _Doing_Command:={Lean Left};
          'E' : _Doing_Command:={Lean Right};
          'e' : _Doing_Command:={Lean Right};
          *)
          #0 : begin
               end; {case #0}
         end {case Ch}
        end {if KeyPressed}
   else _Doing_Command:=0; {do nothing}
 {--movement engine--} {----------------------------------------------------}
(*
   000 : case _Doing_Command of
           0 : _Need_To_Do:=000; {Do Nothing}
           1 : _Need_To_Do:=000; {Walk Forward}
           2 : _Need_To_Do:=000; {Walk Backward}
           3 : _Need_To_Do:=000; {Run Forward}
           4 : _Need_To_Do:=000; {Run Backward}
           5 : _Need_To_Do:=000; {Jump}
         end; {case}
*)
  case _Doing_Now of
   000 : case _Doing_Command of {stand still (ss)}
           0 : _Need_To_Do:=000; {Do Nothing} {random s-stand move}
           1 : _Need_To_Do:=001; {Walk Forward}     {ss-wf}
           2 : _Need_To_Do:=002; {Walk Backward}    {ss-wb}
           3 : _Need_To_Do:=003; {Run Forward}      {ss-rf}
           4 : _Need_To_Do:=004; {Run Backward}     {ss-rf}
           5 : _Need_To_Do:=011; {Jump}             {ss-js}
         end; {case}
   001 : case _Doing_Command of {s-stand -> walking fw (ss-wf)}
           0 : _Need_To_Do:=202; {Do Nothing}       {wkfw-ss}
           1 : _Need_To_Do:=200; {Walk Forward}     {wkfw}
           2 : _Need_To_Do:=202; {Walk Backward}    {wkfw-ss}
           3 : _Need_To_Do:=204; {Run Forward}      {wkfw-rnfw}
           4 : _Need_To_Do:=202; {Run Backward}     {wkfw-ss}
           5 : _Need_To_Do:=210; {Jump}             {wkfw-ju}
         end; {case}
   002 : case _Doing_Command of {s-stand -> walking bw (ss-wb)}
           0 : _Need_To_Do:=203; {Do Nothing}        {wkbw-ss}
           1 : _Need_To_Do:=203; {Walk Forward}      {wkbw-ss}
           2 : _Need_To_Do:=201; {Walk Backward}     {wkbw}
           3 : _Need_To_Do:=203; {Run Forward}       {wkbw-ss}
           4 : _Need_To_Do:=205; {Run Backward}      {wkbw-rnbw}
           5 : _Need_To_Do:=211; {Jump}              {wkbw-ju (???)}
         end; {case}
   003 : case _Doing_Command of {s-stand -> running fw (ss-rf)}
           0 : _Need_To_Do:=603; {Do Nothing}        {rnfw-wkfw}
           1 : _Need_To_Do:=603; {Walk Forward}      {rnfw-wkfw}
           2 : _Need_To_Do:=603; {Walk Backward}     {rnfw-wkfw}
           3 : _Need_To_Do:=601; {Run Forward}       {rnfw}
           4 : _Need_To_Do:=603; {Run Backward}      {rnfw-wkfw}
           5 : _Need_To_Do:=609; {Jump}              {rnfw-ju}
         end; {case}
   004 : case _Doing_Command of {s-stand -> running bw (ss-rb)}
           0 : _Need_To_Do:=604; {Do Nothing}        {rnbw-wkbw}
           1 : _Need_To_Do:=604; {Walk Forward}      {rnbw-wkbw}
           2 : _Need_To_Do:=604; {Walk Backward}     {rnbw-wkbw}
           3 : _Need_To_Do:=604; {Run Forward}       {rnbw-wkbw}
           4 : _Need_To_Do:=602; {Run Backward}      {rnbw}
           5 : _Need_To_Do:=610; {Jump}              {rnbw-ju}
         end; {case}
(*   005 : case _Doing_Command of {s-stand -> slide left  (ss-sl)}
           0 : _Need_To_Do:=000; {Do Nothing}
           1 : _Need_To_Do:=000; {Walk Forward}
           2 : _Need_To_Do:=000; {Walk Backward}
           3 : _Need_To_Do:=000; {Run Forward}
           4 : _Need_To_Do:=000; {Run Backward}
           5 : _Need_To_Do:=000; {Jump}
         end; {case}*)
(*   006 : case _Doing_Command of {slide left -> s-stand (sl-ss)}
           0 : _Need_To_Do:=000; {Do Nothing}
           1 : _Need_To_Do:=000; {Walk Forward}
           2 : _Need_To_Do:=000; {Walk Backward}
           3 : _Need_To_Do:=000; {Run Forward}
           4 : _Need_To_Do:=000; {Run Backward}
           5 : _Need_To_Do:=000; {Jump}
         end; {case}*)

   200 : case _Doing_Command of {walking forward (wkfw)}
           0 : _Need_To_Do:=202; {Do Nothing}        {wkfw-ss}
           1 : _Need_To_Do:=200; {Walk Forward}      {wkfw}
           2 : _Need_To_Do:=202; {Walk Backward}     {wkfw-ss}
           3 : _Need_To_Do:=204; {Run Forward}       {wkfw-rnfw}
           4 : _Need_To_Do:=202; {Run Backward}      {wkfw-ss}
           5 : _Need_To_Do:=210; {Jump}              {wkfw-ju}
         end; {case}
   201 : case _Doing_Command of {walking backward (wkbw)}
           0 : _Need_To_Do:=203; {Do Nothing}        {wkbw-ss}
           1 : _Need_To_Do:=203; {Walk Forward}      {wkbw-ss}
           2 : _Need_To_Do:=201; {Walk Backward}     {wkbw}
           3 : _Need_To_Do:=203; {Run Forward}       {wkbw-ss}
           4 : _Need_To_Do:=205; {Run Backward}      {wkbw-rnbw}
           5 : _Need_To_Do:=211; {Jump}              {wkbw-ju (???)}
         end; {case}


  end;

  (*
  case _Doing_Command of
     0 : _Movement:=''; {Random stand movement}
     1 : case _Movement[1] of {Walk fw}
          ''  : _Movement:='1'; {s-stand -> walking}
          '1' : _Movement[2]:='7'; {walking -> walking}
          '
         end;
  end; {case}
  *)
 {--graphic engine--} {-----------------------------------------------------}
  if _Done_Movement_Flag=-1
   then Show(_Need_To_Do)
   else ContinueMoving;
 until _Quit;

END.