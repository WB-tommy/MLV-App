#include "StatusFpmDialog.h"
#include "ui_StatusFpmDialog.h"

StatusFpmDialog::StatusFpmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusFpmDialog)
{
    ui->setupUi(this);
    // Use QDialog flags for proper input focus on Android
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint );
}

StatusFpmDialog::~StatusFpmDialog()
{
    delete ui;
}
