#include "embyserverdialog.h"
#include "MediaServer/Emby/embyclient.h"
#include "UI/ela/ElaCheckBox.h"
#include "UI/ela/ElaLineEdit.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QUrl>

EmbyServerDialog::EmbyServerDialog(QWidget *parent)
    : CFramelessDialog(tr("Add Emby Server"), parent, true)
{
    titleEdit = new ElaLineEdit(this);
    titleEdit->setPlaceholderText(tr("Server name"));

    urlEdit = new ElaLineEdit(this);
    urlEdit->setPlaceholderText(tr("Example: http://192.168.1.2:8096"));

    apiKeyCheck = new ElaCheckBox(tr("Use API key"), this);

    userEdit = new ElaLineEdit(this);
    userEdit->setPlaceholderText(tr("User name"));

    passwordEdit = new ElaLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("Password"));

    apiKeyEdit = new ElaLineEdit(this);
    apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyEdit->setPlaceholderText(tr("API key"));

    QGridLayout *inputLayout = new QGridLayout(this);
    inputLayout->addWidget(new QLabel(tr("Title"), this), 0, 0);
    inputLayout->addWidget(titleEdit, 0, 1, 1, 3);
    inputLayout->addWidget(new QLabel(tr("URL"), this), 1, 0);
    inputLayout->addWidget(urlEdit, 1, 1, 1, 3);
    inputLayout->addWidget(apiKeyCheck, 2, 0, 1, 4);
    inputLayout->addWidget(new QLabel(tr("User"), this), 3, 0);
    inputLayout->addWidget(userEdit, 3, 1);
    inputLayout->addWidget(new QLabel(tr("Password"), this), 3, 2);
    inputLayout->addWidget(passwordEdit, 3, 3);
    inputLayout->addWidget(new QLabel(tr("API Key"), this), 4, 0);
    inputLayout->addWidget(apiKeyEdit, 4, 1, 1, 3);
    inputLayout->setColumnStretch(1, 1);
    inputLayout->setColumnStretch(3, 1);

    QObject::connect(apiKeyCheck, &QCheckBox::toggled, this, &EmbyServerDialog::updateAuthMode);
    updateAuthMode(false);

    setSizeSettingKey("EmbyServerDialog", QSize(420, 150));
}

void EmbyServerDialog::onAccept()
{
    config.title = titleEdit->text().trimmed();
    config.url = EmbyClient::normalizeServerUrl(urlEdit->text());
    config.user = userEdit->text().trimmed();
    config.password = passwordEdit->text();
    config.apiKey = apiKeyCheck->isChecked() ? apiKeyEdit->text().trimmed() : QString();

    if (config.title.isEmpty() || config.url.isEmpty())
    {
        showMessage(tr("Title/URL can't be empty"), NM_ERROR | NM_HIDE);
        return;
    }

    const QUrl serverUrl(config.url);
    if (!serverUrl.isValid() || serverUrl.scheme().isEmpty() || serverUrl.host().isEmpty())
    {
        showMessage(tr("Server URL is invalid"), NM_ERROR | NM_HIDE);
        return;
    }

    if (config.useApiKey())
    {
        if (config.apiKey.isEmpty())
        {
            showMessage(tr("API key can't be empty"), NM_ERROR | NM_HIDE);
            return;
        }
    }
    else if (config.user.isEmpty() || config.password.isEmpty())
    {
        showMessage(tr("User/Password can't be empty"), NM_ERROR | NM_HIDE);
        return;
    }

    CFramelessDialog::onAccept();
}

void EmbyServerDialog::updateAuthMode(bool apiKeyMode)
{
    userEdit->setEnabled(!apiKeyMode);
    passwordEdit->setEnabled(!apiKeyMode);
    apiKeyEdit->setEnabled(apiKeyMode);
}
