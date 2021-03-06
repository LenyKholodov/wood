// ��������� ��类� ������, ���ᠭ��� � "expr.h"
#include "expr.h"

// �. ���ᠭ��
char* SkipSpace(char* s)
{
 while(isspace(*s) && *s)
  s++;
 return s;
}

// �. ���ᠭ��
char* lexema(char* s,size_t* l)
{
 if(*(s=SkipSpace(s))=='\0' || (!isalpha(*s) && *s!='_'))
  *l=0;
 else
  for(*l=1;s[*l]=='_' || isalpha(s[*l]) || isdigit(s[*l]);(*l)++);
 return s;
}

// ���� ����� �㭪樨 � ��ப�
int DefFn::SetupName(void)
{
 FnName="";
 size_t l;
 CurrentPtr=lexema(CurrentPtr,&l);	// ���ࣨ���� ���ᥬ�...
 if(l)	// ��� ����?
 {
  char tmp=CurrentPtr[l];	// �� - ��࠭塞 ���
  CurrentPtr[l]='\0';
  FnName=CurrentPtr;
  CurrentPtr[l]=tmp;
  CurrentPtr+=l;    		// ��६�頥��� ����� �� �����.
 }
 return OK;
}

// ����� ������ �㭪樨
void DefFn::printOn( ostream _FAR& os) const
{
	os<<"Function name: ";
	if(FnName==String(""))
		os<<"(empty)";
	else
		os<<FnName;
	os<<endl<<"Parameters:";
	if(parameters.top())
		for(int i=0;i<parameters.top();os<<endl<<*parameters[i++]);
	else
		os<<"void";
	os<<endl;
	os<<expression;
}

// ��⠭���� ��ࠬ��஢
int DefFn::SetupParameters(void)
{
	parameters.flush(1);
	// ���� ᨬ��� - ���뢠��� ᪮���, �᫨ ���,
	// � ��⠥�, �� ��ࠬ��஢ ���.
	if(*(CurrentPtr=SkipSpace(CurrentPtr))!='(')
		return OK;

	size_t l;
	String* s;
	// �饬 ���ᥬ�...
	for(CurrentPtr=lexema(++CurrentPtr,&l);l;)
	{
		char tmp=CurrentPtr[l];
		CurrentPtr[l]='\0';
		s=new String(CurrentPtr); // �����뢥� ���...
		CurrentPtr[l]=tmp;
		if(s==NULL)
		{
			parameters.flush(1);
			return ERROR_NOMEM;
		}
		// �᫨ ⠪�� ��� 㦥 ����...
		if(parameters.find(s)!=UINT_MAX)
		{
			parameters.flush(1);
			delete s;
			// ������� � ��室��
			return ERROR_MULTIPLIE_DECLARATION;
		}
		// ���� ������塞 ���窮
		parameters.add(s);
		// ᬮ�ਬ �� ��᫥ ����� -
		switch(*(CurrentPtr=SkipSpace(CurrentPtr+=l)))
		{
			// ᪮��� - ��, ����� ࠡ���
			case ')': CurrentPtr++; return OK;
			// ᫥���騩 ��ࠬ��� - ⮦� ��.
			case ';':
			case ',': CurrentPtr=lexema(++CurrentPtr,&l); break;
			// ���� - ���宥 ���ᠭ��
			default:  parameters.flush(1); return ERROR_BAD_DECLARATION;
		}
	}
	// ᬮ�ਬ, ��� ��ࠡ�⠫���.
	switch(*SkipSpace(CurrentPtr))
	{
		// ��⠭������� �� �� ᪮��� - �� ������ ���ᠭ�� ��ࠬ��஢
		case 0: 	return ERROR_OPENED_PARAMETERS_STRING;
		// ��� ��
		case ')':	CurrentPtr++; return OK;
		// �ய�� ��ࠬ���
		case ';':
		case ',':	return ERROR_BAD_DECLARATION;
		// �������⭮ ��, �� ���ࠢ��쭮
		default:	return ERROR;
	}
}

// �⠥� �������� � ��ࠬ���� �㭪樨...
// �ਬ�砭��: �����⨬묨 ������� ᫥���騥 ��ਠ���:
// f(x)=1+3+...
// f=1+sin(pi)*...
// =45+10
// � ����(���⮥ ���) (x,y)=12*x+y+...
int DefFn::SetupDeclare(void)
{
	SetupName();					// �⠥� ��������
	int result=SetupParameters();   // � ��ࠬ����,
	if(result!=OK)					// �᫨ ���ࠢ��쭮 - ��室�� � ᮮ�蠥�
		return result;
	CurrentPtr=SkipSpace(CurrentPtr);
	if(*CurrentPtr!='=')            //�饬 ��砫� ॠ����樨,�᫨ ���, ��室
		return ERROR_BAD_DECLARATION;
	CurrentPtr++;					// ���� ��� ��, ���室�� �� ��ࠦ����
	return OK;
}

// �������� �訡��
const char* DefFn::errorstr(int error) const
{
 switch(error)
 {
	case OK:		return "Ok";
	case ERROR_NOMEM:	return "Error: not enough memory";
	case ERROR_OPENED_PARAMETERS_STRING:
				return "Error: opened parameters string";
	case ERROR_MULTIPLIE_DECLARATION:
				return "Error: multiplie declaration";
	case ERROR_BAD_DECLARATION:
				return "Error: bad declaration";
	case ERROR_LINE_TOO_LONG:
				return "Error: fn-line is too long";
	case ERROR_LINE_TOO_SHORT:
				return "Error: fn-line is too short";
	case ERROR_BAD_IMPLEMENTATION:
				return "Error: bad or incorrect operation";
	case ERROR_FUNCTION_NOT_DEFINED:
				return "Error: function not defined";
	default: 		return "Error: ???";
 }
}
// ���樠������ ��ꥪ�
int DefFn::setup(char* s)
{
	clear();			// ��⨬ ���
	CurrentPtr=s;       // ��砫� �����९�樨 - ��砫� ��ப�
	if((result=SetupDeclare())!=OK)	// �᫨ ����� �� ������樨 - ��室
		return result;
	if((result=expression.setup())!=OK)	// �᫨ ����� �� ��ࠦ���� - ��室
		return result;
	// �᫨ ��᫥ ������� ��ࠦ���� ��⠫��� "墮���" - ��室��
	if(*(CurrentPtr=SkipSpace(CurrentPtr))!='\0')
		return ERROR_LINE_TOO_LONG;
	return OK;		// �� �஡���!
}

// �����頥� ���祭�� ������� ��������� ��ࠦ����
double Expression::value(void) const
{
	// ��ࠦ���� ��⮨� �� �㬬� �ମ�,
	// ���⮬� ��� ���᫥��� - ���� �㬬��㥬 ��
	double v=0.0;
	for(int i=0;i<terms.top();v+=terms[i++]->value());
	return v;
}

// �����頥� ���祭�� �ଠ.
double Term::value(void) const
{
//	��� ��⮨� �� �ந�������� 䠪�஢, ���⮬�
	double v=signum?-1.0:1.0;			// ���뢠�� ���� �ଠ
	for(int i=0;i<factors.top();i++)	// ��ॡ�ࠥ� �� 䠪���
		switch(factors[i]->mode)		// �஢��塞 ०�� 㬭������
		{								// � 㬭�����
			case 0:	v*=factors[i]->value(); break;
			case 1: v/=factors[i]->value(); break;
			case 2: v=floor(v/factors[i]->value()); break;
			case 3: v=fmod(v,factors[i]->value()); break;
		}
	return v;
}

// ���樠������ ��ࠦ����
int Expression::setup(void)
{
	terms.flush(1);			   				// ��⨬ ���
	char* &CurrentPtr=owner->CurrentPtr;    // ����砥� ��뫪� �� 㪠��⥫�
									// ����樨 �����९�樨 ��ப� ⥪��
	if(*(CurrentPtr=SkipSpace(CurrentPtr))=='\0')// ���९��஢��� ��祣�,
		return ERROR_LINE_TOO_SHORT;// ��������� � ��室��
	// �.�. ��ࠦ���� - �� �㬬� �ମ�, ������� ᫥���� ⮫쪮 �� �������
	// "+/-", � ��⠫��� ࠡ��� ������ �� ���९���� ���
	for(Term* t;;)
	{
		if((t=new Term(owner))==NULL)		// ���! ��� �����
		{
			terms.flush(1);					// ��⨬, ��室��
			return ERROR_NOMEM;
		}
		terms.add(t);						// ������塞 ��
		if((owner->result=t->setup())!=OK)	// ���� � �� ��������
		{
			terms.flush(1);
			return	owner->result;
		}
		// ᢥ�塞 �����...
		switch(*(CurrentPtr=SkipSpace(CurrentPtr)))
		{
			case '-':
			case '+':	break;		// ��� ���窨, �த������
			default:	return OK;	// ��� - ��室��, ����, �� ��ࠦ����
									// ���稫���
		}
	}
}

// ���樠������ �ଠ
int Term::setup(void)
{
	factors.flush(1); 	// ��⨬��,
	int mode=signum=0;	// ��⠭�������� ०��
	char* &CurrentPtr=owner->CurrentPtr;
	// ᬮ�ਬ ����, �� 㬮�砭�� - ���� ����
	switch(*(CurrentPtr=SkipSpace(CurrentPtr)))
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;
		case '+':	CurrentPtr++;	break;
	}
	// �.�. �� - �� �ந�������� 䠪�஢, ������� ᫥���� ⮫쪮 �� �������
	// "*/\%", � ��⠫��� ࠡ��� ������ �� ���९���� 䠪���
	// �����쪠� ���ࠢ��: 䠪�� - ��:
	// �᫮, �㭪��, ��६����� ��� ��ࠦ���� � ᪮����
	// ���⮬� �������� ����� 4 ����⪨ ������, �� �� ⠪��
	for(Factor* t;;)
	{
		// �� �㬠��, �� �� �᫮...
		if((t=new Number(owner))==NULL)
		{
			factors.flush(1);
			return ERROR_NOMEM;
		}
		if((owner->result=t->setup())!=OK)
		{	// ��� - ����, �� ��६����...
			delete t;
			if((t=new Variable(owner))==NULL)
			{
				factors.flush(1);
				return ERROR_NOMEM;
			}
			if((owner->result=t->setup())!=OK)
			{
				// ��� - ⮣�� �� ��ࠦ���� � ᪮���...
				delete t;
				if((t=new SubExpression(owner))==NULL)
				{
					factors.flush(1);
					return ERROR_NOMEM;
				}
				if((owner->result=t->setup())!=OK)
				{
					// ᭮�� ��� - �� �㭪��!
					delete t;
					if((t=new MathFunction(owner))==NULL)
					{
						factors.flush(1);
						return ERROR_NOMEM;
					}
					if((owner->result=t->setup())!=OK)
					{
						// �� ������, �� �� ⠪��, ��諨 �� �� #$%...
						factors.flush(1);
						return	owner->result;
					}
				}
			}
		}
		// �� ��-�!
		t->mode=mode; 		// ���������� ⥪�騩 ����,
		factors.add(t);		// ������塞 � ᯨ��
		switch(*(CurrentPtr=SkipSpace(CurrentPtr)))// � ᬮ�ਬ ᫥���騩 ����
		{
			case '*':       mode=0; break;
			case '/':	mode=1; break;
			case '\\':	mode=2; break;
			case '%':	mode=3; break;
			default:	return OK;
		}
		CurrentPtr++;
	}
}

// ���樠������ �᫠...
int Number::setup(void)
{
	char* ptr;
	data=strtod(owner->CurrentPtr,&ptr);
	if(ptr!=owner->CurrentPtr)
	{
		owner->CurrentPtr=ptr;	// �� - �� �᫮, ��७�ᨬ
		// 㪠��⥫� ����⥯�樨 �� ����, � ���� �����
		return OK;
	}
	// ���� - 䮪�� �� ��襫
	return ERROR_BAD_IMPLEMENTATION;
}

// ���樠������ ��६�����
int Variable::setup(void)
{
	size_t l;
	// �뤨ࠥ� ᫮��...
	char* ptr=lexema(owner->CurrentPtr,&l);
	if(l)	// �᫨ ᫮�� ����,
	{
		char tmp=ptr[l];
		ptr[l]='\0';
		// �饬 � ᯨ᪥ ��६�����...
		index=owner->parameters.find(&String(ptr));
		ptr[l]=tmp;
		// ⠪�� ��६����� ���� ?
		if(index!=UINT_MAX)
		{
			// �� - ��� ��!
			owner->CurrentPtr+=l;
			return OK;
		}
	}
	// ���� ���, �訡��
	index=0;
	return ERROR_BAD_IMPLEMENTATION;
}

// ��� ��� ���� - �� ��������� ������ � ᯨ᪥ ��ࠬ��஢,
// �㦭� ��� ����� � �� ���� ������� ���祭��.
double Variable::value(void) const
{
	return owner->ParamValue[index];
}

// �����ࠦ����. �� ���筮� ��ࠦ����, �� � ᪮����
int SubExpression::setup(void)
{
	char *BackUp, *ptr=SkipSpace(BackUp=owner->CurrentPtr);
	int result;
	// ᬮ�ਬ ����,
	switch(*ptr)
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;ptr=SkipSpace(++ptr);break;
		case '+':	signum=0;ptr=SkipSpace(++ptr);break;
	}
	if(*ptr!='(')	// �᫨ �� � ᪮����, � �� �� ��ࠦ���� � ᪮����
					// ����筮, �� ??!
		return ERROR_BAD_IMPLEMENTATION;
	owner->CurrentPtr=SkipSpace(++ptr);
	// ��⠥��� ���९��஢��� ��ࠦ���� ����� ᪮���...
	if((result=expression.setup())!=OK)
	{
		owner->CurrentPtr=BackUp;
		expression.clear();
		return result;
	}
	// �饬 ���������� ᪮���...
	if(*(ptr=SkipSpace(owner->CurrentPtr))!=')')
	{
		owner->CurrentPtr=BackUp;	// ���! ����
		expression.clear();
		return ERROR_BAD_IMPLEMENTATION;	// ����
	}
	owner->CurrentPtr=ptr+1;	// ��� ��, ��६�頥� 㪠��⥫� ��ப�
	return OK;
}

// �㭪��, ���祭��
double MathFunction::value(void) const
{
	double v,d;
	if(arguments.top()>0)
		d=arguments[0]->value();
	switch(IdFn)
	{
		// �㭪樨 ���ᨬ㬠 � �������㬠 - ���-�� ��㬥�⮢ �� ��࠭�祭�
		case 0: v=d;
			for(int i=1;i<arguments.top();)
				if(v>(d=arguments[i++]->value()))
					v=d;
			break;
		case 1: v=d;
			for(i=1;i<arguments.top();)
				if(v<(d=arguments[i++]->value()))
					v=d;
			break;
		// ����� ����୮�, ����⭮
		case 2: v=fabs(d); break;
		case 3: v=d*d; break;
		case 4: v=sqrt(d); break;
		case 5: v=acos(d); break;
		case 6: v=asin(d); break;
		case 7: v=atan(d); break;
		case 8: v=atan2(d,arguments[1]->value());break;
		case 9: v=M_PI; break;
		case 10:v=cos(d); break;
		case 11:v=sin(d); break;
		case 12:v=tan(d); break;
		case 13:v=cosh(d); break;
		case 14:v=sinh(d); break;
		case 15:v=tanh(d); break;
		case 16:v=exp(d); break;
		case 17:v=hypot(d,arguments[1]->value());break;
		case 18:v=log(d);break;
		case 19:v=log(d)/log(arguments[1]->value());break;
		case 20:v=log10(d);break;
		case 21:v=pow(d,arguments[1]->value());break;
		case 22:v=d>0.0?1.0:d<0.0?-1.0:0.0; break;
		case 23:v=floor(d); break;
		case 24:v=ceil(d); break;
	}
	return signum?-v:v;	// ���뢠�� ����
}

// ���樠������ �㭪樨
int MathFunction::setup(void)
{
	char *BackUp, *ptr=SkipSpace(BackUp=owner->CurrentPtr),tmp;
	size_t l;
	int result;
	unsigned int i,ParamCount;
	// 㧭�� ����...
	switch(*ptr)
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;ptr=SkipSpace(++ptr);break;
		case '+':	signum=0;ptr=SkipSpace(++ptr);break;
	}
	// �⠥� ��������
	ptr=lexema(ptr,&l);
	if(l==0)
		return	ERROR;
	tmp=ptr[l];
	ptr[l]='\0';
	// find function
	for(i=0;i<MathFunctionsCount;i++)
		if(strcmp(ptr,MathFunctions[i].name)==0)
		{
			IdFn=i;
			ParamCount=MathFunctions[i].ArgCount;
			break;
		}
	// �� ��諨 - ��� ⠪�� �㭪樨, ��室��
	if(IdFn==UINT_MAX)
		return ERROR_FUNCTION_NOT_DEFINED;
	*(ptr+=l)=tmp;
	// ᪮��� ����?
	if(*(ptr=SkipSpace(ptr))!='(')
	{              // ����,
		if(ParamCount)	// �᫨ � �㭪樨 ������ ���� ��ࠬ����, ��室��
			return ERROR_LINE_TOO_SHORT;
		owner->CurrentPtr=ptr;	// ���� - ��०��� ⠪�� ���
		return OK;
	}
	ptr++;
	if(ParamCount==0)	// ��ࠬ��஢ ���...
	{
		if(*(ptr=SkipSpace(ptr))!=')')	// � ᪮��� ���- �����冷�
			return ERROR;
		owner->CurrentPtr=ptr+1;	// ��� ��.
		return OK;
	}
	// ��ࠬ���� ���� - ��� ���� �� ����� �� ����
	// ��稭��� ��������� ��.
	arguments.add(new Expression(owner));
	if(arguments[0]==NULL)
	{
		arguments.flush();
		return ERROR_NOMEM;
	}
	owner->CurrentPtr=ptr;
	if((owner->result=arguments[0]->setup())!=OK)
	{
		arguments.flush(1);
		owner->CurrentPtr=BackUp;
		return owner->result;
	}
	for(i=1;i<ParamCount;i++)
	{
		// ������ ��� -
		if(*(ptr=SkipSpace(owner->CurrentPtr))!=',')
		{	// ��室��,
			if(*ptr==')' && ParamCount==UINT_MAX)
			{          // �᫨ ⠪ �뫮 ���㬠�� (����࠭�祭��� �᫮ ��ࠬ.)
				owner->CurrentPtr=ptr+1; // ��� ��.
				return OK;

			}
			owner->CurrentPtr=BackUp;		// ���� - �����-� ���ࠢ����
			arguments.flush(1);				// ��ࠬ���, �����饬� �訡��
			return ERROR;
		}
		owner->CurrentPtr=ptr+1;		// �᫨ �� ��� �� - ������塞 ��ࠦ����
		arguments.add(new Expression(owner));	// � ��� � ᫥���饬�...
		if(arguments[i]==NULL)
		{
			owner->CurrentPtr=BackUp;
			arguments.flush();
			return ERROR_NOMEM;
		}
		if((owner->result=arguments[i]->setup())!=OK)
		{
			arguments.flush(1);
			owner->CurrentPtr=BackUp;
			return owner->result;
		}
	}
	// �஢��塞 �� ���������� ᪮���
	if(*(ptr=SkipSpace(owner->CurrentPtr))!=')')
	{
		owner->CurrentPtr=BackUp;
		arguments.flush(1);
		return ERROR_BAD_IMPLEMENTATION;
	}
	owner->CurrentPtr=ptr+1;
	return OK;
}

// ��!!!!!!!!!!!!!!!!!!!!!!!!!! ��!!!!!!!!!!!!!!
// ����� �� �� - ����� �������ਨ!
// "������� �� ��� ��� �������..." (�) "������� � ��좠� ����樭��"