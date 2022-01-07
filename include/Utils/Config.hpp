//
// Created by Denis Wagner on 5/21/19.
//

#ifndef PERSONAL_VOICE_ASSISTANT_CONFIG_HPP
#define PERSONAL_VOICE_ASSISTANT_CONFIG_HPP

#include <QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

class Config : public QObject
{
    static Config* configSingleton;
    static QJsonObject jsonConfig;

Q_OBJECT

private slots:
    void saveConfig();

public:
    static void init();

    static std::string getPrefix();
    static void setPrefix( const std::string& prefix );
    static int getSampleRate();
    static void setSampleRate( int sampleRate );
    static std::string getMbrolaVoice();
    static void setMbrolaVoice( const std::string& mbrolaVoice );
    static std::string getNoSkillReply();
    static void setNoSkillReply( const std::string& noSkillReply );

private:
    Config();
    static void applyConfig();
    static void applyIfNotPresent( const QString& key, const QJsonValue& value );
};

#endif //PERSONAL_VOICE_ASSISTANT_CONFIG_HPP
