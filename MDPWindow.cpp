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

#include "MDPWindow.h"
#include <cstdlib>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QtCore/qmath.h>
#include <QTextStream>
#include <QFile>
#include <QMenuBar>
#include <QMessageBox>
#include <QRandomGenerator>

MDPWindow::MDPWindow(QWidget* parent) : QMainWindow(parent)
{
    QTime time(QTime::currentTime());
    qsrand(time.msec());

    initialiseCharLists();
    MAX_FRENCH = countNumberOfWords(":/french_dict.txt");
    MAX_ENGLISH = countNumberOfWords(":/english_dict.txt");
    MAX_GERMAN = countNumberOfWords(":/german_dict.txt");

    this->setWindowTitle("MDPCreator 2.2");
    this->setWindowIcon(QIcon(":/icon.jpg"));
    this->setMinimumSize(800,250);
    QWidget* widget = new QWidget;
    this->setCentralWidget(widget);
    initialiseMenuBar();

    // drop-down list: 'level of security'
    QVBoxLayout* securityLayout = new QVBoxLayout;
    mdpSecurityLabel = new QLabel(tr("Jeu de caractères/dictionnaire :"), this);
    mdpSecurity = new QComboBox(this);
    QIcon lowIcon(":/low.png");
    QIcon midIcon(":/mid.png");
    QIcon highIcon(":/high.png");
    QIcon veryHighIcon(":/vhigh.png");
    QIcon veryHighEasyIcon(":/vhigheasy.png");
    mdpSecurity->addItem(lowIcon, tr("Restreint"));
    mdpSecurity->addItem(midIcon, tr("Moyen"));
    mdpSecurity->addItem(highIcon, tr("Grand"));
    mdpSecurity->addItem(veryHighIcon, tr("Très grand"));
    mdpSecurity->addItem(veryHighEasyIcon, tr("Immense et facile à retenir"));
    mdpSecurity->setIconSize(QSize(48,48));
    securityLayout->addWidget(mdpSecurityLabel);
    securityLayout->addWidget(mdpSecurity);

    // spin box: length of password
    QVBoxLayout* lengthLayout = new QVBoxLayout;
    mdpLengthLabel = new QLabel(tr("Longueur du mot de passe :"), this);
    mdpLength = new QSpinBox(this);
    lengthLayout->addWidget(mdpLengthLabel);
    lengthLayout->addWidget(mdpLength);

    // if password with words: choose French or English dictionary
    mdpLanguage = new QGroupBox(tr("Langue"));
    chooseFrench  = new QRadioButton(tr("Français"));
    chooseEnglish = new QRadioButton(tr("Anglais"));
    chooseGerman = new QRadioButton(tr("Allemand"));
    chooseFrench->setChecked(true);
    QVBoxLayout *languageLayout = new QVBoxLayout;
    languageLayout->addWidget(chooseFrench);
    languageLayout->addWidget(chooseEnglish);
    languageLayout->addWidget(chooseGerman);
    mdpLanguage->setLayout(languageLayout);

    // button + text field: generate and view password
    generateMdpButton = new QPushButton(tr("Générer !"));
    generateMdpButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    changeMode(0);

    QVBoxLayout* resultingMdpLayout = new QVBoxLayout;
    resultingMdp = new QLabel(tr("Mot de passe généré :"), this);
    resultingMdpEdit = new QLineEdit(this);
    resultingMdpEdit->setReadOnly(true);
    resultingMdpLayout->addWidget(resultingMdp);
    resultingMdpLayout->addWidget(resultingMdpEdit);

    // infos on security level of generated password
    QFrame* infosFrame = new QFrame;
    infosFrame->setFrameShape(QFrame::Panel);
    infosFrame->setFrameShadow(QFrame::Sunken);
    infosFrame->setLineWidth(2);
    infosFrame->setMidLineWidth(2);
    QGridLayout* infosSplitLayout = new QGridLayout;
    // fixed labels and dynamically updated infos
    QLabel* attemptsLabel = new QLabel(tr("Nombre moyen de tentatives :") + " ");
    QLabel* timeToBreakLabel = new QLabel(tr("Durée moyenne d'une attaque :") + " ");
    QLabel* bitEntropyLabel = new QLabel(tr("Entropie :") + " ");
    QLabel* assessmentLabel = new QLabel(tr("Qualité : ") + " ");
    resultingAttempts = new QLabel("     ");
    resultingTimeToBreak = new QLabel("     ");
    resultingBitEntropy = new QLabel("");
    resultingAssessment = new QLabel("");
    infosSplitLayout->addWidget(attemptsLabel,0,0);
    infosSplitLayout->addWidget(resultingAttempts,0,1);
    infosSplitLayout->addWidget(timeToBreakLabel,1,0);
    infosSplitLayout->addWidget(resultingTimeToBreak,1,1);
    infosSplitLayout->addWidget(bitEntropyLabel,0,3);
    infosSplitLayout->addWidget(resultingBitEntropy,0,4);
    infosSplitLayout->addWidget(assessmentLabel,1,3);
    infosSplitLayout->addWidget(resultingAssessment,1,4);
    infosSplitLayout->setColumnMinimumWidth(2,50);
    infosFrame->setLayout(infosSplitLayout);

    QGridLayout* generalLayout = new QGridLayout;
    generalLayout->addLayout(securityLayout,0,0,1,2);
    generalLayout->addLayout(lengthLayout,1,0,1,1,Qt::AlignTop);
    generalLayout->addWidget(mdpLanguage,1,1,1,1);
    generalLayout->addWidget(generateMdpButton,0,2,2,1);
    generalLayout->addLayout(resultingMdpLayout,2,0,1,3);
    generalLayout->addWidget(infosFrame,3,0,1,3);
    widget->setLayout(generalLayout);

    connect(generateMdpButton,SIGNAL(clicked()),this,SLOT(generateMdp()));
    connect(generateMdpButton,SIGNAL(clicked()),this,SLOT(updateSecurityInfos()));
    connect(mdpSecurity,SIGNAL(currentIndexChanged(int)),this,SLOT(changeMode(int)));
}

void MDPWindow::generateMdp()
{
    QString mdp;
    QRandomGenerator* gen = QRandomGenerator::system();// uses /dev/urandom on Linux
    // password made of words
    if (mdpSecurity->currentIndex() == 4) {
        QFile dict;
        double unif=0;
        if (chooseFrench->isChecked()) {
            dict.setFileName(":/french_dict.txt");
        } else if (chooseEnglish->isChecked()){
            dict.setFileName(":/english_dict.txt");
        } else {
            dict.setFileName(":/german_dict.txt");
        }
        if (dict.open(QFile::ReadOnly)) {
            QTextStream in(&dict);
            QVector<int> lines(mdpLength->value());
            unsigned int max_words;
            if (chooseFrench->isChecked()) {
                max_words = MAX_FRENCH;
            } else if (chooseEnglish->isChecked()) {
                max_words = MAX_ENGLISH;
            } else {
                max_words = MAX_GERMAN;
            }
            for (int i=0; i<lines.size(); i++) {
                unif = gen->generateDouble();// draw a real in [0,1)
                lines[i] = qCeil(unif*max_words);
            }
            QVector<int> lines_ord(lines);
            std::sort(lines_ord.begin(),lines_ord.end());//avoid multiple look-ups in database
            QStringList mots;
            int counter=0;
            for (int i=0; i<lines_ord.size(); i++) {
                while (++counter<lines_ord[i] and !in.atEnd()) {
                    in.readLine();
                }
                mots << in.readLine();
            }
            for (int i=0; i<lines.size(); i++) {
                mdp += " - " + mots[lines_ord.indexOf(lines[i])];
            }
            mdp.remove(0,3);
            dict.close();
        }
    // password made of random characters
    } else {
        unsigned int sample, which_class, which_within;
        unsigned int max_class = mdpSecurity->currentIndex();
        for (int i=0; i<mdpLength->value(); ++i) {
            sample = gen->generate() % cumulativeListSize[max_class];
            which_class = findCharClass(sample);
            which_within = sample - cumulativeListSize[which_class] +
                    charLists[which_class].size();
            mdp.append(charLists[which_class][which_within]);
        }
    }
    resultingMdpEdit->setText(mdp);
}

void MDPWindow::updateSecurityInfos()
{
    unsigned int classSize;
    if (mdpSecurity->currentIndex() == 4) {
        if (chooseFrench->isChecked()) {
            classSize = MAX_FRENCH;
        } else if (chooseEnglish->isChecked()) {
            classSize = MAX_ENGLISH;
        } else {
            classSize = MAX_GERMAN;
        }
    } else {
        classSize = cumulativeListSize[mdpSecurity->currentIndex()];
    }
    double nAttempts = qPow(classSize,mdpLength->value())/2;// cf. birthday paradox
    int entropy = qFloor(log2(nAttempts));
    double breaktime = nAttempts / PASSWORDS_PER_SEC;
    QString timeUnit=tr("secondes");
    QString assessment=tr("excellente", "quality assessment");
    const double year = 365 * 24 * 60 * 60;
    if (breaktime < year) {
        assessment = tr("basse", "quality assessment");
        if (breaktime > 24 * 60 * 60) {
            breaktime /= 24 * 60 * 60;
            timeUnit = tr("jours");
        } else if (breaktime > 60 * 60) {
            breaktime /= 60 * 60;
            timeUnit = tr("heures");
        } else if (breaktime > 60) {
            breaktime /= 60;
            timeUnit = tr("minutes");
        }
    } else if (breaktime < 1000*year) {
        assessment = tr("bonne", "quality assessment");
        breaktime = breaktime / year;
        timeUnit = tr("années");
    } else {
        breaktime = breaktime / year;
        timeUnit = tr("années");
    }

    // convert 'e' notation to '×10' notation and remove leading zeroes in exponent
    QString nAttemptsTxt = QLocale(QLocale::French).toString(nAttempts,'g',2);
    nAttemptsTxt.replace(QRegExp("e\\+0?([1-9][0-9]*)$"), "×10<sup>\\1</sup>");
    resultingAttempts->setText(nAttemptsTxt);
    QString breaktimeMsg = (timeUnit == tr("années")) & (breaktime > 1e6) ?
                tr("> 1 mio", "larger than one million (years)") :
                QLocale(QLocale::French).toString((int)breaktime);
    resultingTimeToBreak->setText(breaktimeMsg + " " + timeUnit);
    resultingBitEntropy->setText(QString::number(entropy) + " " + tr("bits"));
    resultingAssessment->setText(assessment);
}

void MDPWindow::changeMode(int index)
{
    if(index < 4){
        // avoid resetting spinbox when changing between similar options
        if (mdpLanguage->isEnabled()) {
            mdpLength->setSuffix(" " + tr("caractères"));
            mdpLength->setRange(5,35);
            mdpLength->setValue(10);
            mdpLanguage->setDisabled(true);
        }
        if(index == 0){
            generateMdpButton->setStyleSheet("QPushButton{ background-image: url(:/abc.png);"
                                             "background-position: center center;"
                                             "background-repeat: no-repeat; }");
        }else if(index == 1){
            generateMdpButton->setStyleSheet("QPushButton{ background-image: url(:/aBc.png);"
                                             "background-position: center center;"
                                             "background-repeat: no-repeat; }");
        }else if(index == 2){
            generateMdpButton->setStyleSheet("QPushButton{ background-image: url(:/aB6.png);"
                                             "background-position: center center;"
                                             "background-repeat: no-repeat; }");
        }else{
            generateMdpButton->setStyleSheet("QPushButton{ background-image: url(:/ad6.png);"
                                             "background-position: center center;"
                                             "background-repeat: no-repeat; }");
        }
    }else{
        mdpLength->setSuffix(" mots");
        mdpLength->setRange(3,8);
        mdpLength->setValue(4);
        mdpLanguage->setEnabled(true);
        generateMdpButton->setStyleSheet("QPushButton{ background-image: url(:/mot.png);"
                                         "background-position: center center;"
                                         "background-repeat: no-repeat; }");
    }
}

void MDPWindow::initialiseCharLists()
{
    QStringList lower,upper,digit,special;
    QChar a;
    cumulativeListSize = QVector<unsigned int>(4, 0);

    for (int i=33; i<128; i++) {
        a = QChar(i);
        switch (a.category()) {
        case QChar::Letter_Lowercase:
            lower.append(a);
            break;
        case QChar::Letter_Uppercase:
            upper.append(a);
            break;
        case QChar::Number_DecimalDigit:
            digit.append(a);
            break;
        case QChar::Other_Control:
            break;
        case QChar::Symbol_Modifier:
            break;
        default:
            special.append(a);
            break;
        }
    }
    charLists.clear();
    charLists.push_back(lower);
    charLists.push_back(upper);
    charLists.push_back(digit);
    charLists.push_back(special);

    for (int i=0; i<4; i++) {
        cumulativeListSize[i] = charLists[i].size();
        if (i > 0)
            cumulativeListSize[i] += cumulativeListSize[i-1];
    }
}

unsigned int MDPWindow::countNumberOfWords(const QString &file) const
{
    QFile ff(file);
    unsigned int counter=0;
    if(ff.open(QFile::ReadOnly)){
        QTextStream in(&ff);
        while(!in.atEnd()){
            in.readLine();
            counter++;
        }
    }
    ff.close();
    return counter;
}

unsigned int MDPWindow::findCharClass(const unsigned int &i) const
{
    for (unsigned int j=0; j<3; j++) {
        if (i < cumulativeListSize[j])
            return j;
    }
    return 3;
}

void MDPWindow::initialiseMenuBar(){
    aboutMDPCreatorAct = new QAction(tr("À propos de &MPDCreator..."), this);
    aboutQtAct         = new QAction(tr("À propos de &Qt..."), this);
    connect(aboutMDPCreatorAct, SIGNAL(triggered()), this, SLOT(aboutMDPCreator()));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // build menu
    aboutMenu = menuBar()->addMenu(tr("À propos"));
    aboutMenu->addAction(aboutQtAct);
    aboutMenu->addAction(aboutMDPCreatorAct);
}

void MDPWindow::aboutMDPCreator(){
QString title = tr("MDPCreator version 2.2");
QString text  = tr("<strong>MDPCreator version 2.2</strong><br/><br/>"
            "Imaginé et créé par Thomas Lugrin © 2014-2021 GPL-2+<br/><br/>"
            "Les figures noir et blanc proviennent de <em>Xinh Studio</em> et sont sous la licence<br/>"
            "<a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 Unported Licence</a>.<br/><br/>"
            "Les bases de données de mots français, anglais et allemands proviennent de"
            "<ul><li><a href='http://www.lexique.org'>Lexique 3.83</a>,</li>"
            "<li><a href='https://www.diginoodles.com/projects/eowl'>English Open Word List</a> "
            "basé sur le <a href='http://www.crosswordman.com/wordlist.html'>UK Advanced Cryptics Dictionary, et</li>"
            "<li><a href='http://www.ids-mannheim.de/kl/derewo/'>Korpusbasierte Grundformenliste DeReWo</a>,</li></ul>"
            "et sont respectivement sous la licence <a href='https://creativecommons.org/licenses/by-sa/4.0/'>"
            "Creative Commons Attribution-ShareAlike 4.0 International</a>, "
            "une licence de logiciel libre disponible <a href='https://www.diginoodles.com/projects/eowl'>en ligne</a>, et "
            "la licence <a href='http://creativecommons.org/licenses/by-nc/3.0/deed.de'>"
            "Attribution-NonCommercial 3.0 Unported</a>.<br/><br/>"
            "Les licences respectives sont également disponibles dans les sources de ce programme.<br/><br/>"
            "Les listes francophone et germanophone originales ont été modifiées en les restreignant "
            "aux seules listes de mots, sans les informations supplémentaires (catégorie grammaticale, fréquence) ; "
            "les doublons apparaissant après cette manipulation ont été éliminés.");
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/icon.ico"));
    QSpacerItem* horizontalSpacer = new QSpacerItem(1000, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
}

void MDPWindow::aboutQt(){
    QMessageBox::aboutQt(this);
}
