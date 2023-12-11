#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define SIMUL

#define SETADD \
    m_NamesAdd[tr("Generateur")]             = 0xBB;\
    m_NamesAdd[tr("Gaz")]                    = 0xBC;\
    m_NamesAdd[tr("Plasma")]                 = 0xBD;\
    m_NamesAdd[tr("Arret d'urgence")]        = 0x65;\
    m_NamesAdd[tr("Defaut critique")]        = 0x66;\
    m_NamesAdd[tr("Etat du procede")]        = 0x6E;\
    m_NamesAdd[tr("Consigne puissance")]     = 0xB2;\
    m_NamesAdd[tr("Consigne debit")]         = 0xB3;\
    m_NamesAdd[tr("Mesure puissance")]       = 0x6B;\
    m_NamesAdd[tr("Mesure debit")]           = 0x68;\
    m_NamesAdd[tr("Courant pont")]           = 0x7F;\
    m_NamesAdd[tr("Tension PFC ")]           = 0x72;\
    m_NamesAdd[tr("Puissance limite basse")] = 0x96;\
    m_NamesAdd[tr("Puissance limite haute")] = 0x97;\
    m_NamesAdd[tr("Debit bas")]              = 0xA0;\
    m_NamesAdd[tr("Debit haut")]             = 0xA1;\
    m_nReg = m_NamesAdd.size();\
    QVector<QString> MODIFIABLES = {"Consigne puissance","Consigne debit","Puissance limite basse",\
"Puissance limite haute","Debit bas","Debit haut"};

#define ALIVE_ADDRESS 205
#define ALIVE_VALUE 330
#define WATCHDOG_TIME_MILLISEC 1000
//#define MODBUS_ACCESS_REG_TIMEOUT_MILLISEC 5 NOT USED
/*
 *  Connect to the board through an usb line. The parameters can be changed from the GUI,
 *  the possible and default values are set in the comboboxes of mainwindow.ui.
 *  A watchgod can be run at a frequency set by WATCHDOG_TIME_MILLISEC above.
 *  In case of any error (serial connection close or board not powered) the application finish with an explicit error message.
 *  When sending values all values are sent and re-read.
 *  To add a register modify the macro SETADD above (don't forget to modify also MODIFIABNLES if necessary)
 */

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QGridLayout>
#include <QLineEdit>
#include <QTimer>

#include <modbus/modbus.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    modbus_t *m_ctx;
    int m_nReg;
    QMap<QString, unsigned int> m_NamesAdd;
    QVector<int> m_adds;
    QVector<int> m_registerVals;
    QVector<int> m_valuesToSend;
    int m_bidon; // testing purpose
#ifdef SIMUL
    QVector<int> m_fake;
#endif
    int m_NOTConnected;
    QVector<QLabel*> m_labelsValue;
    QVector<QLabel*> m_labelsName;
    QVector<QLineEdit*> m_lineEditsValue;
    QVector<int> m_modifiables;
    QTimer *m_timer;

    void mySetupUi();
    void connectDevice();
    void updateValuesOnGui();
    void toggleWatchdog();
    int getValues(); // register values are read from the modbus
    int setValues(); // register values are written to the modbus
    void checkAlive();
};

#define TRACE(msg) {QMessageBox mb;mb.setText(msg);mb.exec();}
#define EXIT(msg) {QMessageBox mb;mb.setText(msg);mb.exec();qApp->exit(0);exit(0);}

#endif // MAINWINDOW_H
