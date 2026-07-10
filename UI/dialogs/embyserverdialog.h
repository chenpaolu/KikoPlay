#ifndef EMBYSERVERDIALOG_H
#define EMBYSERVERDIALOG_H

#include "UI/framelessdialog.h"

class QCheckBox;
class QLineEdit;

struct EmbyServerConfig
{
    QString title;
    QString url;
    QString user;
    QString password;
    QString apiKey;

    bool useApiKey() const { return !apiKey.trimmed().isEmpty(); }
};

class EmbyServerDialog : public CFramelessDialog
{
    Q_OBJECT
public:
    explicit EmbyServerDialog(QWidget *parent = nullptr);

    const EmbyServerConfig &serverConfig() const { return config; }

protected:
    void onAccept() override;

private:
    void updateAuthMode(bool apiKeyMode);

    EmbyServerConfig config;
    QLineEdit *titleEdit{nullptr};
    QLineEdit *urlEdit{nullptr};
    QLineEdit *userEdit{nullptr};
    QLineEdit *passwordEdit{nullptr};
    QLineEdit *apiKeyEdit{nullptr};
    QCheckBox *apiKeyCheck{nullptr};
};

#endif // EMBYSERVERDIALOG_H
