#include "embyclient.h"
#include "globalobjects.h"
#include <QJsonDocument>
#include <QJsonParseError>

namespace
{
constexpr const char *kClientName = "KikoPlay";
constexpr const char *kDeviceName = "KikoPlay";
constexpr const char *kDeviceId = "kikoplay-desktop";

QString cleanPath(QString path)
{
    while (path.startsWith('/')) path.remove(0, 1);
    return path;
}
}

EmbyClient::EmbyClient(const QString &serverUrl)
{
    setServerUrl(serverUrl);
}

void EmbyClient::setServerUrl(const QString &serverUrl)
{
    m_serverUrl = normalizeServerUrl(serverUrl);
}

void EmbyClient::clearSession()
{
    m_accessToken.clear();
    m_userId.clear();
}

EmbyClient::Result EmbyClient::authenticateByName(const QString &userName, const QString &password)
{
    QJsonObject payload;
    payload.insert("Username", userName);
    payload.insert("Pw", password);

    Network::Reply reply = Network::httpPost(makeUrl(m_serverUrl, "Users/AuthenticateByName").toString(),
                                             QJsonDocument(payload).toJson(QJsonDocument::Compact),
                                             headers(true));
    Result result = parseReply(reply);
    if (result.success)
    {
        const QJsonObject user = result.object.value("User").toObject();
        m_accessToken = result.object.value("AccessToken").toString();
        m_userId = user.value("Id").toString();
    }
    return result;
}

EmbyClient::Result EmbyClient::systemInfo() const
{
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, "System/Info").toString(), QUrlQuery(), headers()));
}

EmbyClient::Result EmbyClient::publicSystemInfo() const
{
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, "System/Info/Public").toString(), QUrlQuery(), headers()));
}

EmbyClient::Result EmbyClient::views(const QString &userId) const
{
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, QString("Users/%1/Views").arg(effectiveUserId(userId))).toString(),
                                       QUrlQuery(), headers()));
}

EmbyClient::Result EmbyClient::items(const QString &parentId, const QUrlQuery &extraQuery, const QString &userId) const
{
    QUrlQuery query(extraQuery);
    if (!parentId.isEmpty()) query.addQueryItem("ParentId", parentId);
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, QString("Users/%1/Items").arg(effectiveUserId(userId)), query).toString(),
                                       QUrlQuery(), headers()));
}

EmbyClient::Result EmbyClient::item(const QString &itemId, const QString &userId) const
{
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, QString("Users/%1/Items/%2").arg(effectiveUserId(userId), itemId)).toString(),
                                       QUrlQuery(), headers()));
}

EmbyClient::Result EmbyClient::playbackInfo(const QString &itemId, const QString &userId) const
{
    QUrlQuery query;
    const QString uid = effectiveUserId(userId);
    if (!uid.isEmpty()) query.addQueryItem("UserId", uid);
    return parseReply(Network::httpGet(makeUrl(m_serverUrl, QString("Items/%1/PlaybackInfo").arg(itemId), query).toString(),
                                       QUrlQuery(), headers()));
}

QUrl EmbyClient::streamUrl(const QString &itemId, const QString &mediaSourceId, bool staticStream) const
{
    QUrlQuery query;
    if (!mediaSourceId.isEmpty()) query.addQueryItem("MediaSourceId", mediaSourceId);
    if (staticStream) query.addQueryItem("Static", "true");
    return makeUrl(m_serverUrl, QString("Videos/%1/stream").arg(itemId), query);
}

QString EmbyClient::normalizeServerUrl(const QString &serverUrl)
{
    QString normalized = serverUrl.trimmed();
    while (normalized.endsWith('/')) normalized.chop(1);
    return normalized;
}

QString EmbyClient::authorizationHeaderValue(const QString &token)
{
    QString value = QString("MediaBrowser Client=\"%1\", Device=\"%2\", DeviceId=\"%3\", Version=\"%4\"")
            .arg(kClientName, kDeviceName, kDeviceId, QString::fromUtf8(GlobalObjects::kikoVersion));
    if (!token.isEmpty()) value += QString(", Token=\"%1\"").arg(token);
    return value;
}

QUrl EmbyClient::makeUrl(const QString &serverUrl, const QString &path, const QUrlQuery &query)
{
    QUrl url(normalizeServerUrl(serverUrl) + '/' + cleanPath(path));
    if (!query.isEmpty()) url.setQuery(query);
    return url;
}

EmbyClient::Result EmbyClient::parseReply(const Network::Reply &reply, bool arrayResponse) const
{
    Result result;
    result.statusCode = reply.statusCode;
    if (reply.hasError)
    {
        result.error = reply.errInfo;
        return result;
    }
    if (reply.statusCode < 200 || reply.statusCode >= 300)
    {
        result.error = QString("HTTP %1").arg(reply.statusCode);
        return result;
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(reply.content, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        result.error = parseError.errorString();
        return result;
    }
    if (arrayResponse)
    {
        if (!document.isArray())
        {
            result.error = QStringLiteral("Expected JSON array response");
            return result;
        }
        result.array = document.array();
    }
    else
    {
        if (!document.isObject())
        {
            result.error = QStringLiteral("Expected JSON object response");
            return result;
        }
        result.object = document.object();
    }
    result.success = true;
    return result;
}

QStringList EmbyClient::headers(bool json) const
{
    QStringList values;
    values << "X-Emby-Authorization" << authorizationHeaderValue(m_accessToken);
    if (!m_accessToken.isEmpty()) values << "X-Emby-Token" << m_accessToken;
    if (json) values << "Content-Type" << "application/json";
    return values;
}

QString EmbyClient::effectiveUserId(const QString &userId) const
{
    return userId.isEmpty() ? m_userId : userId;
}
