/*
                Файловая подсистема

   Основная цель - автоматизировать работу как с дисковыми файлами, так и с pak-файлами.
   Для этого введена система "файловых групп" - коллекций pak-файлов. 
   
   Открытие файла:
                BinFile* file = F_OpenFile ("group:\\anydir\\anyfile.any",BF_OPEN_RO);

      1) Поиск файловой группы group
      2) В случае, есшли группа найдена открытие файла происходит из этой группы
      3) Иначе происходит поиск в таблице alias'ов:
      4) Если поиск успешен - файл возвращается
      5) Иначе открытие файла из группы default

      Если группа явно не указана:
           BinFile* file = F_OpenFile ("anydir\\anyfile.any",BF_OPEN_RO);

           файл открывается из группы default
   
   Файловые группы:
      F_RegisterGroup / F_UnregisterGroup / F_RegisterPackFile

      Псевдо-имена  для файлов объединённых в группы. Для подробного "зачем" звоните DadyCool.

      В группу добавляются файлы (открытие начинается с последнего добавленного, кроме default)
     
   Alias'ы:
      Система предоставление имени файлам (фактически предоставление видимости).

      F_AddAlias (фтн ышьищды - any_path_tha_you_like/anyfilename.any - any simbols");

      Данное открытие не имеет формат - возможны любые имена. 

   Подробное описание методов смотри в service\file.h
   
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
  Загружает службу поддержки файлов  
    _Pool - пул для выделения памяти под файловый пул
    _Size - размер файлового пула
*/

FileService*     F_Load   (Pool* _Pool,size_t _Size = 0);

/*
  Выгружает службу поддержки файлов
*/

void             F_Done             (FileService* = fileService);

/*
  Открывает файл
       1-я перегрузка

           _FileName  - имя файла
           _Mode      - режим доступа к файлу

    имя файла представляет собой как путь к физическому файлу, так и к логически
    определённому и принадлежащему "группе файлов", определённой посредством
    F_RegisterGroup (), F_RegisterPackFile ()

       2-я перегрузка

           _GroupName - имя файла
           _FileName  - имя файла
           _Mode      - режим доступа к файлу

    аналогично первому случаю, только имя группы указывается явно
*/

#define F_OpenFile      fileService->OpenFile

/*
  Закрывает файл
    _File - файл

    эквивалентен delete file
*/


#define F_CloseFile     fileService->CloseFile

/*
  Регистрирует "файловую группу"
    _GroupName - имя "группы файлов"    
    _File      - файл, который будет добавлен в группу, 
                 если _File = NULL группа будет создана, _File != NULL - файл
                 будет добавлен в существующую группу, при отсутствии которой
                 последняя будет создана    
*/


#define F_Register      fileService->RegisterGroup

/*
  Удаляет "файловую группу"
    _GroupName - имя "группы файлов"
    _File      - файл, который будет удалён из группы
                 NULL  - удаляется группа
                 !NULL - файл исключается из группы
*/

#define F_Unregister    fileService->UnregisterGroup

/*
  Добавление pak'а для обработки по умолчанию  
*/

#define F_AddDefault    fileService->AddDefault

/*
  Удаление pak'а для обработки по умолчанию  
*/

#define F_DelDefault    fileService->DelDefault

/*
  Открывает PackFile
    _PackName - имя PackFile'а

    имя PackFile'а должно представлять собой путь к физическому файлу,
    префиксы, имена групп на имя PackFile'а не распрастраняется
*/

#define F_OpenPackFile  fileService->OpenPackFile

/*
  Открывает PackFile и регистрирует группу, с добавлением PackFile'а
    _PackName  - имя PackFile'а
    _GroupName - имя группы 
*/

#define F_RegisterPackFile fileService->RegisterPackFile

/*
  Добавляет псевдоним для файла
*/

#define F_AddAlias      fileService->AddAlias


/*
  Удаляет псевдоним для файла
*/

#define F_DelAlias      fileService->DelAlias

/*
  Открытие поиска файла
*/

#define F_FirstFile     fileService->FirstFile

/*
  Закрытие поиска файла
*/

#define F_CloseSearch   fileService->CloseSearch

/*
  Следующий файл
*/

#define F_NextFile      fileService->NextFile

#define F_AddFastPath   fileService->AddFastPath
#define F_DelFastPath   fileService->DelFastPath
#define F_SetCurrentDir fileService->SetCurrentDir


#endif
