#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <gmp.h>
#include "gost_mods.h"
#include "streebog.h"
#include "rsa.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void send_to_serverA(QByteArray,QByteArray);
    void send_to_bob(QByteArray,QByteArray);
    void alarm_alice_failed(QString);

    void send_to_serverB(QByteArray,QByteArray);
    void send_to_alice(QByteArray,QByteArray);
    void alarm_bob_failed(QString);

private slots:
    void server_gen_keys_slot();
    void alice_send_message_slot();
    void alice_send_wrong_slot();
    void bob_get_message_slot( QByteArray enc_bob_text,QByteArray enc_bob_hash);
    void server_get_messageA_slot( QByteArray enc_alice_text,QByteArray enc_alice_hash);
    void alarm_alice_slot(QString str);


    void bob_send_message_slot();
    void bob_send_wrong_slot();
    void alice_get_message_slot( QByteArray enc_bob_text,QByteArray enc_bob_hash);
    void server_get_messageB_slot( QByteArray enc_alice_text,QByteArray enc_alice_hash);
    void alarm_bob_slot(QString str);

private:
    Ui::MainWindow *ui;

    struct Kparam
    {
        mpz_t session_key;
        mpz_t rsa_open_key;
        mpz_t rsa_self_module_key;
        mpz_t rsa_module_key;
        mpz_t rsa_private_key;
        mpz_t server_open_key;
        Kparam()
        {
            mpz_init(session_key);
            mpz_init(rsa_open_key);
            mpz_init(rsa_self_module_key);
            mpz_init(rsa_module_key);
            mpz_init(rsa_private_key);
            mpz_init(server_open_key);
        }
        ~Kparam()
        {
            mpz_clear(session_key);
            mpz_clear(rsa_open_key);
            mpz_clear(rsa_self_module_key);
            mpz_clear(rsa_module_key);
            mpz_clear(rsa_private_key);
            mpz_clear(server_open_key);
        }
    };

    Kparam alice_key;
    Kparam bob_key;
    Kparam server_alice_key;
    Kparam server_bob_key;
    gmp_randstate_t state;
    bool key_flag = 0;

};
#endif // MAINWINDOW_H
