//
// Created by Denis Wagner on 5/22/19.
//

#include <include/Utils/Config.hpp>
#include <QtCore/QtCore>

Config* Config::configSingleton;
QJsonObject Config::jsonConfig;

Config::Config()
{
    QFile configFile( "../config.json" );

    if ( !configFile.open( QIODevice::ReadOnly ) )
    {
        qWarning() << "Config file \"config.json\" can't be opened.";
    }
    else
    {
        QJsonDocument jsonDoc( QJsonDocument::fromJson( configFile.readAll() ) );
        jsonConfig = std::move( jsonDoc.object() );
    }

    applyConfig();

    connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( saveConfig() ) );
}

void Config::init()
{
    if ( !configSingleton )
        configSingleton = new Config();
}

void Config::applyConfig()
{
    applyIfNotPresent( "prefix", "zeitgeist" );
    applyIfNotPresent( "sampleRate", 16000 );
    applyIfNotPresent( "noSkillReply", "Entschuldigung, das habe ich nicht verstanden." );
    applyIfNotPresent( "mbrolaVoice", "mb-de5" );
}

void Config::applyIfNotPresent( const QString& key, const QJsonValue& value )
{
    if ( !jsonConfig.contains( key ) )
        jsonConfig.insert( key, value );
}

void Config::saveConfig()
{
    QFile configFile( "../config.json" );

    if ( !configFile.open( QIODevice::WriteOnly ) )
    {
        qWarning() << "Couldn't write to \"config.json\".";
        return;
    }

    configFile.write( QJsonDocument( jsonConfig ).toJson() );
}


// ********************* //
// *** Getter/Setter *** //
// ********************* //

std::string Config::getPrefix()
{
    return jsonConfig["prefix"].toString().toStdString();
}

void Config::setPrefix( const std::string& prefix )
{
    jsonConfig["prefix"] = prefix.c_str();
}

int Config::getSampleRate()
{
    return jsonConfig["sampleRate"].toInt();
}

void Config::setSampleRate( int sampleRate )
{
    jsonConfig["sampleRate"] = sampleRate;

}

std::string Config::getMbrolaVoice()
{
    return jsonConfig["mbrolaVoice"].toString().toStdString();
}

void Config::setMbrolaVoice( const std::string& mbrolaVoice )
{
    jsonConfig["mbrolaVoice"] = mbrolaVoice.c_str();
}

std::string Config::getNoSkillReply()
{
    return jsonConfig["noSkillReply"].toString().toStdString();
}

void Config::setNoSkillReply( const std::string& noSkillReply )
{
    jsonConfig["noSkillReply"] = noSkillReply.c_str();
}

