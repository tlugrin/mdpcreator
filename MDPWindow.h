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

#ifndef MDPWINDOW_H
#define MDPWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include <QStringList>
#include <QMenu>
#include <QAction>
#include <QtMath>

class MDPWindow : public QMainWindow
{
    Q_OBJECT

    private:
        QVector<QStringList> charLists;
        QVector<unsigned int> cumulativeListSize;
        QLabel* mdpSecurityLabel;
        QComboBox* mdpSecurity;
        QLabel* mdpLengthLabel;
        QSpinBox* mdpLength;
        QGroupBox* mdpLanguage;
        QRadioButton* chooseFrench;
        QRadioButton* chooseEnglish;
        QRadioButton* chooseGerman;
        QPushButton* generateMdpButton;
        QLabel* resultingMdp;
        QLineEdit* resultingMdpEdit;
        QLabel* resultingAttempts;
        QLabel* resultingTimeToBreak;
        QLabel* resultingBitEntropy;
        QLabel* resultingAssessment;
        // number of passwords a powerful attacker could supposedly try per second
        const unsigned int PASSWORDS_PER_SEC=qPow(10,12);
        // number of lines in French, English and German wordlists
        unsigned int MAX_FRENCH;
        unsigned int MAX_ENGLISH;
        unsigned int MAX_GERMAN;
        // menu and sub-menus
        QMenu* aboutMenu;
        QAction* aboutMDPCreatorAct;
        QAction* aboutQtAct;

        // help-functions at initialisation
        void initialiseCharLists();
        unsigned int countNumberOfWords(QString const &file) const;
        unsigned int findCharClass(const unsigned int &i) const;
        void initialiseMenuBar();

    protected slots:
        void generateMdp();
        void updateSecurityInfos();
        void changeMode(int index);
        // menu actions
        void aboutMDPCreator();
        void aboutQt();

    public:
        MDPWindow(QWidget* parent =0);
};

#endif // MDPWINDOW_H
