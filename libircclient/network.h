//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef NETWORK_H
#define NETWORK_H

#include "../libirc/network.h"
#include "user.h"
#include "mode.h"
#include <QList>
#include <QString>
#include <QDateTime>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QTimer>
#include "libircclient_global.h"
#include "../libirc/irc_standards.h"

class QTcpSocket;

namespace libirc
{
    class ServerAddress;
}

namespace libircclient
{
    enum Encoding
    {
        EncodingUTF8
    };

    class Server;
    class Channel;
    class Parser;

    class LIBIRCCLIENTSHARED_EXPORT Network : public libirc::Network
    {
        Q_OBJECT

        public:
            Network(libirc::ServerAddress &server, QString name);
            Network(QHash<QString, QVariant> hash);
            virtual ~Network();
            void Connect();
            void Reconnect();
            void Disconnect(QString reason = "");
            bool IsConnected();
            //! This function can be used to change the default nickname that will be requested upon connection to server
            //! subsequent calls of this function while on active IRC connection will be ignored.
            void SetDefaultNick(QString nick);
            void SetDefaultIdent(QString ident);
            void SetDefaultUsername(QString realname);
            QString GetNick();
            QString GetHost();
            QString GetIdent();
            QString GetServerAddress();
            //! This will update the nick in operating memory, it will not request it from server and may cause troubles
            //! if not properly called. This is only used for resynchronization.
            void SetNick(QString nick);
            void SetPassword(QString Password);
            void TransferRaw(QString raw);
            int SendMessage(QString text, Channel *channel);
            int SendMessage(QString text, User *user);
            int SendMessage(QString text, QString target);
            int GetTimeout() const;
            void RequestPart(QString channel_name);
            void RequestPart(Channel *channel);
            void RequestNick(QString nick);
            void Identify(QString Nickname = "", QString Password = "");
            bool ContainsChannel(QString channel_name);
            //////////////////////////////////////////////////////////////////////////////////////////
            // Synchronization tools
            //! This will delete all internal memory structures related to channels this user is in.
            //! Use only for synchronization purposes, while implementing something like grumpyd
            //! calling this function on live IRC network connection will have unpredictable result
            void _st_ClearChannels();
            /*!
             * \brief InsertChannel Inserts a deep copy of provided channel to local list of channels
             * \param channel Source which is copied, can be safely deleted
             * \return The copy of input object which was inserted to local vector of channels same as result of GetChannel
             */
            Channel *_st_InsertChannel(libircclient::Channel *channel);
            //////////////////////////////////////////////////////////////////////////////////////////
            Channel *GetChannel(QString channel_name);
            QList<Channel *> GetChannels();
            User *GetLocalUserInfo();
            /*!
             * \brief StartsWithCUPrefix checks the user name whether it starts with a CUMode prefix (such as @)
             * \param user_name Name of user including the prefix (@channel_op)
             * \return 0 in case there is no prefix, otherwise it returns the respective CUMode (o in case of @)
             */
            char StartsWithCUPrefix(QString user_name);
            /*!
             * \brief PositionOfChannelPrefix returns a position of UCP or negative number in case it's not in there
             * \param prefix
             * \return
             */
            int PositionOfUCPrefix(char prefix);
            void SetChannelUserPrefixes(QList<char> data);
            void SetCModes(QList<char> data);
            QList<char> GetChannelUserPrefixes();
            QList<char> GetCModes();
            QList<char> GetCPModes();
            void SetCPModes(QList<char> data);
            void SetCRModes(QList<char> data);
            QList<char> GetCCModes();
            QList<char> GetCRModes();
            void SetCUModes(QList<char> data);
            QList<char> GetCUModes();
            void SetCCModes(QList<char> data);
            void LoadHash(QHash<QString, QVariant> hash);
            QHash<QString, QVariant> ToHash();
            bool ResolveOnNickConflicts;
            QHash<char, QString> ChannelModeHelp;
            QHash<char, QString> UserModeHelp;

        signals:
            void Event_RawOutgoing(QByteArray data);
            void Event_RawIncoming(QByteArray data);
            void Event_Invalid(QByteArray data);
            void Event_ConnectionFailure(QAbstractSocket::SocketError reason);
            void Event_Parse(libircclient::Parser *parser);
            void Event_SelfJoin(libircclient::Channel *chan);
            void Event_Join(libircclient::Parser *parser, libircclient::User *user, libircclient::Channel *chan);
            /*!
             * \brief Event_PerChannelQuit Emitted when a user quit the network for every single channel this user was in
             *                             so that it's extremely simple to render the information in related scrollbacks
             * \param parser   Pointer to parser of IRC raw message
             * \param chan     Pointer to channel this user just left
             */
            void Event_PerChannelQuit(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_Quit(libircclient::Parser *parser);
            //! Emitted before the channel is removed from memory on part of a channel you were in
            void Event_SelfPart(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_Part(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_SelfKick(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_Kick(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_ServerMode(libircclient::Parser *parser);
            void Event_ChannelMode(libircclient::Parser *parser);
            void Event_MOTDEnd(libircclient::Parser *parser);
            void Event_MOTDBegin(libircclient::Parser *parser);
            void Event_MOTD(libircclient::Parser *parser);
            void Event_Mode(libircclient::Parser *parser);
            void Event_NickCollision(libircclient::Parser *parser);
            void Event_WhoisInfo(libircclient::Parser *parser);
            void Event_INFO(libircclient::Parser *parser);
            void Event_PRIVMSG(libircclient::Parser *parser);
            void Event_EndOfNames(libircclient::Parser *parser);
            void Event_NOTICE(libircclient::Parser *parser);
            void Event_NICK(libircclient::Parser *parser, QString old_nick, QString new_nick);
            void Event_SelfNICK(libircclient::Parser *parser, QString old_nick, QString new_nick);
            //! IRC_NUMERIC_MYINFO
            void Event_MyInfo(libircclient::Parser *parser);
            //! Emitted when someone changes the topic
            void Event_TOPIC(libircclient::Parser *parser, libircclient::Channel * chan, QString old_topic);
            //! Retrieved after channel is joined as part of info
            void Event_TOPICInfo(libircclient::Parser *parser, libircclient::Channel *chan);
            void Event_TOPICWhoTime(libircclient::Parser *parser, libircclient::Channel *chan);
            //! Server gave us some unknown command
            void Event_Unknown(libircclient::Parser *parser);
            void Event_Timeout();
            void Event_Connected();
            void Event_Disconnected();

        private slots:
            void OnPing();
            void OnPingSend();
            void OnError(QAbstractSocket::SocketError er);
            void OnReceive();
            void OnConnected();

        protected:
            bool usingSSL;
            QTcpSocket *socket;
            QString hostname;
            unsigned int port;
            int pingTimeout;
            int pingRate;
            QString defaultQuit;
            bool autoRejoin;
            bool autoIdentify;
            QString identifyString;
            QString password;

        private:
            void processIncomingRawData(QByteArray data);
            void processNamrpl(Parser *parser);
            void process433(Parser *parser);
            void processInfo(Parser *parser);
            void processNick(Parser *parser, bool self_command);
            void deleteTimers();
            //! List of symbols that are used to prefix users
            QList<char> channelUserPrefixes;
            QList<char> CModes;
            QList<char> CPModes;
            QList<char> CRModes;
            QList<char> CUModes;
            QList<char> CCModes;
            QString originalNick;
            UMode localUserMode;
            QString alternateNick;
            int alternateNickNumber;
            char channelPrefix;
            Server *server;
            QList<User*> users;
            QList<Channel*> channels;
            User localUser;
            QDateTime lastPing;
            QTimer *timerPingTimeout;
            QTimer *timerPingSend;
    };
}

#endif // NETWORK_H
