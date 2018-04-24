#include "t_proto.hpp"
#include <arpa/inet.h>
#include <string.h>

namespace T_TCP
{

    //  proto format:  | 4 type head | payload |
    BusiCodec::BusiCodec(int iType) :m_iCodeType(iType)
    {
    }

    BusiCodec::~BusiCodec()
    {
    }

    enum CODERET BusiCodec::DeCode(const char* pDstBuf, int iSrcLen, char* pSrcPayLoad, int *piPayLoadLen)
    {
        if (iSrcLen < sizeof(int))
        {
            *piPayLoadLen = 0;
            return RET_RECV_NOT_COMPLETE;
        }

        int iHeadBufData = ::ntohl( *((int*)pDstBuf) );
        if (iSrcLen >= iHeadBufData + sizeof(int))
        {
            ::memcpy(pSrcPayLoad, pDstBuf + sizeof(int), iHeadBufData);
            *piPayLoadLen =  iHeadBufData + sizeof(int);
            return RET_RECV_OK;
        }
        return RET_RECV_NOT_COMPLETE;
    }

    enum CODERET BusiCodec::EnCode(const char* pSrcBuf, int iSrcLen, char *pDstMsg, int* piDstMsgLen)
    {
        if (pSrcBuf == NULL || pDstMsg == NULL || piDstMsgLen == NULL)
        {
            return RET_RECV_ERR;
        }
        char *pPreCodeAddr = pDstMsg + (*piDstMsgLen);
        
        char buf[1024];
        ::memset(buf, 0, sizeof(buf));
        //set head content;
        *((int*)buf) = ::htonl(iSrcLen);
        //set body content
        memcpy(buf + sizeof(int), pSrcBuf, iSrcLen);
        //set conn send buf content.
        memcpy(pPreCodeAddr, buf,  iSrcLen + sizeof(int));
        (*piDstMsgLen) += iSrcLen + sizeof(int);
        return RET_RECV_OK;
    }
}
