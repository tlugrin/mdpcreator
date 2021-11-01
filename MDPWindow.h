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
        QPushButton* generateMdpButton;
        QLabel* resultingMdp;
        QLineEdit* resultingMdpEdit;
        QLabel* resultingAttempts;
        QLabel* resultingTimeToBreak;
        QLabel* resultingBitEntropy;
        QLabel* resultingAssessment;
        const QString resultingAttemptsText="Nombre moyen de tentatives : ";
        const QString resultingTimeToBreakText="Durée moyenne d'une attaque : ";
        const QString resultingBitEntropyText="Entropie : ";
        const QString resultingAssessmentText="Qualité : ";
        // number of passwords an attacker could supposedly try per second
        const unsigned int PASSWORDS_PER_SEC=qPow(10,9);
        // number of lines in French and English databases
        unsigned int MAX_FRENCH;
        unsigned int MAX_ENGLISH;
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
