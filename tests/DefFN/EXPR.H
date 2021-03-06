/***************************************************************************
*       														           *
*	���ᠭ�� ��� ����ᮢ � �㭪権, �ᯮ��㥬�� � �����९��� �㭪権 *
*       														           *
***************************************************************************/
#ifndef __EXPRESSION_LIB_DECLARE__
#define __EXPRESSION_LIB_DECLARE__

#include<vectimp.h>
#include<strng.h>
#include<ctype.h>
#include<stdlib.h>
#include<math.h>

// �ய�� ���⮣� ����. �����頥� ������ ��ࢮ�� ���஡���
char* SkipSpace(char*);

// �����頥� 㪠��⥫� �� ��砫� ���ᥬ�(᫮�� ��� �᫠),
// �� ��뫪� �����頥� �� ࠧ���
char* lexema(char*,size_t*);

// �������� �訡�� �� �믮������.
 enum {	OK=0,
	ERROR_NOMEM,
	ERROR_OPENED_PARAMETERS_STRING,
	ERROR_MULTIPLIE_DECLARATION,
	ERROR_BAD_DECLARATION,
	ERROR_LINE_TOO_LONG,
	ERROR_LINE_TOO_SHORT,
	ERROR_BAD_IMPLEMENTATION,
	ERROR_FUNCTION_NOT_DEFINED,
	ERROR};

// �� �㦭� ⮫쪮 ����娨 ���ᮢ ��ૠ��� - ��� ����� � ����娨
const 	DefFNClass		=__firstUserClass,
	ProtoExpressionClass	=DefFNClass+1,
	ExpressionClass		=ProtoExpressionClass+1,
	TermClass		=ExpressionClass+1,
	FactorClass		=TermClass+1,
	NumberClass		=FactorClass+1,
	VariableClass		=NumberClass+1,
	SubExpressionClass	=VariableClass+1,
	MathFunctionClass	=SubExpressionClass+1;

// ������ �।���ᠭ�� ����ᮢ... ��� �� 㢥७, ��䨣 ����, �� �뤠�� �����
// ��୨����, � ���� �� ��� ���ᠭ�� ��. ������⥪...
// ���ࠫ �� ��直� ��砩. :-)
_CLASSDEF(DefFn);
_CLASSDEF(ProtoExpression);
_CLASSDEF(Expression);
_CLASSDEF(Term);
_CLASSDEF(Factor);
_CLASSDEF(Number);
_CLASSDEF(Variable);
_CLASSDEF(SubExpression);
_CLASSDEF(MathFunction);

// �����-�।�� ��� ��ࠦ����- ᠬ ��祣� �� ������
// ��� �।�� - ������, �᫨ ⠪ �� ᤥ����, ���⥩���� �⪠����� �࠭���
// ����������.
class ProtoExpression: public Object
{
protected:
	DefFn	*owner;		// �������� ��������� - �㭪��
						// �ᯮ������ ��� ࠡ��� ��ࠬ��ࠬ� �㭪樨.
	int	signum;			// ���� ��ࠦ���� - "+/-" 0 -"+", 1-"-"
public:
	// ����᫥���騥 ��楤��� �㦭� ��� ���४⮭�� ࠡ��� ����娨 � �।��
	// �⮣� �����:
	// ��� �����
	virtual classType isA(void) const { return ProtoExpressionClass; };
	// ��� �����
	virtual char _FAR *nameOf(void) const { return "ProtoExpression"; };
	// ��� ���஢��
	virtual hashValueType hashValue(void) const { return 0; };
	// �஢�ઠ �� ࠢ���⢮
	virtual int isEqual( const Object _FAR & ) const { return 0; };
	// �ᯥ�⪠ ��ꥪ�
	virtual void printOn(ostream _FAR&) const {};
	// ��⠭���� ��ࠬ��஢
	virtual int setup(void) { return OK; };
	// ���祭��
	virtual double value(void) const { return 0; };
	ProtoExpression(DefFn* ptr):signum(0) { owner=ptr; }
	virtual ~ProtoExpression(void) {};
};

// ������� �ந��������
class Factor: public ProtoExpression
{
protected:
public:
	int mode;		// ०�� - 㬭������/�������/������� ��楫�/���⮪

	virtual classType isA(void) const { return FactorClass; };
	virtual char _FAR *nameOf(void) const { return "Factor"; };

	Factor(DefFn* ptr):ProtoExpression(ptr),mode(0) {};

	virtual int setup(void)	{ return OK; };
	virtual double value(void) const { return 0; };
	virtual ~Factor(void) {};
};

// ������� ����⠭� - ���� �࠭�� ���� �᫮
class Number: public Factor
{
protected:
	double	data;
public:
	virtual classType isA(void) const { return NumberClass; };
	virtual char _FAR *nameOf(void) const { return "Number"; };

	Number(DefFn* ptr):Factor(ptr) {};

	virtual int setup(void);
	virtual double value(void) const { return data; };
};

// ������� ��६�����
class Variable: public Factor
{
protected:
	unsigned int index;
public:
	virtual classType isA(void) const { return VariableClass; };
	virtual char _FAR *nameOf(void) const { return "Variable"; };

	Variable(DefFn* ptr):Factor(ptr) {};

	virtual int setup(void);
	virtual double value(void) const;
};

// ������� ��ࠦ���� - �ந��������
class Term: public ProtoExpression
{
protected:
	BI_ICVectorImp <Factor> factors;//�࠭�� ����� 䠪�஢- ������⮢ �ந��������
public:
	virtual classType isA(void) const { return TermClass; };
	virtual char _FAR *nameOf(void) const { return "Term"; };

	Term(DefFn* ptr):ProtoExpression(ptr),factors(0,2) {};
	virtual int setup(void);
	virtual double value(void) const;
	virtual ~Term(void) { factors.flush(1); };// ��⨬ ���ᨢ 䠪�஢
};

// ��ࠦ���� - ᮤ�ন� �㬬� (�����) �ந��������
class Expression: public ProtoExpression
{
protected:
	BI_ICVectorImp<Term> terms;
public:
	virtual classType isA(void) const { return ExpressionClass; };
	virtual char _FAR *nameOf(void) const { return "Expression"; };

	Expression(DefFn* ptr):ProtoExpression(ptr),terms(0,2) {};
	virtual int setup(void);
	virtual void printOn(ostream _FAR& os) const { os<<"terms count:"<<terms.top()<<endl;};
	virtual double value(void) const;
	virtual ~Expression(void) { clear(); }
	void clear(void) { terms.flush(1); }
};

// ���ᠭ�� ���஥���� �㭪権
typedef struct
{
 char* name;			// ��� �㭪樨
 unsigned int ArgCount; // ���-�� ��㬥�⮢, �᫨ UINT_MAX, � ��
} MathFunctionStruct;

const int		 MathFunctionsCount=25;
const MathFunctionStruct MathFunctions[]={
	{	"min",	UINT_MAX},
	{	"max",	UINT_MAX},
	{	"abs",	1	},
	{	"sqr",	1	},
	{	"sqrt",	1	},
	{	"acos",	1	},
	{	"asin",	1	},
	{	"atan",	1	},
	{	"atan2",2	},
	{	"pi",	0	},
	{	"cos",	1	},
	{	"sin",	1	},
	{	"tan",	1	},
	{	"cosh",	1	},
	{	"sinh",	1	},
	{	"tanh",	1	},
	{	"exp",	1	},
	{	"hypot",2	},
	{	"ln",	1	},
	{	"log",	2	},
	{	"log10",1	},
	{	"pow",	2	},
	{	"sign",	1	},
	{	"int",	1	},
	{	"round",1	},
};

// ����� �����প� ���஥��� �㭪権.
class MathFunction: public Factor
{
protected:
	BI_ICVectorImp<Expression> arguments;// ����� ��ࠦ����, ��⠢����� ��㬥���
	unsigned int IdFn;		// ��� �㭪樨 (�. ���ᨢ ���)
public:
	virtual classType isA(void) const { return MathFunctionClass; };
	virtual char _FAR *nameOf(void) const { return "MathFunction"; };
	MathFunction(DefFn* ptr):Factor(ptr),arguments(0,2),IdFn(UINT_MAX) {};
	virtual int setup(void);
	virtual double value(void) const;
	virtual ~MathFunction(void) { arguments.flush(1); }
};

//��ࠦ���� � ᪮����
class SubExpression: public Factor
{
protected:
	Expression	expression;
public:
	virtual classType isA(void) const { return SubExpressionClass; };
	virtual char _FAR *nameOf(void) const { return "SubExpression"; };
	SubExpression(DefFn* ptr):Factor(ptr),expression(ptr) {};

	virtual int setup(void);
	virtual double value(void) const
		{ return signum?-expression.value():expression.value(); };

	virtual ~SubExpression(void) { expression.clear(); };
};

// �������-�! ������ �����! ���ᠭ�� �㭪樨.
class DefFn:public Object
{
protected:
	BI_ICVectorImp<String> parameters;	// ᮤ�ন� ᯨ᮪(�����) ��㬥�⮢
	String FnName;						// �������� �㭪樨
	Expression expression;				// ����塞�� ��ࠦ����

	inline int SetupName(void);
	int SetupParameters(void);
	inline int SetupDeclare(void);

public:
	virtual classType isA(void) const { return DefFNClass; };
	virtual char _FAR *nameOf(void) const { return "DefFn"; };
	virtual hashValueType hashValue(void) const { return 0; };
	virtual int isEqual( const Object _FAR & ) const { return 0; };
	virtual void printOn( ostream _FAR& ) const;

	char *CurrentPtr;
	int result;
	double* ParamValue;

	inline DefFn(void): expression(this),parameters(0,5),FnName("")
			{ CurrentPtr=NULL; }

	inline String name(void) const { return FnName; }

	inline int ParamCount(void) const { return parameters.top(); }

	inline String ParamItem(int i) const
		{
			if(i>=0 && i<parameters.top())
				return *parameters[i];
			else
				return String("");
		}

	inline void clear(void)
		{ FnName=""; CurrentPtr=NULL; parameters.flush(1); }

	const char* errorstr(int) const;

	virtual ~DefFn(void) { clear(); }

	int setup(char*);

	double operator()(double* data)
		{ ParamValue=data; return expression.value(); }
// ����� Variable ����� ��㣮�, �.�. ��� �㦭� ��ਮ���᪨ ������ � ᯨ᮪
// ��।������� ��ࠬ��஢
friend		Variable;
};

#endif

// EOF
// ���쪨� �����