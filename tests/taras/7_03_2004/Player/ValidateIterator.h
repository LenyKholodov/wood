#ifndef __ValidateIterator_h__
#define __ValidateIterator_h__

#include <iterator>

class iterator;
bool ValidateIterator(iterator iter)
{
	try	{
		if(*iter!=NULL)
			return true;
	}
	catch
	{
		return false;
	}
}

#endif