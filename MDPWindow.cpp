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
    mdpSecurityLabel = new QLabel("Niveau de sécurité :", this);
    mdpSecurity = new QComboBox(this);
    QIcon lowIcon(":/low.png");
    QIcon midelIcon(":/mid.png");
    QIcon highIcon(":/high.png");
    QIcon veryHighIcon(":/vhigh.png");
    QIcon veryHighEasyIcon(":/vhigheasy.png");
    mdpSecurity->addItem(lowIcon, "Normal");
    mdpSecurity->addItem(midelIcon, "Moyen");
    mdpSecurity->addItem(highIcon, "Élevé");
    mdpSecurity->addItem(veryHighIcon, "Très élevé");
    mdpSecurity->addItem(veryHighEasyIcon, "Très élevé ET facile à retenir");
    mdpSecurity->setIconSize(QSize(48,48));
    securityLayout->addWidget(mdpSecurityLabel);
    securityLayout->addWidget(mdpSecurity);

    // spin box: length of password
    QVBoxLayout* lengthLayout = new QVBoxLayout;
    mdpLengthLabel = new QLabel("Longueur du mot de passe :", this);
    mdpLength = new QSpinBox(this);
    lengthLayout->addWidget(mdpLengthLabel);
    lengthLayout->addWidget(mdpLength);

    // if password with words: choose French or English dictionary
    mdpLanguage = new QGroupBox("Langue");
    chooseFrench  = new QRadioButton("Français");
    chooseEnglish = new QRadioButton("Anglais");
    chooseGerman = new QRadioButton("Allemand");
    chooseFrench->setChecked(true);
    QVBoxLayout *languageLayout = new QVBoxLayout;
    languageLayout->addWidget(chooseFrench);
    languageLayout->addWidget(chooseEnglish);
    languageLayout->addWidget(chooseGerman);
    mdpLanguage->setLayout(languageLayout);

    // button + text field: generate and see password
    generateMdpButton = new QPushButton("Générer !");
    generateMdpButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    changeMode(0);

    QVBoxLayout* resultingMdpLayout = new QVBoxLayout;
    resultingMdp = new QLabel("Mot de passe généré :", this);
    resultingMdpEdit = new QLineEdit(this);
    resultingMdpEdit->setReadOnly(true);
    resultingMdpLayout->addWidget(resultingMdp);
    resultingMdpLayout->addWidget(resultingMdpEdit);

    QGridLayout* generalLayout = new QGridLayout;
    generalLayout->addLayout(securityLayout,0,0,1,2);
    generalLayout->addLayout(lengthLayout,1,0,1,1,Qt::AlignTop);
    generalLayout->addWidget(mdpLanguage,1,1,1,1);
    generalLayout->addWidget(generateMdpButton,0,2,2,1);
    generalLayout->addLayout(resultingMdpLayout,2,0,1,3);
    widget->setLayout(generalLayout);

    connect(generateMdpButton,SIGNAL(clicked()),this,SLOT(generateMdp()));
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
            for (int i=0; i<lines.size(); i++) {
                unif = gen->generateDouble();// draw a real in [0,1)
                if (chooseFrench->isChecked()) {
                    lines[i] = qCeil(unif*MAX_FRENCH);
                } else {
                    lines[i] = qCeil(unif*MAX_ENGLISH);
                }
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
        for (int i=0; i<mdpLength->value(); ++i) {
            unsigned int max_class = mdpSecurity->currentIndex();
            unsigned int sample = gen->generate() % cumulativeListSize[max_class];
            unsigned int which_class = findCharClass(sample);
            unsigned int which_within = sample -
                    cumulativeListSize[which_class] +
                    charLists[which_class].size();
            mdp.append(charLists[which_class][which_within]);
        }
    }
    resultingMdpEdit->setText(mdp);
}

void MDPWindow::changeMode(int index)
{
    if(index < 4){
        mdpLength->setSuffix(" caractères");
        mdpLength->setRange(5,35);
        mdpLength->setValue(7);
        mdpLanguage->setDisabled(true);
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
        mdpLength->setValue(3);
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
    aboutMDPCreatorAct = new QAction("À propos de &MPDCreator...", this);
    aboutQtAct         = new QAction("À propos de &Qt...", this);
    connect(aboutMDPCreatorAct, SIGNAL(triggered()), this, SLOT(aboutMDPCreator()));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // build menu
    aboutMenu = menuBar()->addMenu("À propos");
    aboutMenu->addAction(aboutQtAct);
    aboutMenu->addAction(aboutMDPCreatorAct);
}

void MDPWindow::aboutMDPCreator(){
QString title = "MDPCreator version 2.2";
QString text  = "<strong>MDPCreator version 2.2</strong><br/><br/>"
            "Imaginé et créé par Thomas Lugrin © 2014-2021 GPL-2+<br/><br/>"
            "Les figures noir et blanc proviennent de <em>Xinh Studio</em> et sont sous la licence<br/>"
            "<a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 Unported Licence</a>.<br/><br/>"
            "Les bases de données de mots français, anglais et allemands proviennent de"
            "<ul><li><a href='http://www.lexique.org'>Lexique 3.82</a>,</li>"
            "<li><a href='https://www.diginoodles.com/projects/eowl'>English Open Word List</a> "
            "basé sur le <a href='http://www.crosswordman.com/wordlist.html'>UK Advanced Cryptics Dictionary, et</li>"
            "<li><a href='http://www.ids-mannheim.de/kl/derewo/'>Korpusbasierte Grundformenliste DeReWo</a>,</li></ul>"
            "et sont respectivement sous la licence <a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 Unported Licence</a>, "
            "une licence de logiciel libre disponible <a href='https://www.diginoodles.com/projects/eowl'>en ligne</a>, et "
            "la licence <a href='http://creativecommons.org/licenses/by-nc/3.0/deed.de'>Creative Commons (by-nc)</a>.<br/><br/>"
            "Les licences respectives sont également disponibles dans les sources de ce programme.";
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
