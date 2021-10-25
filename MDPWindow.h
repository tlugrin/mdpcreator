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
        void changeMode(int index);
        // menu actions
        void aboutMDPCreator();
        void aboutQt();

    public:
        MDPWindow(QWidget* parent =0);
};

#endif // MDPWINDOW_H
