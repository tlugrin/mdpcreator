/*
 *  MDPcreator : Password generator utility
 *  Copyright (C) 2016-2021   Thomas Lugrin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 *  On Debian systems, the complete text of the GNU General
 *  Public License version 2 can be found in "/usr/share/common-licenses/GPL-2".
 */

#include <QApplication>
#include "MDPWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MDPWindow mdpWindow;

    mdpWindow.show();

    return app.exec();
}
