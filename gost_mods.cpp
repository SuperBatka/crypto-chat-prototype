#include "gost_mods.h"

GOST::GOST (QObject *parent)
{

}
GOST::~GOST()
{

}

int GOST::addPaddingText( char *buffer, int position, int blockSize)
{
    int padNum =blockSize - (position % blockSize);
    char paddingByte = 0x01;
    for(int i = 1 ; i < padNum; i++)
    {
        paddingByte+=0x01;
    }

    for(int i = 0; i < padNum; i++)
    {
        buffer[position + i] = paddingByte;
    }

    return padNum;
}



void GOST::gost_ecb_encrypt_text(char *buffer, int &len, char *key)
{
     gost_context ctx;
     gost_key_setup(key, &ctx);

     quint32 out[2], L, R;

     len += addPaddingText(buffer,len,8);
     for(int i = 0 ; i <= len - 8; i+=8)
     {
         L = GET_32BIT_MSB_FIRST(buffer + i);
         R = GET_32BIT_MSB_FIRST(buffer + 4 + i);
         gostEncryptBlock(out, L, R, &ctx);
         PUT_32BIT_MSB_FIRST(buffer + i, out[0]);
         PUT_32BIT_MSB_FIRST(buffer + 4 + i, out[1]);

     }



}


void GOST::gost_ecb_decrypt_text(char *buffer, int &len, char *key)
{
     gost_context ctx;
     gost_key_setup(key, &ctx);
     quint32 out[2], L, R;

     for(int i = 0 ; i <= len - 8; i+=8)
     {
         L = GET_32BIT_MSB_FIRST(buffer + i);
         R = GET_32BIT_MSB_FIRST(buffer + 4 + i);
         gostDecryptBlock(out, L, R, &ctx);
         PUT_32BIT_MSB_FIRST(buffer + i, out[0]);
         PUT_32BIT_MSB_FIRST(buffer + 4 + i, out[1]);

     }
        char paddingByte =buffer[len-1];
        for(char k = paddingByte; k > 0x00; k--)
        {
            len--;
        }
}



void GOST::gost_gamma_encrypt_text(char *buffer, int &len, char *key)
{
     gost_context ctx;
     gost_key_setup(key, &ctx);

     quint32 leftIV =  GET_32BIT_MSB_FIRST(key);
     quint32 rightIV = GET_32BIT_MSB_FIRST(key + 4);

     quint32 out[2], L, R;


     out[0] = leftIV;
     out[1] = rightIV;

        len += addPaddingText(buffer,len,8);
        for(int i = 0 ; i <= len - 8; i+=8)
        {
            gostEncryptBlock(out, out[0], out[1], &ctx);
            L = GET_32BIT_MSB_FIRST(buffer + i);
            R = GET_32BIT_MSB_FIRST(buffer + 4 + i);

            out[0] ^= L;
            out[1] ^= R;

            PUT_32BIT_MSB_FIRST(buffer + i, out[0]);
            PUT_32BIT_MSB_FIRST(buffer + 4 + i, out[1]);

        }

}


void GOST::gost_gamma_decrypt_text(char *buffer, int &len, char *key)
{

     gost_context ctx;
     gost_key_setup(key, &ctx);

     quint32 leftIV =  GET_32BIT_MSB_FIRST(key);
     quint32 rightIV = GET_32BIT_MSB_FIRST(key + 4);

     quint32 out[2], L, R;

      L = leftIV;
      R = rightIV;

      for(int i = 0 ; i <= len - 8; i+=8)
      {
          gostEncryptBlock(out, L, R, &ctx);
          L = GET_32BIT_MSB_FIRST(buffer + i);
          R = GET_32BIT_MSB_FIRST(buffer + 4 + i);

          out[0] ^= L;
          out[1] ^= R;

          PUT_32BIT_MSB_FIRST(buffer + i, out[0]);
          PUT_32BIT_MSB_FIRST(buffer + 4 + i, out[1]);

      }


        char paddingByte =buffer[len-1];
        for(char k = paddingByte; k > 0x00; k--)
        {
            len--;
        }

}




