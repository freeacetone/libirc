//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#ifndef SERVER_H
#define SERVER_H

#include "libirc_global.h"
#include <QString>

namespace libirc
{
    class LIBIRCSHARED_EXPORT Server
    {
        public:
            Server();
            virtual ~Server() {}

        protected:
            QString _name;
            QString _host;
            unsigned int _port;
            bool _ssl;
    };
}

#endif // SERVER_H
