#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deffiehellman.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gmp_randinit_default(state);
    gmp_randseed_ui (state,time(nullptr) );
    connect(ui->server_gen_key_button,SIGNAL(clicked()), this, SLOT(server_gen_keys_slot()));

    connect(ui->alice_send_message_button, SIGNAL(clicked()), this, SLOT(alice_send_message_slot()));
    connect(this, SIGNAL(send_to_serverA(QByteArray,QByteArray)),this, SLOT(server_get_messageA_slot( QByteArray ,QByteArray)));
    connect(this, SIGNAL(send_to_bob(QByteArray,QByteArray)),this, SLOT(bob_get_message_slot( QByteArray ,QByteArray)));
    connect(this, SIGNAL(alarm_alice_failed(QString)),this, SLOT(alarm_alice_slot(QString)));
    connect(ui->alice_wrong_button,SIGNAL(clicked()), this, SLOT(alice_send_wrong_slot()));

    connect(ui->bob_send_message_button, SIGNAL(clicked()), this, SLOT(bob_send_message_slot()));
    connect(this, SIGNAL(send_to_serverB(QByteArray,QByteArray)),this, SLOT(server_get_messageB_slot( QByteArray ,QByteArray)));
    connect(this, SIGNAL(send_to_alice(QByteArray,QByteArray)),this, SLOT(alice_get_message_slot( QByteArray ,QByteArray)));
    connect(this, SIGNAL(alarm_bob_failed(QString)),this, SLOT(alarm_bob_slot(QString)));
    connect(ui->bob_wrong_button,SIGNAL(clicked()), this, SLOT(bob_send_wrong_slot()));


}

void dh_set_open_keys(mpz_t g, mpz_t p,  mpz_t q,Deffiehellman &dh)
{
    dh.generatePrimeNumbers(p,q,20);
    dh.generateGeneratorNumber(g,p,q);

}

void dh_send(mpz_t result, mpz_t a, const mpz_t g,const mpz_t p,Deffiehellman &dh)
{
    dh.generateNumber(a, 20);
    dh.calculate(result,g,a,p);
}

void dh_get_session(mpz_t result,const mpz_t A, const mpz_t a,const mpz_t p,Deffiehellman &dh)
{
    dh.calculate(result,A,a,p);

}

MainWindow::~MainWindow()
{
    gmp_randclear(state);
    delete ui;
}

void MainWindow::alarm_alice_slot(QString str)
{
    ui->alice_log_edit->append(str + "\n");
    ui->alice_text->append("\t ОШИБКА проверки подписи \n \t Сообщение не было доставлено\n");
}

void MainWindow::alarm_bob_slot(QString str)
{
    ui->bob_log_edit->append(str + "\n");
    ui->bob_text->append("\t ОШИБКА проверки подписи \n \t Сообщение не было доставлено\n");
}

void MainWindow::server_gen_keys_slot()
{
    ui->server_log_edit->append("--------------------");
    ui->server_log_edit->append("Генерация ключей");
    Rsa rsa;
    rsa.generatePrimeNumbers(20);
    rsa.getOpenExp(alice_key.rsa_open_key);
    rsa.getModule(alice_key.rsa_module_key);
    rsa.getClosedExp(server_alice_key.rsa_private_key);
    rsa.getModule(server_alice_key.rsa_self_module_key);
    ui->server_log_edit->append("Открытый ключ сервера для общения с Алисой: " + QString(mpz_get_str(NULL,16,alice_key.rsa_open_key)) +"\n");
    ui->server_log_edit->append("Приватный ключ сервера для общения с Алисой: " + QString(mpz_get_str(NULL,16,server_alice_key.rsa_private_key)) +"\n");
    ui->server_log_edit->append("Модуль: " + QString(mpz_get_str(NULL,16,server_alice_key.rsa_self_module_key)) +"\n");
    ui->server_log_edit->append("\n");
   // rsa.clear();
    rsa.generatePrimeNumbers(20);

    rsa.getClosedExp(alice_key.rsa_private_key);
    rsa.getModule(alice_key.rsa_self_module_key);
    rsa.getOpenExp(server_alice_key.rsa_open_key);
    rsa.getModule(server_alice_key.rsa_module_key);
    ui->alice_log_edit->append("Открытый ключ Алисы для общения с сервером: " + QString(mpz_get_str(NULL,16,server_alice_key.rsa_open_key)) +"\n");
    ui->alice_log_edit->append("Приватный ключ Алисы для общения с сервером: " + QString(mpz_get_str(NULL,16,alice_key.rsa_private_key)) +"\n");
    ui->alice_log_edit->append("Модуль: " + QString(mpz_get_str(NULL,16,server_alice_key.rsa_module_key)) +"\n");
    ui->alice_log_edit->append("\n");

   // rsa.clear();
    rsa.generatePrimeNumbers(20);
    rsa.getOpenExp(bob_key.rsa_open_key);
    rsa.getModule(bob_key.rsa_module_key);
    rsa.getClosedExp(server_bob_key.rsa_private_key);
    rsa.getModule(server_bob_key.rsa_self_module_key);

    ui->server_log_edit->append("Открытый ключ сервера для общения с Бобом: " + QString(mpz_get_str(NULL,16,bob_key.rsa_open_key)) +"\n");
    ui->server_log_edit->append("Приватный ключ сервера для общения с Бобом: " + QString(mpz_get_str(NULL,16,server_bob_key.rsa_private_key)) +"\n");
    ui->server_log_edit->append("Модуль: " + QString(mpz_get_str(NULL,16,server_bob_key.rsa_self_module_key)) +"\n");
    ui->server_log_edit->append("\n");

    //rsa.clear();
    rsa.generatePrimeNumbers(20);
    rsa.getClosedExp(bob_key.rsa_private_key);
    rsa.getModule(bob_key.rsa_self_module_key);
    rsa.getOpenExp(server_bob_key.rsa_open_key);
    rsa.getModule(server_bob_key.rsa_module_key);
    ui->bob_log_edit->append("Открытый ключ Боба для общения с сервером: " + QString(mpz_get_str(NULL,16,server_bob_key.rsa_open_key)) +"\n");
    ui->bob_log_edit->append("Приватный ключ Боба для общения с сервером: " + QString(mpz_get_str(NULL,16,bob_key.rsa_private_key)) +"\n");
    ui->bob_log_edit->append("Модуль: " + QString(mpz_get_str(NULL,16,server_bob_key.rsa_module_key)) +"\n");
    ui->bob_log_edit->append("\n");

    mpz_t g;
    mpz_t p;
    mpz_t q;
    mpz_t a;
    mpz_t b;
    mpz_t A;
    mpz_t B;
    mpz_t k1;
    mpz_t k2;

    mpz_init(g);
    mpz_init(p);
    mpz_init(q);
    mpz_init(a);
    mpz_init(b);
    mpz_init(A);
    mpz_init(B);
    mpz_init(k1);
    mpz_init(k2);
    Deffiehellman dh;

    dh_set_open_keys(g,p,q,dh);
    dh_send(A,a,g,p,dh);
    dh_send(B,b,g,p,dh);
    dh_get_session(k1,B, a, p,dh);
    dh_get_session(k2,A, b,p,dh);
    mpz_set(alice_key.session_key, k1);
    mpz_set(server_alice_key.session_key, k2);
    ui->alice_log_edit->append("Сессионный ключ Алисы и Сервера: " + QString(mpz_get_str(NULL,16,alice_key.session_key)) +"\n");
    ui->server_log_edit->append("Сессионный ключ Алисы и Сервера: " + QString(mpz_get_str(NULL,16,alice_key.session_key)) +"\n");


    dh_set_open_keys(g,p,q,dh);
    dh_send(A,a,g,p,dh);
    dh_send(B,b,g,p,dh);
    dh_get_session(k1,B, a, p,dh);
    dh_get_session(k2,A, b,p,dh);
    mpz_set(bob_key.session_key, k1);
    mpz_set(server_bob_key.session_key, k2);

    ui->bob_log_edit->append("Сессионный ключ Боба и Сервера: " + QString(mpz_get_str(NULL,16,bob_key.session_key)) +"\n");
    ui->server_log_edit->append("Сессионный ключ Боба и Сервера: " + QString(mpz_get_str(NULL,16,bob_key.session_key)) +"\n");

    ui->server_log_edit->append("--------------------");
    ui->bob_log_edit->append("===========================");
    ui->alice_log_edit->append("===========================");
    key_flag = 1;

    mpz_clear(g);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(b);
    mpz_clear(A);
    mpz_clear(B);
    mpz_clear(k1);
    mpz_clear(k2);

}

void MainWindow::server_get_messageA_slot( QByteArray enc_alice_text,QByteArray enc_alice_hash)
{
    ui->server_log_edit->append("1. Сервер получил сообщение от Алисы");
    GOST gost;
    Rsa rsa;
    Streebog *hashCore = new Streebog();
    char *hash;
    size_t dec_hash_size;
    rsa.clear();
    rsa.setClosedExp(server_alice_key.rsa_private_key);
    rsa.setOpenExp(alice_key.rsa_open_key);
    rsa.setModule(server_alice_key.rsa_self_module_key);

    rsa.setBufferSize(128);
    char *decrypted_hash = rsa.decrypt(enc_alice_hash.constData(),enc_alice_hash.size(),&dec_hash_size);

    ui->server_log_edit->append("2. Расшифрованный хэш сообщения Алисы: " + QByteArray(decrypted_hash,64).toHex() + "\n");
    size_t size = mpz_sizeinbase(alice_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, server_alice_key.session_key);

    int enc_text_size = enc_alice_text.size();
    char *buf = new char[enc_alice_text.size()+8];
    for (int i = 0; i < enc_alice_text.size(); i++)
    {
        buf[i] = enc_alice_text[i];
    }
    gost.gost_ecb_decrypt_text(buf,enc_text_size,strparam);

    ui->server_log_edit->append("3. Расшифрованное сообщение Алисы: " + QByteArray(buf,enc_text_size) + "\n");
    int startPos = enc_text_size - 64;
    hashCore->Clear();
    if ( enc_text_size > 64)
    {
        int count = enc_text_size/64;
        if( enc_text_size % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
        hash = ( char*)hashCore->getHashResult();
    }
    ui->server_log_edit->append("4. Хэш расшифрованного сообщения: " + QByteArray(hash,64).toHex() + "\n");
    if( memcmp(hash,decrypted_hash,64) != 0)
    {
         ui->server_log_edit->append("ОШИБКА проверки ЭП сообщения \n");
         emit alarm_alice_failed("Ошибка проверки на стороне сервера");
         ui->server_log_edit->append("Замена ключей \n");
         server_gen_keys_slot();

    } else {
        ui->server_log_edit->append("Успешная  проверка  ЭП  сообщения \n");
        hashCore->Clear();
        if ( enc_text_size > 64)
        {
            int count = enc_text_size/64;
            if( enc_text_size % 64 == 0)
                count--;
            for(int i = 0; i < count; i ++)
            {
                hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
            }
            hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
            hash = (char*)hashCore->getHashResult();

         } else {

            hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
            hash = ( char*)hashCore->getHashResult();
        }
        if(ui->server_status_box->isChecked())
        {
            hash[0]++;
        }
        strparam = mpz_get_str(strparam, 10, server_bob_key.session_key);
        gost.gost_ecb_encrypt_text(buf,enc_text_size,strparam);
        ui->server_log_edit->append("5. Зашифрованное сообщение Алисы на сессионом ключе Боба и сервера " + QByteArray(buf,enc_text_size).toHex() + "\n");
        rsa.setClosedExp(bob_key.rsa_private_key);
        rsa.setOpenExp(server_bob_key.rsa_open_key);
        rsa.setModule(bob_key.rsa_self_module_key);
        rsa.setBufferSize(128);
        size_t size_res;
        char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1);
        QByteArray enc_bob_text(buf,enc_text_size);
        QByteArray enc_bob_hash(encrypted_hash,size_res);

        ui->server_log_edit->append("6. Сервер подписал сообщение Алисы и отправил Бобу \n");
        emit send_to_bob(enc_bob_text,enc_bob_hash);
     }

    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}

void MainWindow::bob_get_message_slot( QByteArray enc_bob_text,QByteArray enc_bob_hash)
{
    ui->bob_log_edit->append("1. Боб получил сообщение от сервера \n");
    GOST gost;
    Rsa rsa;
    char *hash;
    size_t dec_hash_size;
    int enc_text_size = enc_bob_text.size();
    Streebog *hashCore = new Streebog();

    rsa.setClosedExp(bob_key.rsa_private_key);
    rsa.setOpenExp(server_bob_key.rsa_open_key);
    rsa.setModule(bob_key.rsa_self_module_key);

    rsa.setBufferSize(128);
    char *decrypted_hash = rsa.decrypt(enc_bob_hash.constData(),enc_bob_hash.size(),&dec_hash_size);
    ui->bob_log_edit->append("2. Расшифрованный хэш сообщения " + QByteArray(decrypted_hash,64).toHex() + "\n");
    char *buf = new char[enc_bob_text.size()+8];
    for (int i = 0; i < enc_bob_text.size(); i++)
    {
        buf[i] = enc_bob_text[i];
    }

    size_t size = mpz_sizeinbase(bob_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, bob_key.session_key);

    gost.gost_ecb_decrypt_text(buf,enc_text_size,strparam);
    ui->bob_log_edit->append("3. Расшифрованное сообщение " + QByteArray(buf,enc_text_size) + "\n");
    int startPos = enc_text_size - 64;
    hashCore->Clear();
    if ( enc_text_size > 64)
    {
        int count = enc_text_size/64;
        if( enc_text_size % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
        hash = ( char*)hashCore->getHashResult();
    }
    ui->bob_log_edit->append("4. Хэш расшифрованного сообщения " + QByteArray(hash,64).toHex() + "\n");

    if( memcmp(hash,decrypted_hash,64) != 0)
    {
        ui->bob_log_edit->append("ОШИБКА проверки  ЭП  сообщения \n");
        emit alarm_alice_failed("Ошибка проверки на стороне Боба");
        ui->server_log_edit->append("Сервер инициирует замену ключей \n");
        server_gen_keys_slot();

    } else
    {
        ui->bob_log_edit->append("Успешная  проверка  ЭП  сообщения");
        ui->bob_text->append("Алиса: " + QByteArray(buf,enc_text_size) + "\n");
    // ВЫВОД НА ЭКРАН

    }


    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete[] decrypted_hash;
    delete hashCore;
}

void MainWindow::alice_send_message_slot()
{

    if(key_flag == 0)
    {
        server_gen_keys_slot();
    }

    QString text = ui->alice_send_text->text();
    if(text.length() == 0)
    {
        return;
    }
    if(text.length() > 1024)
    {
        QMessageBox::warning(this,"Warning", "Ошибка открытия файла");
        return;
    }


    GOST gost;
    Rsa rsa;

    Streebog *hashCore = new Streebog();
    char *hash;
    ui->alice_send_text->clear();
    QByteArray textByteArray( text.toUtf8() );

    int textSize = textByteArray.size();
    int startPos = textSize - 64;

    char *buf = new char[textSize+8];
    for (int i = 0; i < textSize; i++)
    {
        buf[i] = textByteArray[i];
    }

    size_t size = mpz_sizeinbase(alice_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, alice_key.session_key);
    ui->alice_log_edit->append("1. Алиса хочет отправить Бобу сообщение \n");
    gost.gost_ecb_encrypt_text(buf,textSize,strparam);
    ui->alice_log_edit->append("2. Зашифрованное сессионым ключём сообщение Алисы:" + QByteArray(buf,textSize).toHex() + "\n");


    if ( textByteArray.size() > 64)
    {
        int count = textByteArray.size()/64;
        if( textByteArray.size() % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(textByteArray.constData() + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(textByteArray.constData()),textByteArray.size() - count*64,textByteArray.size());
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)textByteArray.constData(),textByteArray.size(),textByteArray.size());
        hash = ( char*)hashCore->getHashResult();
    }
    ui->alice_log_edit->append("3. Хэш сообщения Алисы: " + QByteArray(hash,64).toHex() + "\n");
    rsa.clear();
    rsa.setClosedExp(server_alice_key.rsa_private_key);
    rsa.setOpenExp(alice_key.rsa_open_key);
    rsa.setModule(server_alice_key.rsa_self_module_key);

    size_t size_res;
    rsa.setBufferSize(128);
    char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1); //зашифровали ассим
    ui->alice_log_edit->append("4. Алиса подписала сообщение и отправила на сервер \n");
    QByteArray enc_text(buf,textSize);
    QByteArray enc_hash(encrypted_hash,size_res);
    ui->alice_text->append("Алиса: " + text + "\n");
    emit send_to_serverA(enc_text,enc_hash);

    delete[] encrypted_hash;
    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}
void MainWindow::alice_send_wrong_slot()
{

    if(key_flag == 0)
    {
        server_gen_keys_slot();
    }

    QString text = ui->alice_send_text->text();
    if(text.length() == 0)
    {
        return;
    }
    if(text.length() > 1024)
    {
        QMessageBox::warning(this,"Warning", "Ошибка открытия файла");
        return;
    }


    GOST gost;
    Rsa rsa;

    Streebog *hashCore = new Streebog();
    char *hash;
    ui->alice_send_text->clear();
    QByteArray textByteArray( text.toUtf8() );

    int textSize = textByteArray.size();
    int startPos = textSize - 64;

    char *buf = new char[textSize+8];
    for (int i = 0; i < textSize; i++)
    {
        buf[i] = textByteArray[i];
    }

    size_t size = mpz_sizeinbase(alice_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, alice_key.session_key);
    ui->alice_log_edit->append("1. Алиса хочет отправить Бобу сообщение \n");
    gost.gost_ecb_encrypt_text(buf,textSize,strparam);
    ui->alice_log_edit->append("2. Зашифрованное сессионым ключём сообщение Алисы:" + QByteArray(buf,textSize).toHex() + "\n");


    if ( textByteArray.size() > 64)
    {
        int count = textByteArray.size()/64;
        if( textByteArray.size() % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(textByteArray.constData() + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(textByteArray.constData()),textByteArray.size() - count*64,textByteArray.size());
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)textByteArray.constData(),textByteArray.size(),textByteArray.size());
        hash = ( char*)hashCore->getHashResult();
    }
    ui->alice_log_edit->append("3. Хэш сообщения Алисы: " + QByteArray(hash,64).toHex() + "\n");
    rsa.clear();
    hash[0]++;
    rsa.setClosedExp(server_alice_key.rsa_private_key);
    rsa.setOpenExp(alice_key.rsa_open_key);
    rsa.setModule(server_alice_key.rsa_self_module_key);

    size_t size_res;
    rsa.setBufferSize(128);
    char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1); //зашифровали ассим
    ui->alice_log_edit->append("4. Алиса подписала сообщение и отправила на сервер \n");
    QByteArray enc_text(buf,textSize);
    QByteArray enc_hash(encrypted_hash,size_res);
    ui->alice_text->append("Алиса: " + text + "\n");
    emit send_to_serverA(enc_text,enc_hash);

    delete[] encrypted_hash;
    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}
//bob

void MainWindow::server_get_messageB_slot( QByteArray enc_bob_text,QByteArray enc_bob_hash)
{
    ui->server_log_edit->append("1. Сервер получил сообщение от Алисы");
    GOST gost;
    Rsa rsa;
    Streebog *hashCore = new Streebog();
    char *hash;
    size_t dec_hash_size;
    rsa.clear();
    rsa.setClosedExp(server_bob_key.rsa_private_key);
    rsa.setOpenExp(bob_key.rsa_open_key);
    rsa.setModule(server_bob_key.rsa_self_module_key);

    rsa.setBufferSize(128);
    char *decrypted_hash = rsa.decrypt(enc_bob_hash.constData(),enc_bob_hash.size(),&dec_hash_size);

    ui->server_log_edit->append("2. Расшифрованный хэш сообщения Боба: " + QByteArray(decrypted_hash,64).toHex() + "\n");
    size_t size = mpz_sizeinbase(bob_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, server_bob_key.session_key);

    int enc_text_size = enc_bob_text.size();
    char *buf = new char[enc_bob_text.size()+8];
    for (int i = 0; i < enc_bob_text.size(); i++)
    {
        buf[i] = enc_bob_text[i];
    }
    gost.gost_ecb_decrypt_text(buf,enc_text_size,strparam);

    ui->server_log_edit->append("3. Расшифрованное сообщение Боба: " + QByteArray(buf,enc_text_size) + "\n");
    int startPos = enc_text_size - 64;
    hashCore->Clear();
    if ( enc_text_size > 64)
    {
        int count = enc_text_size/64;
        if( enc_text_size % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
        hash = ( char*)hashCore->getHashResult();
    }
    ui->server_log_edit->append("4. Хэш расшифрованного сообщения: " + QByteArray(hash,64).toHex() + "\n");
    if( memcmp(hash,decrypted_hash,64) != 0)
    {
         ui->server_log_edit->append("ОШИБКА проверки ЭП сообщения \n");
         emit alarm_bob_failed("Ошибка проверки на стороне сервера");
         ui->server_log_edit->append("Замена ключей \n");
         server_gen_keys_slot();

    } else {
        ui->server_log_edit->append("Успешная  проверка  ЭП  сообщения \n");
        hashCore->Clear();
        if ( enc_text_size > 64)
        {
            int count = enc_text_size/64;
            if( enc_text_size % 64 == 0)
                count--;
            for(int i = 0; i < count; i ++)
            {
                hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
            }
            hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
            hash = (char*)hashCore->getHashResult();

         } else {

            hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
            hash = ( char*)hashCore->getHashResult();
        }
        if(ui->server_status_box->isChecked())
        {
            hash[0]++;
        }
        strparam = mpz_get_str(strparam, 10, server_alice_key.session_key);
        gost.gost_ecb_encrypt_text(buf,enc_text_size,strparam);
        ui->server_log_edit->append("5. Зашифрованное сообщение Боба на сессионом ключе Алисы и сервера " + QByteArray(buf,enc_text_size).toHex() + "\n");
        rsa.setClosedExp(alice_key.rsa_private_key);
        rsa.setOpenExp(server_alice_key.rsa_open_key);
        rsa.setModule(alice_key.rsa_self_module_key);
        rsa.setBufferSize(128);
        size_t size_res;
        char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1);
        QByteArray enc_alice_text(buf,enc_text_size);
        QByteArray enc_alice_hash(encrypted_hash,size_res);

        ui->server_log_edit->append("6. Сервер подписал сообщение Боба и отправил Алисе \n");
        emit send_to_alice(enc_alice_text,enc_alice_hash);
     }

    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}

void MainWindow::alice_get_message_slot( QByteArray enc_alice_text,QByteArray enc_alice_hash)
{
    ui->alice_log_edit->append("1. Алиса получила сообщение от сервера \n");
    GOST gost;
    Rsa rsa;
    char *hash;
    size_t dec_hash_size;
    int enc_text_size = enc_alice_text.size();
    Streebog *hashCore = new Streebog();

    rsa.setClosedExp(alice_key.rsa_private_key);
    rsa.setOpenExp(server_alice_key.rsa_open_key);
    rsa.setModule(alice_key.rsa_self_module_key);

    rsa.setBufferSize(128);
    char *decrypted_hash = rsa.decrypt(enc_alice_hash.constData(),enc_alice_hash.size(),&dec_hash_size);
    ui->alice_log_edit->append("2. Расшифрованный хэш сообщения " + QByteArray(decrypted_hash,64).toHex() + "\n");
    char *buf = new char[enc_alice_text.size()+8];
    for (int i = 0; i < enc_alice_text.size(); i++)
    {
        buf[i] = enc_alice_text[i];
    }

    size_t size = mpz_sizeinbase(alice_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, alice_key.session_key);

    gost.gost_ecb_decrypt_text(buf,enc_text_size,strparam);
    ui->alice_log_edit->append("3. Расшифрованное сообщение " + QByteArray(buf,enc_text_size) + "\n");
    int startPos = enc_text_size - 64;
    hashCore->Clear();
    if ( enc_text_size > 64)
    {
        int count = enc_text_size/64;
        if( enc_text_size % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(buf + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(buf),enc_text_size - count*64,enc_text_size);
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)buf,enc_text_size,enc_text_size);
        hash = ( char*)hashCore->getHashResult();
    }
    ui->alice_log_edit->append("4. Хэш расшифрованного сообщения " + QByteArray(hash,64).toHex() + "\n");

    if( memcmp(hash,decrypted_hash,64) != 0)
    {
        ui->alice_log_edit->append("ОШИБКА проверки  ЭП  сообщения \n");
        emit alarm_bob_failed("Ошибка проверки на стороне Алисы");
        ui->server_log_edit->append("Сервер инициирует замену ключей \n");
        server_gen_keys_slot();

    } else
    {
        ui->alice_log_edit->append("Успешная  проверка  ЭП  сообщения");
        ui->alice_text->append("Боб: " + QByteArray(buf,enc_text_size) + "\n");
    // ВЫВОД НА ЭКРАН

    }


    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete[] decrypted_hash;
    delete hashCore;
}

void MainWindow::bob_send_message_slot()
{

    if(key_flag == 0)
    {
        server_gen_keys_slot();
    }

    QString text = ui->bob_send_text->text();
    if(text.length() == 0)
    {
        return;
    }
    if(text.length() > 1024)
    {
        QMessageBox::warning(this,"Warning", "Сообщение не должно быть больше 1024 знаков");
        return;
    }


    GOST gost;
    Rsa rsa;

    Streebog *hashCore = new Streebog();
    char *hash;
    ui->bob_send_text->clear();
    QByteArray textByteArray( text.toUtf8() );

    int textSize = textByteArray.size();
    int startPos = textSize - 64;

    char *buf = new char[textSize+8];
    for (int i = 0; i < textSize; i++)
    {
        buf[i] = textByteArray[i];
    }

    size_t size = mpz_sizeinbase(bob_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, bob_key.session_key);
    ui->bob_log_edit->append("1. Боб хочет отправить Алисе сообщение \n");
    gost.gost_ecb_encrypt_text(buf,textSize,strparam);
    ui->bob_log_edit->append("2. Зашифрованное сессионым ключём сообщение Боба:" + QByteArray(buf,textSize).toHex() + "\n");


    if ( textByteArray.size() > 64)
    {
        int count = textByteArray.size()/64;
        if( textByteArray.size() % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(textByteArray.constData() + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(textByteArray.constData()),textByteArray.size() - count*64,textByteArray.size());
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)textByteArray.constData(),textByteArray.size(),textByteArray.size());
        hash = ( char*)hashCore->getHashResult();
    }
    ui->bob_log_edit->append("3. Хэш сообщения Боба: " + QByteArray(hash,64).toHex() + "\n");
    rsa.clear();
    rsa.setClosedExp(server_bob_key.rsa_private_key);
    rsa.setOpenExp(bob_key.rsa_open_key);
    rsa.setModule(server_bob_key.rsa_self_module_key);

    size_t size_res;
    rsa.setBufferSize(128);
    char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1); //зашифровали ассим
    ui->bob_log_edit->append("4. Боб подписал сообщение и отправил на сервер \n");
    QByteArray enc_text(buf,textSize);
    QByteArray enc_hash(encrypted_hash,size_res);
    ui->bob_text->append("Боб: " + text + "\n");
    emit send_to_serverB(enc_text,enc_hash);

    delete[] encrypted_hash;
    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}

void MainWindow::bob_send_wrong_slot()
{

    if(key_flag == 0)
    {
        server_gen_keys_slot();
    }

    QString text = ui->bob_send_text->text();
    if(text.length() == 0)
    {
        return;
    }
    if(text.length() > 1024)
    {
        QMessageBox::warning(this,"Warning", "Сообщение не должно быть больше 1024 знаков");
        return;
    }


    GOST gost;
    Rsa rsa;

    Streebog *hashCore = new Streebog();
    char *hash;
    ui->bob_send_text->clear();
    QByteArray textByteArray( text.toUtf8() );

    int textSize = textByteArray.size();
    int startPos = textSize - 64;

    char *buf = new char[textSize+8];
    for (int i = 0; i < textSize; i++)
    {
        buf[i] = textByteArray[i];
    }

    size_t size = mpz_sizeinbase(bob_key.session_key, 10);
    char *strparam = new char[size+3];
    strparam = mpz_get_str(strparam, 10, bob_key.session_key);
    ui->bob_log_edit->append("1. Боб хочет отправить Алисе сообщение \n");
    gost.gost_ecb_encrypt_text(buf,textSize,strparam);
    ui->bob_log_edit->append("2. Зашифрованное сессионым ключём сообщение Боба:" + QByteArray(buf,textSize).toHex() + "\n");


    if ( textByteArray.size() > 64)
    {
        int count = textByteArray.size()/64;
        if( textByteArray.size() % 64 == 0)
            count--;
        for(int i = 0; i < count; i ++)
        {
            hashCore->hashBlock((unsigned char*)(textByteArray.constData() + startPos - i*64));
        }
        hashCore->hashFinaleBlock((unsigned char*)(textByteArray.constData()),textByteArray.size() - count*64,textByteArray.size());
        hash = (char*)hashCore->getHashResult();

     } else {

        hashCore->hashFinaleBlock((unsigned char*)textByteArray.constData(),textByteArray.size(),textByteArray.size());
        hash = ( char*)hashCore->getHashResult();
    }
    ui->bob_log_edit->append("3. Хэш сообщения Боба: " + QByteArray(hash,64).toHex() + "\n");
    rsa.clear();
    rsa.setClosedExp(server_bob_key.rsa_private_key);
    rsa.setOpenExp(bob_key.rsa_open_key);
    rsa.setModule(server_bob_key.rsa_self_module_key);

    size_t size_res;
    rsa.setBufferSize(128);
    hash[0]++;
    char *encrypted_hash = rsa.encrypt(hash,64,&size_res,1); //зашифровали ассим
    ui->bob_log_edit->append("4. Боб подписал сообщение и отправил на сервер \n");
    QByteArray enc_text(buf,textSize);
    QByteArray enc_hash(encrypted_hash,size_res);
    ui->bob_text->append("Боб: " + text + "\n");
    emit send_to_serverB(enc_text,enc_hash);

    delete[] encrypted_hash;
    delete[] buf;
    delete[] strparam;
    delete[] hash;
    delete hashCore;
}

