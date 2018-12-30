#include <pch.h>
#include <binfile\SocketStream.h>

size_t  socketBinFile::_read    (void* buf,size_t size)
{
  int res = recv (mSocket,buf,size,mFlags);

  return res < 0 ? 0 : res;
}

size_t  socketBinFile::_write   (const void* buf,size_t size)
{
  int res = send (mSocket,buf,size,mFlags);

  return res < 0 ? 0 : res;  
}

bool    socketBinFile::init     (ulong addr,ulong port,int mode,int domain,int type,int protocol)
{
  sockaddr_t peer;

  peer.sin_family      = domain;
  peer.sin_port        = htone (port);
  peer.sin_addr.s_addr = addr;

  mSocket              = socket (domain,type,protocol);

  if (mSocket < 0)
    return false;

  mConnect             = connect (mSocket,&peer,sizeof (sockaddr_t));

  if (!mConnect)
    return false;

  return true;
}

socketBinFile (const char* inet_addr,ulong port,int mode,int domain,int type,int protocol)
{
}

socketBinFile (ulong inet_addr,ulong port,int mode,int domain,int type,int protocol)
{
}

socketBinFile::~socketBinFile ()
{
}