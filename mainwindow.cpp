#include <modbus/modbus.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,600);
    mySetupUi();
#ifdef SIMUL
    TRACE("tr(running in simulation mode)");
    m_fake.resize(m_registerVals.size(),-2);
    m_NOTConnected = 0;
    ui->labelSimul->setVisible(1);
    for(uint i=0;i<m_nReg;i++)
        m_fake.push_back(-i);
#endif
    m_registerVals.resize(m_nReg);
    m_valuesToSend.resize(m_nReg);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifdef SIMUL
int MainWindow::setValues() {
    for(uint i=0;i<m_nReg;i++)
        m_fake[i] = m_valuesToSend[i];
    return m_nReg;
}    // FIN int MainWindow::setValues()
// ******************************************************************************************

int MainWindow::getValues() {
    for(uint i=0;i<m_nReg;i++)
        m_registerVals[i] = m_fake[i];
    return m_nReg;
}    // FIN int MainWindow::getValues()
// ******************************************************************************************

void MainWindow::connectDevice() {
    ui->connectBtn->setVisible(0);
    ui->sendValuesBtn->setVisible(1);
    for(uint i=0;i<m_labelsValue.size();i++) {
        m_labelsValue[i]->setVisible(1);
        m_labelsName[i]->setVisible(1);
        m_lineEditsValue[i]->setVisible(1);
    }
    for (uint i=0;i<m_nReg;i++)
        m_lineEditsValue[i]->setText(QString::number(m_registerVals[i])); // since updateValueOnGui will send these values
    updateValuesOnGui();
    ui->statusbar->showMessage(tr("connected"));
}    // FIN void MainWindow::connectDevice()
// ******************************************************************************************

#else
int MainWindow::setValues()
    // modifiable register values are written to the modbus from vector m_registerVals
    // number of values effectively written is returned
{
    int cpt=0;
    for (uint i=0;i<m_registerVals.size();i++) {
        if (!m_modifiables[i])
            continue;
        int addr = m_adds[i];
        uint16_t dest = m_valuesToSend[i];
        if (modbus_write_registers(m_ctx,addr,1,&dest) == 1)
            cpt++;
        else {
            EXIT("Register at " +QString::number(addr)+" could not be written");
        }
    }
    return cpt;
}      // FIN int MainWindow::setValues()
// ******************************************************************************************

int MainWindow::getValues()
    // register values are read from the modbus to set the vector m_registerVals
    // number of values effectively read is returned
{
    int cpt=0;
    for (uint i=0;i<m_registerVals.size();i++) {
        int addr = m_adds[i];
        uint16_t dest;
        if (modbus_read_registers(m_ctx,addr,1,&dest) == 1)
            cpt++;
        else {
            EXIT("Register at " +QString::number(addr)+" could not be read");
        }
        m_registerVals[i] = dest;
    }
    return cpt;
}    // FIN int MainWindow::getValues()
// ******************************************************************************************

void MainWindow::connectDevice() {
    // connect and set m_NOTConnected with the appopriate value
    // in case of problem a message is shown
    QString pcQ = ui->portCom->currentText();
    std::string pcS = pcQ.toStdString();
    const char* pc = pcS.c_str();
    int br = ui->baudRate->currentText().toInt();
    char pa = ui->parity->currentText().toStdString()[0];
    int nb = ui->nbBits->currentText().toInt();
    int sb = ui->stopBits->currentText().toInt();
    m_ctx = modbus_new_rtu(pc, br, pa, nb, sb);
    if (m_ctx == NULL) {
        TRACE(tr("modbus_new_rtu failed"));
        m_NOTConnected = 1;
        return ;
    }
    //modbus_set_debug(m_ctx, 1);
    int con = modbus_connect(m_ctx);
    if (con == -1) {
        //fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        TRACE(tr("Connection failed : ") +QString(modbus_strerror(errno))
              +" returnValue="+QString::number(con)
              +" "+QString(pc)
              +" baud="+QString::number(br)
              + "parity :"+QString(pa)
              +" #bits="+QString::number(nb)
              +" stopbit="+QString::number(sb)
              );
        modbus_free(m_ctx);
        m_NOTConnected = 2;
        return ;
    }
    //modbus_set_response_timeout(m_ctx, 0, MODBUS_ACCESS_REG_TIMEOUT_MILLISEC);

    int ma = ui->master->currentText().toInt();
    if (modbus_set_slave(m_ctx, ma) == -1) {
        //fprintf(stderr, "modbus_set_slave(%d) failed: %s\n", REMOTE_ID, modbus_strerror(errno));
        TRACE(tr("modbus_set_slave failed : ")+QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_NOTConnected = 3;
        return ;
    }
    ui->watchdogButton->setVisible(1);
    m_NOTConnected = 0;
    ui->connectBtn->setVisible(0);
    ui->sendValuesBtn->setVisible(1);
    ui->statusbar->showMessage("tr(connected)");
    for(uint i=0;i<m_labelsValue.size();i++) {
        m_labelsValue[i]->setVisible(1);
        m_labelsName[i]->setVisible(1);
        m_lineEditsValue[i]->setVisible(1);
    }
    for (int i=0;i<m_nReg;i++)
        m_lineEditsValue[i]->setText(QString::number(m_registerVals[i])); // since updateValueOnGui will send these values
    checkAlive();
    updateValuesOnGui();
    return ;
}     // FIN void MainWindow::connectDevice()
// ***************************************************************************************************************
#endif

void MainWindow::mySetupUi(){
    SETADD; // this macro files the m_vectors to the proper values  FOR EASY ADDING REGISTERS
    m_labelsName = {ui->label,
                                    ui->label_2,
                                    ui->label_3,
                                    ui->label_4,
                                    ui->label_5,
                                    ui->label_6,
                                    ui->label_7,
                                    ui->label_8,
                                    ui->label_9,
                                    ui->label_10,
                                    ui->label_11,
                                    ui->label_12,
                                    ui->label_13,
                                    ui->label_14,
                                    ui->label_15,
                                    ui->label_16};
    m_labelsValue = {ui->label_17,
                                     ui->label_18,
                                     ui->label_19,
                                     ui->label_20,
                                     ui->label_21,
                                     ui->label_22,
                                     ui->label_23,
                                     ui->label_24,
                                     ui->label_25,
                                     ui->label_26,
                                     ui->label_27,
                                     ui->label_28,
                                     ui->label_29,
                                     ui->label_30,
                                     ui->label_31,
                                     ui->label_32};
    m_lineEditsValue = {ui->lineEdit,
                                           ui->lineEdit_2,
                                           ui->lineEdit_3,
                                           ui->lineEdit_4,
                                           ui->lineEdit_5,
                                           ui->lineEdit_6,
                                           ui->lineEdit_7,
                                           ui->lineEdit_8,
                                           ui->lineEdit_9,
                                           ui->lineEdit_10,
                                           ui->lineEdit_11,
                                           ui->lineEdit_12,
                                           ui->lineEdit_13,
                                           ui->lineEdit_14,
                                           ui->lineEdit_15,
                                           ui->lineEdit_16};
    if (m_labelsName.size() != m_nReg) {
        EXIT(tr("number of registers and number of labels different "));
    }
    ui->labelSimul->setVisible(0);
    ui->baudRate->setCurrentIndex(2);
    ui->portCom->setCurrentIndex(2);
    ui->stopBits->setCurrentIndex(1);
    ui->master->setCurrentIndex(3);
    QVector<QString> names;
    m_adds.resize(0);
    m_registerVals.resize(0);
    for (auto i = m_NamesAdd.cbegin(), end = m_NamesAdd.cend(); i != end; ++i) {
        QString name = i.key();
        unsigned int add = i.value();
        names.push_back(name);
        m_adds.push_back(add);
        m_registerVals.push_back(-1);
        m_modifiables.push_back(MODIFIABLES.contains(name));
    }
    for(int i=0;i<m_nReg;i++) {
        m_labelsName[i]->setGeometry(QRect(10,100+i*25,120,60));// left,top,width,heght
        m_labelsName[i]->setText(names[i]);
        m_labelsValue[i]->setGeometry(QRect(140,100+i*25,120,60));
        m_labelsValue[i]->setText(QString::number(m_registerVals[i]));
        m_lineEditsValue[i]->setGeometry(QRect(240,100+20+i*25,100,20));
        m_labelsName[i]->setVisible(0);
        m_labelsValue[i]->setVisible(0);
        m_lineEditsValue[i]->setVisible(0);
        m_lineEditsValue[i]->setValidator(new QIntValidator(this));
        m_lineEditsValue[i]->setEnabled(m_modifiables[i]);
    }

    ui->connectBtn->setGeometry(400-40,200,80,20);

    ui->sendValuesBtn->setGeometry(400,100+20+7*25,140,20);
    connect(ui->sendValuesBtn,&QPushButton::clicked,this,&MainWindow::updateValuesOnGui);
    connect(ui->connectBtn,&QPushButton::clicked,this,&MainWindow::connectDevice);
    ui->sendValuesBtn->setVisible(0);

    ui->watchdogButton->setVisible(0);
    ui->watchdogButton->setGeometry(400+200,100+20+7*25,140,20);
    ui->watchdogButton->setText("turn watchdog on");
    connect(ui->watchdogButton,&QPushButton::clicked,this,&MainWindow::toggleWatchdog);
    m_timer = new QTimer(this);
    m_timer->setInterval(WATCHDOG_TIME_MILLISEC);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::checkAlive);
    m_bidon = 0;
}    // FIN void MainWindow::mySetupUi()
// **********************************************************************************************

void MainWindow::checkAlive() {
    uint16_t dest=123;
    int k = modbus_read_registers(m_ctx,ALIVE_ADDRESS,1,&dest);
    if (k != 1) {
        EXIT("Version register "+QString::number(ALIVE_ADDRESS)+" is unaccessible< Probably the board is not powered");
    }
    if (dest != ALIVE_VALUE) {
        EXIT("Unexpected value "+QString::number(dest)+ " instead of "+QString::number(ALIVE_VALUE)+" in version Register "+QString::number(ALIVE_ADDRESS));
    }
}    // FIN void MainWindow::checkAlive()
// **********************************************************************************************

void MainWindow::toggleWatchdog() {
    if (ui->watchdogButton->text() == "turn watchdog on") {
        ui->watchdogButton->setText("turn watchdog off");
        m_timer->start();
    } else {
        ui->watchdogButton->setText("turn watchdog on");
        m_timer->stop();
    }
}    // FIN void MainWindow::toggleWatchdog()
// **********************************************************************************************


void MainWindow::updateValuesOnGui() {
    // Writing board's registers
    for (int i=0;i<m_nReg;i++)
        m_valuesToSend[i] =  m_lineEditsValue[i]->text().toInt();
    setValues();
    // Reading board registers
    getValues();
    // Checking consistency
    for(int i=0;i<m_nReg;i++)
        if (m_registerVals[i] != m_valuesToSend[i])
            TRACE("Re-reading error with register "+QString::number(i));
    // Showing new values
    for (int i=0;i<m_nReg;i++) {
        m_labelsValue[i]->setText(QString::number(m_registerVals[i]));
    }
    return;
}      // FIN void MainWindow::updateValuesOnGui() {
// ***********************************************************************************************
