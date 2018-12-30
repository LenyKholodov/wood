/***************************************************************************
*       														           *
*	Описание всех классов и функций, используемых в интертрепаторе функций *
*       														           *
***************************************************************************/
#ifndef __EXPRESSION_LIB_DECLARE__
#define __EXPRESSION_LIB_DECLARE__

#include<vectimp.h>
#include<strng.h>
#include<ctype.h>
#include<stdlib.h>
#include<math.h>

// пропуск пустого места. возвращает позицию первого непробела
char* SkipSpace(char*);

// возвращает указатель на начало лексемы(слова или числа),
// по ссылке возвращает её размер
char* lexema(char*,size_t*);

// возможные ошибки при выполнении.
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

// это нужно только иерархии класов борланда - код класса в иерархии
const 	DefFNClass		=__firstUserClass,
	ProtoExpressionClass	=DefFNClass+1,
	ExpressionClass		=ProtoExpressionClass+1,
	TermClass		=ExpressionClass+1,
	FactorClass		=TermClass+1,
	NumberClass		=FactorClass+1,
	VariableClass		=NumberClass+1,
	SubExpressionClass	=VariableClass+1,
	MathFunctionClass	=SubExpressionClass+1;

// Кажись предописание классов... Сам не уверен, нафиг надо, но выдаёт меньше
// варнингов, и висит во всех описания ст. библиотек...
// Содрал на всякий случай. :-)
_CLASSDEF(DefFn);
_CLASSDEF(ProtoExpression);
_CLASSDEF(Expression);
_CLASSDEF(Term);
_CLASSDEF(Factor);
_CLASSDEF(Number);
_CLASSDEF(Variable);
_CLASSDEF(SubExpression);
_CLASSDEF(MathFunction);

// Класс-предок для выражений- сам ничего не делает
// его предок - обжект, если так не сделать, контейнеры откажутся хранить
// экземпляры.
class ProtoExpression: public Object
{
protected:
	DefFn	*owner;		// владелец экземпляра - функция
						// используется для работы параметрами функции.
	int	signum;			// знак выражения - "+/-" 0 -"+", 1-"-"
public:
	// нижеследующие процедуры нужны для корректоной работы иерархии и предка
	// этого класса:
	// код класса
	virtual classType isA(void) const { return ProtoExpressionClass; };
	// имя класса
	virtual char _FAR *nameOf(void) const { return "ProtoExpression"; };
	// код сортировки
	virtual hashValueType hashValue(void) const { return 0; };
	// проверка на равенство
	virtual int isEqual( const Object _FAR & ) const { return 0; };
	// распечатка объекта
	virtual void printOn(ostream _FAR&) const {};
	// установка параметров
	virtual int setup(void) { return OK; };
	// значение
	virtual double value(void) const { return 0; };
	ProtoExpression(DefFn* ptr):signum(0) { owner=ptr; }
	virtual ~ProtoExpression(void) {};
};

// елемент произведения
class Factor: public ProtoExpression
{
protected:
public:
	int mode;		// режим - умножение/деление/деление нацело/остаток

	virtual classType isA(void) const { return FactorClass; };
	virtual char _FAR *nameOf(void) const { return "Factor"; };

	Factor(DefFn* ptr):ProtoExpression(ptr),mode(0) {};

	virtual int setup(void)	{ return OK; };
	virtual double value(void) const { return 0; };
	virtual ~Factor(void) {};
};

// елемент константа - просто хранит одно число
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

// елемент переменная
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

// элемент выражения - произведение
class Term: public ProtoExpression
{
protected:
	BI_ICVectorImp <Factor> factors;//хранит набор факторов- элементов произведения
public:
	virtual classType isA(void) const { return TermClass; };
	virtual char _FAR *nameOf(void) const { return "Term"; };

	Term(DefFn* ptr):ProtoExpression(ptr),factors(0,2) {};
	virtual int setup(void);
	virtual double value(void) const;
	virtual ~Term(void) { factors.flush(1); };// чистим массив факторов
};

// Выражение - содержит сумму (набор) произведений
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

// описание встроенных функций
typedef struct
{
 char* name;			// имя функции
 unsigned int ArgCount; // кол-во аргументов, если UINT_MAX, то любое
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

// класс поддержки встроеных функций.
class MathFunction: public Factor
{
protected:
	BI_ICVectorImp<Expression> arguments;// набор выражений, составляющих аргументы
	unsigned int IdFn;		// код функции (см. массив выше)
public:
	virtual classType isA(void) const { return MathFunctionClass; };
	virtual char _FAR *nameOf(void) const { return "MathFunction"; };
	MathFunction(DefFn* ptr):Factor(ptr),arguments(0,2),IdFn(UINT_MAX) {};
	virtual int setup(void);
	virtual double value(void) const;
	virtual ~MathFunction(void) { arguments.flush(1); }
};

//выражение в скобках
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

// наконец-то! главный класс! Описание функции.
class DefFn:public Object
{
protected:
	BI_ICVectorImp<String> parameters;	// содержит список(имена) аргументов
	String FnName;						// название функции
	Expression expression;				// вычисляемое выражение

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
// класс Variable объявлен другом, т.к. ему нужно периодически лазить в список
// передаваемых параметров
friend		Variable;
};

#endif

// EOF
// Филькин Конец