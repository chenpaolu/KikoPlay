#ifndef EMBYCLIENT_H
#define EMBYCLIENT_H

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>
#include "Common/network.h"

class EmbyClient
{
public:
    struct Session
    {
        QString serverUrl;
        QString accessToken;
        QString userId;
        QString userName;
        QString serverId;
    };

    struct Result
    {
        bool success{false};
        int statusCode{0};
        QString error;
        QJsonObject object;
        QJsonArray array;
    };

    explicit EmbyClient(const QString &serverUrl = QString());

    void setServerUrl(const QString &serverUrl);
    QString serverUrl() const { return m_serverUrl; }

    void setAccessToken(const QString &token) { m_accessToken = token; }
    QString accessToken() const { return m_accessToken; }

    void setUserId(const QString &userId) { m_userId = userId; }
    QString userId() const { return m_userId; }

    void clearSession();

    Result authenticateByName(const QString &userName, const QString &password);
    Result systemInfo() const;
    Result publicSystemInfo() const;
    Result views(const QString &userId = QString()) const;
    Result items(const QString &parentId = QString(), const QUrlQuery &extraQuery = QUrlQuery(), const QString &userId = QString()) const;
    Result item(const QString &itemId, const QString &userId = QString()) const;
    Result playbackInfo(const QString &itemId, const QString &userId = QString()) const;

    QUrl streamUrl(const QString &itemId, const QString &mediaSourceId = QString(), bool staticStream = false) const;

    static QString normalizeServerUrl(const QString &serverUrl);
    static QString authorizationHeaderValue(const QString &token = QString());
    static QUrl makeUrl(const QString &serverUrl, const QString &path, const QUrlQuery &query = QUrlQuery());

private:
    Result parseReply(const Network::Reply &reply, bool arrayResponse = false) const;
    QStringList headers(bool json = false) const;
    QString effectiveUserId(const QString &userId) const;

    QString m_serverUrl;
    QString m_accessToken;
    QString m_userId;
};

#endif // EMBYCLIENT_H
