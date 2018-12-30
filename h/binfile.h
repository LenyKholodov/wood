/*
                �������� �����⥬�

   �᭮���� 楫� - ��⮬�⨧�஢��� ࠡ��� ��� � ��᪮�묨 䠩����, ⠪ � � pak-䠩����.
   ��� �⮣� ������� ��⥬� "䠩����� ��㯯" - ������権 pak-䠩���. 
   
   ����⨥ 䠩��:
                BinFile* file = F_OpenFile ("group:\\anydir\\anyfile.any",BF_OPEN_RO);

      1) ���� 䠩����� ��㯯� group
      2) � ��砥, ��諨 ��㯯� ������� ����⨥ 䠩�� �ந�室�� �� �⮩ ��㯯�
      3) ���� �ந�室�� ���� � ⠡��� alias'��:
      4) �᫨ ���� �ᯥ襭 - 䠩� �����頥���
      5) ���� ����⨥ 䠩�� �� ��㯯� default

      �᫨ ��㯯� � �� 㪠����:
           BinFile* file = F_OpenFile ("anydir\\anyfile.any",BF_OPEN_RO);

           䠩� ���뢠���� �� ��㯯� default
   
   ������� ��㯯�:
      F_RegisterGroup / F_UnregisterGroup / F_RegisterPackFile

      �ᥢ��-�����  ��� 䠩��� ��ꥤ����� � ��㯯�. ��� ���஡���� "��祬" ������ DadyCool.

      � ��㯯� ����������� 䠩�� (����⨥ ��稭����� � ��᫥����� ������������, �஬� default)
     
   Alias'�:
      ���⥬� �।��⠢����� ����� 䠩��� (䠪��᪨ �।��⠢����� ��������).

      F_AddAlias (�� ����� - any_path_tha_you_like/anyfilename.any - any simbols");

      ������ ����⨥ �� ����� �ଠ� - �������� ��� �����. 

   ���஡��� ���ᠭ�� ��⮤�� ᬮ�� � service\file.h
   
*/

#include <binfile\SysFile.h>
#include <binfile\FrameFile.h>
#include <binfile\CacheFile.h>
#include <binfile\MemFile.h>

#include <binfile\ZipPack.h>
#include <binfile\FastPathPack.h>

#include <service\file.h>

#ifndef __BINFILE_FN__
#define __BINFILE_FN__

class Thread;
class zipBinFile;

extern accessptr_t<FileService>    fileService;

/*
  ����㦠�� �㦡� �����প� 䠩���  
    _Pool - �� ��� �뤥����� ����� ��� 䠩���� ��
    _Size - ࠧ��� 䠩������ �㫠
*/

FileService*     F_Load   (Pool* _Pool,size_t _Size = 0);

/*
  ���㦠�� �㦡� �����প� 䠩���
*/

void             F_Done             (FileService* = fileService);

/*
  ���뢠�� 䠩�
       1-� ��ॣ�㧪�

           _FileName  - ��� 䠩��
           _Mode      - ०�� ����㯠 � 䠩��

    ��� 䠩�� �।�⠢��� ᮡ�� ��� ���� � 䨧��᪮�� 䠩��, ⠪ � � �����᪨
    ��।��񭭮�� � �ਭ������饬� "��㯯� 䠩���", ��।��񭭮� ���।�⢮�
    F_RegisterGroup (), F_RegisterPackFile ()

       2-� ��ॣ�㧪�

           _GroupName - ��� 䠩��
           _FileName  - ��� 䠩��
           _Mode      - ०�� ����㯠 � 䠩��

    �������筮 ��ࢮ�� ����, ⮫쪮 ��� ��㯯� 㪠�뢠���� �
*/

#define F_OpenFile      fileService->OpenFile

/*
  ����뢠�� 䠩�
    _File - 䠩�

    �������⥭ delete file
*/


#define F_CloseFile     fileService->CloseFile

/*
  ���������� "䠩����� ��㯯�"
    _GroupName - ��� "��㯯� 䠩���"    
    _File      - 䠩�, ����� �㤥� �������� � ��㯯�, 
                 �᫨ _File = NULL ��㯯� �㤥� ᮧ����, _File != NULL - 䠩�
                 �㤥� �������� � ���������� ��㯯�, �� ������⢨� ���ன
                 ��᫥���� �㤥� ᮧ����    
*/


#define F_Register      fileService->RegisterGroup

/*
  ������ "䠩����� ��㯯�"
    _GroupName - ��� "��㯯� 䠩���"
    _File      - 䠩�, ����� �㤥� 㤠�� �� ��㯯�
                 NULL  - 㤠����� ��㯯�
                 !NULL - 䠩� �᪫�砥��� �� ��㯯�
*/

#define F_Unregister    fileService->UnregisterGroup

/*
  ���������� pak'� ��� ��ࠡ�⪨ �� 㬮�砭��  
*/

#define F_AddDefault    fileService->AddDefault

/*
  �������� pak'� ��� ��ࠡ�⪨ �� 㬮�砭��  
*/

#define F_DelDefault    fileService->DelDefault

/*
  ���뢠�� PackFile
    _PackName - ��� PackFile'�

    ��� PackFile'� ������ �।�⠢���� ᮡ�� ���� � 䨧��᪮�� 䠩��,
    ��䨪��, ����� ��㯯 �� ��� PackFile'� �� �����࠭����
*/

#define F_OpenPackFile  fileService->OpenPackFile

/*
  ���뢠�� PackFile � ॣ������� ��㯯�, � ����������� PackFile'�
    _PackName  - ��� PackFile'�
    _GroupName - ��� ��㯯� 
*/

#define F_RegisterPackFile fileService->RegisterPackFile

/*
  �������� �ᥢ����� ��� 䠩��
*/

#define F_AddAlias      fileService->AddAlias


/*
  ������ �ᥢ����� ��� 䠩��
*/

#define F_DelAlias      fileService->DelAlias

/*
  ����⨥ ���᪠ 䠩��
*/

#define F_FirstFile     fileService->FirstFile

/*
  �����⨥ ���᪠ 䠩��
*/

#define F_CloseSearch   fileService->CloseSearch

/*
  ������騩 䠩�
*/

#define F_NextFile      fileService->NextFile

#define F_AddFastPath   fileService->AddFastPath
#define F_DelFastPath   fileService->DelFastPath
#define F_SetCurrentDir fileService->SetCurrentDir


#endif
