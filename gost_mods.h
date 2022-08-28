#ifndef GOST_MODS_H
#define GOST_MODS_H
#include "gost.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QFileInfo>



class GOST : public QObject
{
    Q_OBJECT

public:
    explicit GOST (QObject *parent = 0);
    ~GOST();
    void gost_ecb_encrypt_text(char *buffer, int &len, char *key);
    void gost_ecb_decrypt_text(char *buffer, int &len, char *key);

    void gost_gamma_encrypt_text(char *buffer, int &len, char *key);
    void gost_gamma_decrypt_text(char *buffer, int &len, char *key);

private:
    int addPaddingText( char *buffer, int position, int blockSize);


};

#endif // GOST_MODS_H
