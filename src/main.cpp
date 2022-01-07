//
// Created by Denis Wagner on 4/11/19.
//

#include <include/ConversationManager/ConversationManager.hpp>
#include <QtWidgets/QApplication>
#include <include/Utils/Config.hpp>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    app.setApplicationName( "personal-assistant" );

    Config::init();
    ConversationManager conversationManager;

    return app.exec();
}
