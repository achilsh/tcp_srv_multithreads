/**
 * @file: t_proto.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-24
 */

#ifndef _t_proto_hpp_
#define _t_proto_hpp_

namespace T_TCP
{
    //  proto format:  | 4 type head | payload |
    enum CODERET 
    {
        RET_RECV_OK = 0,
        RET_RECV_NOT_COMPLETE = 1,
        RET_RECV_ERR = 2,
    };
    class BusiCodec
    {
     public:
      BusiCodec(int iType);
      virtual ~BusiCodec();
      enum CODERET DeCode(const char* pDstBuf, int iSrcLen, char* pSrcPayLoad, int *piPayLoadLen);

      /**
       * @brief: EnCode 
       *    将 长度为iSrcLen的pSrcBuf编码到 空间 pDstMsg, 最后总的编码空间长度存放
       *    在 piDstMsgLen.
       * @param pSrcBuf
       * @param iSrcLen
       * @param pDstMsg
       * @param piDstMsgLen
       *   既是入参，也是出参，入参作为编码空间的初始偏移量，出参是编码完后，最
       *   终的编码空间的偏移量
       * @return 
       */
      enum CODERET EnCode(const char* pSrcBuf, int iSrcLen, char *pDstMsg, int* piDstMsgLen);
     private:
      int m_iCodeType;
    };
}
#endif

