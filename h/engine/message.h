#ifndef __ENGINE_MESSAGE__
#define __ENGINE_MESSAGE__

#include <types.h>

/*
  �����䨪��� ᮮ�饭��
*/

typedef  long msg_t;

#pragma pack(push,1)

////////////////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� �������� ᮮ�饭��
////////////////////////////////////////////////////////////////////////////////////////////
struct    message_t
{
  size_t        size;       //������ ����� ᮮ�饭��
  msg_t         msg;        //�����䨪��� ᮮ�饭�� 
};

#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////////////////
///�᭮��� ��஢� ᮮ�饭��
///////////////////////////////////////////////////////////////////////////////////////////
enum GameMsg
{
  MSG_GLOBAL_SERVER = 0,
  MSG_LOCAL_SERVER  = 0x1000,
  MSG_USER          = 80000000
};

#endif