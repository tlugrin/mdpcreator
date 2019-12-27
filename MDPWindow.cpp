#include "MDPWindow.h"
#include <cstdlib>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QtCore/qmath.h>

MDPWindow::MDPWindow(QWidget* parent) : QWidget(parent)
{
    QFont editFont("", 13, QFont::Normal);

    QTime time(QTime::currentTime());
    qsrand(time.msec());

    initialiseCharLists();

    this->setWindowTitle("MDPCreator 1.0");
    this->setWindowIcon(QIcon(":/icon.jpg"));

    QVBoxLayout* securityLayout = new QVBoxLayout;
    mdpSecurityLabel = new QLabel("Niveau de sécurité :", this);
    mdpSecurity = new QComboBox(this);
    QIcon lowIcon(":/low.jpg");
    QIcon midelIcon(":/mid.jpg");
    QIcon highIcon(":/high.jpg");
    QIcon veryHighIcon(":/vhigh.jpg");
    mdpSecurity->addItem(lowIcon, "Normal");
    mdpSecurity->addItem(midelIcon, "Moyen");
    mdpSecurity->addItem(highIcon, "Haut");
    mdpSecurity->addItem(veryHighIcon, "Très élevé");
    mdpSecurity->setFont(editFont);
    securityLayout->addWidget(mdpSecurityLabel);
    securityLayout->addWidget(mdpSecurity);

    QVBoxLayout* lengthLayout = new QVBoxLayout;
    mdpLengthLabel = new QLabel("Longueur du mot de passe :", this);
    mdpLength = new QSpinBox(this);
    mdpLength->setRange(5, 35);
    mdpLength->setSuffix(" caractères");
    mdpLength->setFont(editFont);
    lengthLayout->addWidget(mdpLengthLabel);
    lengthLayout->addWidget(mdpLength);

    generateMdpButton = new QPushButton("Générer !", this);
    generateMdpButton->setFont(editFont);

    QHBoxLayout* resultingMdpLayout = new QHBoxLayout;
    resultingMdp = new QLabel("Mot de passe généré :", this);
    resultingMdpEdit = new QLineEdit(this);
    resultingMdpEdit->setReadOnly(true);
    resultingMdpEdit->setFont(editFont);
    resultingMdpLayout->addWidget(resultingMdp);
    resultingMdpLayout->addWidget(resultingMdpEdit);

    QVBoxLayout* generalLayout = new QVBoxLayout;
    generalLayout->addLayout(securityLayout);
    generalLayout->setAlignment(securityLayout,Qt::AlignHCenter);
    generalLayout->addLayout(lengthLayout);
    generalLayout->setAlignment(lengthLayout,Qt::AlignHCenter);
    generalLayout->addWidget(generateMdpButton);
    generalLayout->setAlignment(generateMdpButton,Qt::AlignHCenter);
    generalLayout->addLayout(resultingMdpLayout);
    generalLayout->setAlignment(resultingMdpLayout,Qt::AlignHCenter);
    this->setLayout(generalLayout);

    connect(generateMdpButton,SIGNAL(clicked()),this,SLOT(generateMdp()));
}

void MDPWindow::generateMdp()
{
    QString mdp;
    double rand_max = RAND_MAX;
    //param tres de la Gaussienne
    double mu = 2;
    double pi = 3.1415926536;
    double sig_sq = 1/(8*pi);//pour avoir un max == 2
    QVector<double> prob;
    for(int i(0);i<mdpSecurity->currentIndex()+1;++i){
        double u = qrand()/(0.5*rand_max);
        do{
            u = qSqrt(-2*sig_sq*qLn(qSqrt(2*pi*sig_sq)*u))+mu;
            prob.push_back(u);
        }while(u <= 0);
    }
    for(int i(0);i<mdpLength->value();++i){
        unsigned int nb = qrand();
        //nb %= index1;
        //mdp.append(charList[nb]);
        double choix_old = -1;
        double choix_new;
        unsigned int choix = 0;
        for(int j(0);j<prob.size();++j){
            choix_new = qrand()*prob[j];
            if(choix_new > choix_old) {choix = j;}
            choix_old = choix_new;
        }
        nb %= charLists[choix].size();
        mdp.append(charLists[choix][nb]);
    }
    resultingMdpEdit->setText(mdp);
}

void MDPWindow::initialiseCharLists()
{
    QStringList chars;
    chars <<"a"<<"b"<<"c"<<"d"<<"e"<<"f"<<"g"<<"h"<<"i"<<"j"<<"k"<<"l"<<"m"<<"n"<<"o"<<"p"<<"q"<<"r"<<"s"<<"t"<<"u"<<"v"<<"w"<<"x"<<"y"<<"z";
    charLists.clear();
    charLists.push_back(chars);
    chars.clear();
    chars <<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"<<"Y"<<"Z";
    charLists.push_back(chars);
    chars.clear();
    chars <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9";
    charLists.push_back(chars);
    chars.clear();
    chars <<"°"<<"+"<<"*"<<""<<"%"<<"&"<<"/"<<"("<<")"<<"="<<"?"<<"!"<<""<<"@"<<"#"<<"<"<<">"<<","<<";"<<"."<<":"<<"-";
    charLists.push_back(chars);
}
