/********************************************************************
* ����, � ���஬ ��室���� ��楤�� ����.                	        *
* �����᪨ ᤥ�� ��室���� ⮫쪮 �஢�ઠ ����� �㭪権         *
********************************************************************/
#include "expr.h"			// ��������� 䠩�� ���ᠭ�� ����� �㭪権
#include<alloc.h>

void main(void)
{
 char buffer[200];
 DefFn fn;          		// ᮧ��� ���������稪
 cout<<"Mem status:"<<coreleft()<<endl;	// �㦨� ��� �஢�ન ���४⭮��
										// �᢮�������� ����� ��᫥ ࠡ���
 for(;;)
 {
  cout<<endl<<"Enter string:"<<endl;
  cin>>buffer;
  if(!*buffer)		// �᫨ ������ ���...
   break;			// ��室�� �� �ண�
  int result=fn.setup(buffer);		// ���樠�����㥬 �㭪�� � �஢��塞
  cout<<"Mem status:"<<coreleft()<<endl;
  cout<<fn.errorstr(result)<<endl;	// �᫨ �訡�� ��� - �뢥����� ��
  if(result==OK)				// �᫨ �訡�� ��� - ࠡ�⠥�
  {
	double p[200];				// �������� ��ࠬ���� �㭪樨
	for(int i=0;i<fn.ParamCount();i++)	// ��ॡ�ࠥ� �� �� � ������
	{
		cout<<fn.ParamItem(i)<<"=";
		cin>>p[i];
	}
	cout<<fn<<endl				// �뢮��� �������� �㭪樨
	<<"Value is:"<<fn(p);		// � ���祭��.
  }
  cout<<endl<<"Stopped at: "<<fn.CurrentPtr<<endl;// ������ ��⠭����-
											// � ��砥 ᡮ� �� ����� ��ப�.
 }
 cout<<"Mem status:"<<coreleft()<<endl; // �㦨� ��� �஢�ન ���४⭮��
										// �᢮�������� ����� ��᫥ ࠡ���
}