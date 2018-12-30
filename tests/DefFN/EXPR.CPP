// Реализация всякой гадости, описанной в "expr.h"
#include "expr.h"

// см. описание
char* SkipSpace(char* s)
{
 while(isspace(*s) && *s)
  s++;
 return s;
}

// см. описание
char* lexema(char* s,size_t* l)
{
 if(*(s=SkipSpace(s))=='\0' || (!isalpha(*s) && *s!='_'))
  *l=0;
 else
  for(*l=1;s[*l]=='_' || isalpha(s[*l]) || isdigit(s[*l]);(*l)++);
 return s;
}

// поиск имени функции в строке
int DefFn::SetupName(void)
{
 FnName="";
 size_t l;
 CurrentPtr=lexema(CurrentPtr,&l);	// выдёргиваем лексему...
 if(l)	// она есть?
 {
  char tmp=CurrentPtr[l];	// да - сохраняем имя
  CurrentPtr[l]='\0';
  FnName=CurrentPtr;
  CurrentPtr[l]=tmp;
  CurrentPtr+=l;    		// перемещаемся дальше по буферу.
 }
 return OK;
}

// печать данных функции
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

// установка параметров
int DefFn::SetupParameters(void)
{
	parameters.flush(1);
	// первый символ - открывающая скобка, если нет,
	// то считаем, что параметров нет.
	if(*(CurrentPtr=SkipSpace(CurrentPtr))!='(')
		return OK;

	size_t l;
	String* s;
	// ищем лексемы...
	for(CurrentPtr=lexema(++CurrentPtr,&l);l;)
	{
		char tmp=CurrentPtr[l];
		CurrentPtr[l]='\0';
		s=new String(CurrentPtr); // записывем имя...
		CurrentPtr[l]=tmp;
		if(s==NULL)
		{
			parameters.flush(1);
			return ERROR_NOMEM;
		}
		// если такое имя уже есть...
		if(parameters.find(s)!=UINT_MAX)
		{
			parameters.flush(1);
			delete s;
			// матюкаемся и выходим
			return ERROR_MULTIPLIE_DECLARATION;
		}
		// иначе добавляем имячко
		parameters.add(s);
		// смотрим что после имени -
		switch(*(CurrentPtr=SkipSpace(CurrentPtr+=l)))
		{
			// скобка - ок, конец работы
			case ')': CurrentPtr++; return OK;
			// следующий параметр - тоже ок.
			case ';':
			case ',': CurrentPtr=lexema(++CurrentPtr,&l); break;
			// Иначе - плохое описание
			default:  parameters.flush(1); return ERROR_BAD_DECLARATION;
		}
	}
	// смотрим, как наработались.
	switch(*SkipSpace(CurrentPtr))
	{
		// остановились не на скобке - не закрыто описание параметров
		case 0: 	return ERROR_OPENED_PARAMETERS_STRING;
		// всё ок
		case ')':	CurrentPtr++; return OK;
		// пропуск параметра
		case ';':
		case ',':	return ERROR_BAD_DECLARATION;
		// неизвестно что, но неправильно
		default:	return ERROR;
	}
}

// читаем название и параметры функции...
// примечание: допустимыми считаются следующие варианты:
// f(x)=1+3+...
// f=1+sin(pi)*...
// =45+10
// и даже(пустое имя) (x,y)=12*x+y+...
int DefFn::SetupDeclare(void)
{
	SetupName();					// читаем название
	int result=SetupParameters();   // и параметры,
	if(result!=OK)					// если неправильно - выходим и сообшаем
		return result;
	CurrentPtr=SkipSpace(CurrentPtr);
	if(*CurrentPtr!='=')            //ищем начало реализации,если нет, выход
		return ERROR_BAD_DECLARATION;
	CurrentPtr++;					// иначе всё ок, переходим на выражение
	return OK;
}

// возможные ошибки
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
// инициализация объекта
int DefFn::setup(char* s)
{
	clear();			// чистим всё
	CurrentPtr=s;       // начало интертрепации - начало строки
	if((result=SetupDeclare())!=OK)	// если облом на декларации - выход
		return result;
	if((result=expression.setup())!=OK)	// если облом на выражении - выход
		return result;
	// если после анализа выражения остались "хвосты" - выходим
	if(*(CurrentPtr=SkipSpace(CurrentPtr))!='\0')
		return ERROR_LINE_TOO_LONG;
	return OK;		// но проблем!
}

// возвращает значение данного экземпляра выражения
double Expression::value(void) const
{
	// выражение состоит из суммы термов,
	// поэтому для вычисления - просто суммируем их
	double v=0.0;
	for(int i=0;i<terms.top();v+=terms[i++]->value());
	return v;
}

// возвращает значение терма.
double Term::value(void) const
{
//	Терм состоит из произведения факторов, поэтому
	double v=signum?-1.0:1.0;			// учитываем знак терма
	for(int i=0;i<factors.top();i++)	// перебираем все факторы
		switch(factors[i]->mode)		// проверяем режим умножения
		{								// и умножаем
			case 0:	v*=factors[i]->value(); break;
			case 1: v/=factors[i]->value(); break;
			case 2: v=floor(v/factors[i]->value()); break;
			case 3: v=fmod(v,factors[i]->value()); break;
		}
	return v;
}

// инициализация выражения
int Expression::setup(void)
{
	terms.flush(1);			   				// чистим усё
	char* &CurrentPtr=owner->CurrentPtr;    // получаем ссылку на указатель
									// позиции интертрепации строки текста
	if(*(CurrentPtr=SkipSpace(CurrentPtr))=='\0')// интрепретировать нечего,
		return ERROR_LINE_TOO_SHORT;// обижаемся и выходим
	// т.к. выражение - это сумма термов, остаётся следить только за знаками
	// "+/-", а остальную работу кидать на интрепретацию терму
	for(Term* t;;)
	{
		if((t=new Term(owner))==NULL)		// вах! нет памяти
		{
			terms.flush(1);					// чистим, выходим
			return ERROR_NOMEM;
		}
		terms.add(t);						// добавляем терм
		if((owner->result=t->setup())!=OK)	// мудрёно и не доделано
		{
			terms.flush(1);
			return	owner->result;
		}
		// сверяем знаки...
		switch(*(CurrentPtr=SkipSpace(CurrentPtr)))
		{
			case '-':
			case '+':	break;		// наши значки, продолжаем
			default:	return OK;	// нет - выходим, считая, что выражение
									// кончилось
		}
	}
}

// инициализация терма
int Term::setup(void)
{
	factors.flush(1); 	// чистимся,
	int mode=signum=0;	// устанавливаем режим
	char* &CurrentPtr=owner->CurrentPtr;
	// смотрим знак, по умолчанию - знак плюс
	switch(*(CurrentPtr=SkipSpace(CurrentPtr)))
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;
		case '+':	CurrentPtr++;	break;
	}
	// т.к. терм - это произведение факторов, остаётся следить только за знаками
	// "*/\%", а остальную работу кидать на интрепретацию фактору
	// маленькая поправка: фактор - это:
	// число, функция, переменная или выражение в скобках
	// поэтому делается подряд 4 попытки понять, что это такое
	for(Factor* t;;)
	{
		// ми думаэм, чито это число...
		if((t=new Number(owner))==NULL)
		{
			factors.flush(1);
			return ERROR_NOMEM;
		}
		if((owner->result=t->setup())!=OK)
		{	// нэт - думэм, это переменный...
			delete t;
			if((t=new Variable(owner))==NULL)
			{
				factors.flush(1);
				return ERROR_NOMEM;
			}
			if((owner->result=t->setup())!=OK)
			{
				// нэт - тогды это выражений в скобке...
				delete t;
				if((t=new SubExpression(owner))==NULL)
				{
					factors.flush(1);
					return ERROR_NOMEM;
				}
				if((owner->result=t->setup())!=OK)
				{
					// снова нэт - это функцый!
					delete t;
					if((t=new MathFunction(owner))==NULL)
					{
						factors.flush(1);
						return ERROR_NOMEM;
					}
					if((owner->result=t->setup())!=OK)
					{
						// ну незнаю, что это такое, пошли все на #$%...
						factors.flush(1);
						return	owner->result;
					}
				}
			}
		}
		// это кто-то!
		t->mode=mode; 		// запоминаем текущий знак,
		factors.add(t);		// добавляем к списку
		switch(*(CurrentPtr=SkipSpace(CurrentPtr)))// и смотрим следующий знак
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

// инициализация числа...
int Number::setup(void)
{
	char* ptr;
	data=strtod(owner->CurrentPtr,&ptr);
	if(ptr!=owner->CurrentPtr)
	{
		owner->CurrentPtr=ptr;	// ок - это число, переносим
		// указатель интертепации за него, и едем дальше
		return OK;
	}
	// иначе - фокус не прошел
	return ERROR_BAD_IMPLEMENTATION;
}

// инициализация переменной
int Variable::setup(void)
{
	size_t l;
	// выдираем слово...
	char* ptr=lexema(owner->CurrentPtr,&l);
	if(l)	// если слово есть,
	{
		char tmp=ptr[l];
		ptr[l]='\0';
		// ищем в списке переменных...
		index=owner->parameters.find(&String(ptr));
		ptr[l]=tmp;
		// такая переменная есть ?
		if(index!=UINT_MAX)
		{
			// да - всё ок!
			owner->CurrentPtr+=l;
			return OK;
		}
	}
	// иначе нет, ошибка
	index=0;
	return ERROR_BAD_IMPLEMENTATION;
}

// тут всё просто - мы запомнили индекс в списке параметров,
// нужно его взять и по нему извлечь значение.
double Variable::value(void) const
{
	return owner->ParamValue[index];
}

// подвыражение. Это обычное выражение, но в скобках
int SubExpression::setup(void)
{
	char *BackUp, *ptr=SkipSpace(BackUp=owner->CurrentPtr);
	int result;
	// смотрим знак,
	switch(*ptr)
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;ptr=SkipSpace(++ptr);break;
		case '+':	signum=0;ptr=SkipSpace(++ptr);break;
	}
	if(*ptr!='(')	// если не в скобках, то это не выражение в скобках
					// логично, да ??!
		return ERROR_BAD_IMPLEMENTATION;
	owner->CurrentPtr=SkipSpace(++ptr);
	// пытаемся интрепретировать выражение внутри скобок...
	if((result=expression.setup())!=OK)
	{
		owner->CurrentPtr=BackUp;
		expression.clear();
		return result;
	}
	// ищем заверщающую скобку...
	if(*(ptr=SkipSpace(owner->CurrentPtr))!=')')
	{
		owner->CurrentPtr=BackUp;	// вах! нету
		expression.clear();
		return ERROR_BAD_IMPLEMENTATION;	// плохо
	}
	owner->CurrentPtr=ptr+1;	// всё ок, перемещаем указатель строки
	return OK;
}

// функция, значение
double MathFunction::value(void) const
{
	double v,d;
	if(arguments.top()>0)
		d=arguments[0]->value();
	switch(IdFn)
	{
		// функции максимума и минимимума - кол-во аргументов не ограничено
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
		// дальше наверное, понятно
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
	return signum?-v:v;	// учитываем знак
}

// инициализация функции
int MathFunction::setup(void)
{
	char *BackUp, *ptr=SkipSpace(BackUp=owner->CurrentPtr),tmp;
	size_t l;
	int result;
	unsigned int i,ParamCount;
	// узнаём знак...
	switch(*ptr)
	{
		case '\0':	return ERROR_LINE_TOO_SHORT;
		case '-':	signum=1;ptr=SkipSpace(++ptr);break;
		case '+':	signum=0;ptr=SkipSpace(++ptr);break;
	}
	// читаем название
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
	// не нашли - нет такой функции, выходим
	if(IdFn==UINT_MAX)
		return ERROR_FUNCTION_NOT_DEFINED;
	*(ptr+=l)=tmp;
	// скобка есть?
	if(*(ptr=SkipSpace(ptr))!='(')
	{              // нету,
		if(ParamCount)	// если у функции должны быть параметры, выходим
			return ERROR_LINE_TOO_SHORT;
		owner->CurrentPtr=ptr;	// иначе - переживём такое горе
		return OK;
	}
	ptr++;
	if(ParamCount==0)	// параметров нет...
	{
		if(*(ptr=SkipSpace(ptr))!=')')	// и скобки нет- непорядок
			return ERROR;
		owner->CurrentPtr=ptr+1;	// всё ок.
		return OK;
	}
	// параметры есть - они просто не могут не есть
	// начинаем добавлять их.
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
		// запятая нет -
		if(*(ptr=SkipSpace(owner->CurrentPtr))!=',')
		{	// выходим,
			if(*ptr==')' && ParamCount==UINT_MAX)
			{          // если так было задумано (неограниченное число парам.)
				owner->CurrentPtr=ptr+1; // всё ок.
				return OK;

			}
			owner->CurrentPtr=BackUp;		// иначе - какой-то неправильный
			arguments.flush(1);				// параметр, возвращема ошибку
			return ERROR;
		}
		owner->CurrentPtr=ptr+1;		// если же всё ок - добавляем выражение
		arguments.add(new Expression(owner));	// и идём к следующему...
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
	// проверяем на завершающую скобку
	if(*(ptr=SkipSpace(owner->CurrentPtr))!=')')
	{
		owner->CurrentPtr=BackUp;
		arguments.flush(1);
		return ERROR_BAD_IMPLEMENTATION;
	}
	owner->CurrentPtr=ptr+1;
	return OK;
}

// Ура!!!!!!!!!!!!!!!!!!!!!!!!!! все!!!!!!!!!!!!!!
// Какой это пц - писать комментарии!
// "Никогда не пей эту гадость..." (с) "Человек с бульвара капуцинов"