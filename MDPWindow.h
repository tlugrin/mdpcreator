#ifndef MDPWINDOW_H
#define MDPWINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include <QStringList>

class MDPWindow : public QWidget
{
    Q_OBJECT

    private:
        QVector<QStringList> charLists;
        QLabel* mdpSecurityLabel;
        QComboBox* mdpSecurity;
        QLabel* mdpLengthLabel;
        QSpinBox* mdpLength;
        QPushButton* generateMdpButton;
        QLabel* resultingMdp;
        QLineEdit* resultingMdpEdit;

        //void initialiseCharList();
        void initialiseCharLists();

    protected slots:
        void generateMdp();

    public:
        MDPWindow(QWidget* parent =0);
};

#endif // MDPWINDOW_H
