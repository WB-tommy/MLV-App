#include "cupdaterdialog.h"

#include "ui_cupdaterdialog.h"

#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>
#include <QStringBuilder>
#include <QScrollBar>
#include <QApplication>
#include <QScreen>
#include "maddy/parser.h"
#include <memory>
#include <string>

CUpdaterDialog::CUpdaterDialog(QWidget *parent, const QString& githubRepoAddress, const QString& versionString, bool silentCheck) :
	QDialog(parent),
	ui(new Ui::CUpdaterDialog),
	_silent(silentCheck),
    _updater(this, QUrl(githubRepoAddress), versionString)
{
	ui->setupUi(this);
    setAttribute( Qt::WA_NativeWindow, true );
    setAttribute( Qt::WA_AcceptTouchEvents, true );
    setAttribute( Qt::WA_LayoutOnEntireRect, true );
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

	if (_silent)
		hide();

	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CUpdaterDialog::applyUpdate);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Download");

	ui->stackedWidget->setCurrentIndex(0);
	ui->progressBar->setMaximum(0);
	ui->progressBar->setValue(0);
	ui->lblPercentage->setVisible(false);

    QTimer::singleShot( 1, this, &CUpdaterDialog::checkUpdate );
}

CUpdaterDialog::~CUpdaterDialog()
{
    delete ui;
}

void CUpdaterDialog::checkUpdate( void )
{
    if( !_updater.isUpdateAvailable() )
    {
        accept();
        if (!_silent)
            QMessageBox::information(this, tr("No update available"), tr("You already have the latest version of MLV App."));
        return;
    }

    ui->stackedWidget->setCurrentIndex(1);
    QString text;
    for (const auto& changelogItem: _updater.getUpdateChangelog())
    {
        text.append("<b>" % changelogItem.versionString % "</b>" % "\r\n\r\n" % changelogItem.versionChanges % "<p></p>" % "\r\n\r\n" );
    }
    //ui->changeLogViewer->setMarkdown( text ); //Just for Qt5.14+
    // ////
    //Use Maddy parser Markdown->Html
    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    config->isEmphasizedParserEnabled = true; // default
    config->isHTMLWrappedInParagraph = true; // default
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
    std::stringstream markdownInput( text.toStdString() );
    std::string htmlOutput = parser->Parse( markdownInput );
    // ////
    ui->changeLogViewer->setText( QString::fromStdString( htmlOutput ) );

    _latestUpdateUrl = _updater.getUpdateChangelog().front().versionUpdateUrl;
    QScrollBar *scrollbar = ui->changeLogViewer->verticalScrollBar();
    scrollbar->setSliderPosition(0);
    // Center and align to device pixels to avoid fractional-offset hit regions
    QTimer::singleShot(0, this, [this]{
        QWidget* pw = parentWidget();
        QRect ref = pw ? pw->frameGeometry() : QApplication::primaryScreen()->availableGeometry();
        QPoint center = ref.center();
        QPoint desired = center - QPoint(width()/2, height()/2);
        const qreal dpr = this->devicePixelRatioF();
        QPointF phys(desired.x()*dpr, desired.y()*dpr);
        QPointF physRound(qRound(phys.x()), qRound(phys.y()));
        QPoint logical(qRound(physRound.x()/dpr), qRound(physRound.y()/dpr));
        move(logical);
        show();
    });
}

void CUpdaterDialog::applyUpdate()
{
    QDesktopServices::openUrl(QUrl(_updater.getDownloadUrl()));
}
